/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "stdafx.h"
#include <hb-ot.h>
#include <unicode/brkiter.h>
#include <unicode/locid.h>
#include <unicode/stringpiece.h>
#include <unicode/ubidi.h>
#include <unicode/uchriter.h>
#include <unicode/unistr.h>
#include <unicode/uscript.h>

#include "SkFontMgr.h"
#include "SkOnce.h"
#include "SkShaper.h"
#include "SkStream.h"
#include "skia/src/core/SkTDPQueue.h"
#include "SkTLazy.h"
#include "SkTemplates.h"
#include "SkTextBlob.h"
#include "SkTo.h"
#include "SkTypeface.h"

//#include "win32/windows.h"

#define ICU_UTIL_DATA_SYMBOL "icudt" U_ICU_VERSION_SHORT "_dat"
#define ICU_UTIL_DATA_SHARED_MODULE_NAME _T("icudt62.dll")

inline void SkLoadICU()
{
    HMODULE module = LoadLibrary(ICU_UTIL_DATA_SHARED_MODULE_NAME);
    if (!module)
    {
        SK_ABORT("Failed to load " ICU_UTIL_DATA_SHARED_MODULE_NAME "\n");
    }
    FARPROC addr = GetProcAddress(module, ICU_UTIL_DATA_SYMBOL);
    if (!addr)
    {
        SK_ABORT("Symbol " ICU_UTIL_DATA_SYMBOL " missing in "
            ICU_UTIL_DATA_SHARED_MODULE_NAME ".\n");
    }
    UErrorCode err = U_ZERO_ERROR;
    //udata_setCommonData(reinterpret_cast<void*>(addr), &err);
    if (err != U_ZERO_ERROR)
    {
        SkDebugf("udata_setCommonData() returned %d.\n", (int)err);
        SK_ABORT("");
    }
    //udata_setFileAccess(UDATA_ONLY_PACKAGES, &err);
    if (err != U_ZERO_ERROR)
    {
        SkDebugf("udata_setFileAccess() returned %d.\n", (int)err);
        SK_ABORT("");
    }
}

SkUnichar SkUTF8_NextUnichar(const char ** ptr)
{
    SkASSERT(ptr && *ptr);

    const uint8_t * p = (const uint8_t*)*ptr;
    int c = *p;
    int hic = c << 24;

    if (hic < 0)
    {
        uint32_t mask = (uint32_t)~0x3F;
        hic = SkLeftShift(hic, 1);
        do
        {
            c = (c << 6) | (*++p & 0x3F);
            mask <<= 5;
        }
        while ((hic = SkLeftShift(hic, 1)) < 0);
        c &= ~mask;
    }
    *ptr = (char*)p + 1;
    return c;
}

#define SkUTF16_IsHighSurrogate(c)  (((c) & 0xFC00) == 0xD800)
#define SkUTF16_IsLowSurrogate(c)   (((c) & 0xFC00) == 0xDC00)

size_t SkUTF16_FromUnichar(SkUnichar uni, uint16_t dst[])
{
    SkASSERT((unsigned)uni <= 0x10FFFF);

    int extra = (uni > 0xFFFF);

    if (dst)
    {
        if (extra)
        {
            // dst[0] = SkToU16(0xD800 | ((uni - 0x10000) >> 10));
            // dst[0] = SkToU16(0xD800 | ((uni >> 10) - 64));
            dst[0] = SkToU16((0xD800 - 64) + (uni >> 10));
            dst[1] = SkToU16(0xDC00 | (uni & 0x3FF));

            SkASSERT(SkUTF16_IsHighSurrogate(dst[0]));
            SkASSERT(SkUTF16_IsLowSurrogate(dst[1]));
        }
        else
        {
            dst[0] = SkToU16(uni);
            SkASSERT(!SkUTF16_IsHighSurrogate(dst[0]));
            SkASSERT(!SkUTF16_IsLowSurrogate(dst[0]));
        }
    }
    return 1 + extra;
}


namespace skshaper
{
    template<class T, void(*P)(T *)>
    using resource = std::unique_ptr<T, SkFunctionWrapper<void, T, P>>;
    using HBBlob = resource<hb_blob_t, hb_blob_destroy>;
    using HBFace = resource<hb_face_t, hb_face_destroy>;
    using HBFont = resource<hb_font_t, hb_font_destroy>;
    using HBBuffer = resource<hb_buffer_t, hb_buffer_destroy>;
    using ICUBiDi = resource<UBiDi, ubidi_close>;

    HBBlob stream_to_blob(std::unique_ptr<SkStreamAsset> asset)
    {
        size_t size = asset->getLength();
        HBBlob blob;
        if (const void * base = asset->getMemoryBase())
        {
            blob.reset(hb_blob_create((char*)base, SkToUInt(size),
                HB_MEMORY_MODE_READONLY, asset.release(),
                [](void * p) { delete (SkStreamAsset*)p; }));
        }
        else
        {
            // SkDebugf("Extra SkStreamAsset copy\n");
            void * ptr = size ? sk_malloc_throw(size) : nullptr;
            asset->read(ptr, size);
            blob.reset(hb_blob_create((char*)ptr, SkToUInt(size),
                HB_MEMORY_MODE_READONLY, ptr, sk_free));
        }
        SkASSERT(blob);
        hb_blob_make_immutable(blob.get());
        return blob;
    }

    HBFont create_hb_font(SkTypeface * tf)
    {
        int index;
        HBBlob blob(stream_to_blob(std::unique_ptr<SkStreamAsset>(tf->openStream(&index))));
        HBFace face(hb_face_create(blob.get(), (unsigned)index));
        SkASSERT(face);
        if (!face) { return nullptr; }
        hb_face_set_index(face.get(), (unsigned)index);
        hb_face_set_upem(face.get(), tf->getUnitsPerEm());

        HBFont font(hb_font_create(face.get()));
        SkASSERT(font);
        if (!font) { return nullptr; }
        hb_ot_font_set_funcs(font.get());
        int axis_count = tf->getVariationDesignPosition(nullptr, 0);
        if (axis_count > 0)
        {
            SkAutoSTMalloc<4, SkFontArguments::VariationPosition::Coordinate> axis_values(axis_count);
            if (tf->getVariationDesignPosition(axis_values, axis_count) == axis_count)
            {
                hb_font_set_variations(font.get(),
                    reinterpret_cast<hb_variation_t*>(axis_values.get()),
                    axis_count);
            }
        }
        return font;
    }

    class RunIterator
    {
    public:
        virtual ~RunIterator() {}
        virtual void run() = 0;
        // Pointer one past the last (utf8) element in the current run.
        virtual const char * right() const = 0;
        virtual bool eof() const = 0;
        bool operator<(const RunIterator & that) const { return this->right() < that.right(); }
    };

    class BiDiRunIterator : public RunIterator
    {
    public:
        static SkTLazy<BiDiRunIterator> Make(const char * utf8, size_t utf8Bytes, UBiDiLevel level)
        {
            SkTLazy<BiDiRunIterator> ret;

            // ubidi only accepts utf16 (though internally it basically works on utf32 chars).
            // We want an ubidi_setPara(UBiDi*, UText*, UBiDiLevel, UBiDiLevel*, UErrorCode*);
            if (!SkTFitsIn<int32_t>(utf8Bytes))
            {
                SkDebugf("Bidi error: text too long");
                return ret;
            }
            icu::UnicodeString utf16 = icu::UnicodeString::fromUTF8(icu::StringPiece(utf8, utf8Bytes));

            UErrorCode status = U_ZERO_ERROR;
            ICUBiDi bidi(ubidi_openSized(utf16.length(), 0, &status));
            if (U_FAILURE(status))
            {
                SkDebugf("Bidi error: %s", u_errorName(status));
                return ret;
            }
            SkASSERT(bidi);

            // The required lifetime of utf16 isn't well documented.
            // It appears it isn't used after ubidi_setPara except through ubidi_getText.
            ubidi_setPara(bidi.get(), utf16.getBuffer(), utf16.length(), level, nullptr, &status);
            if (U_FAILURE(status))
            {
                SkDebugf("Bidi error: %s", u_errorName(status));
                return ret;
            }

            ret.init(utf8, std::move(bidi));
            return ret;
        }

        BiDiRunIterator(const char * utf8, ICUBiDi bidi)
            : fBidi(std::move(bidi))
              , fEndOfCurrentRun(utf8)
              , fUTF16LogicalPosition(0)
              , fLevel(UBIDI_DEFAULT_LTR) {}

        void run() override
        {
            SkASSERT(fUTF16LogicalPosition < ubidi_getLength(fBidi.get()));
            int32_t endPosition = ubidi_getLength(fBidi.get());
            fLevel = ubidi_getLevelAt(fBidi.get(), fUTF16LogicalPosition);
            SkUnichar u = SkUTF8_NextUnichar(&fEndOfCurrentRun);
            fUTF16LogicalPosition += SkUTF16_FromUnichar(u, nullptr);
            UBiDiLevel level;
            while (fUTF16LogicalPosition < endPosition)
            {
                level = ubidi_getLevelAt(fBidi.get(), fUTF16LogicalPosition);
                if (level != fLevel)
                {
                    break;
                }
                u = SkUTF8_NextUnichar(&fEndOfCurrentRun);
                fUTF16LogicalPosition += SkUTF16_FromUnichar(u, nullptr);
            }
        }

        const char * right() const override { return fEndOfCurrentRun; }
        bool eof() const override { return fUTF16LogicalPosition == ubidi_getLength(fBidi.get()); }

        UBiDiLevel currentLevel() const { return fLevel; }
    private:
        ICUBiDi fBidi;
        const char * fEndOfCurrentRun;
        int32_t fUTF16LogicalPosition;
        UBiDiLevel fLevel;
    };

    class ScriptRunIterator : public RunIterator
    {
    public:
        static SkTLazy<ScriptRunIterator> Make(const char * utf8, size_t utf8Bytes,
            hb_unicode_funcs_t * hbUnicode)
        {
            SkTLazy<ScriptRunIterator> ret;
            ret.init(utf8, utf8Bytes, hbUnicode);
            return ret;
        }

        ScriptRunIterator(const char * utf8, size_t utf8Bytes, hb_unicode_funcs_t * hbUnicode)
            : fCurrent(utf8), fEnd(fCurrent + utf8Bytes)
              , fHBUnicode(hbUnicode)
              , fCurrentScript(HB_SCRIPT_UNKNOWN) {}

        void run() override
        {
            SkASSERT(fCurrent < fEnd);
            SkUnichar u = SkUTF8_NextUnichar(&fCurrent);
            fCurrentScript = hb_unicode_script(fHBUnicode, u);
            while (fCurrent < fEnd)
            {
                const char * prev = fCurrent;
                u = SkUTF8_NextUnichar(&fCurrent);
                const hb_script_t script = hb_unicode_script(fHBUnicode, u);
                if (script != fCurrentScript)
                {
                    if (fCurrentScript == HB_SCRIPT_INHERITED || fCurrentScript == HB_SCRIPT_COMMON)
                    {
                        fCurrentScript = script;
                    }
                    else if (script == HB_SCRIPT_INHERITED || script == HB_SCRIPT_COMMON) { continue; }
                    else
                    {
                        fCurrent = prev;
                        break;
                    }
                }
            }
            if (fCurrentScript == HB_SCRIPT_INHERITED) { fCurrentScript = HB_SCRIPT_COMMON; }
        }

        const char * right() const override { return fCurrent; }
        bool eof() const override { return fCurrent == fEnd; }

        hb_script_t currentScript() const { return fCurrentScript; }
    private:
        const char * fCurrent;
        const char * fEnd;
        hb_unicode_funcs_t * fHBUnicode;
        hb_script_t fCurrentScript;
    };

    class FontRunIterator : public RunIterator
    {
    public:
        static SkTLazy<FontRunIterator> Make(const char * utf8, size_t utf8Bytes,
            sk_sp<SkTypeface> typeface,
            hb_font_t * hbFace,
            sk_sp<SkFontMgr> fallbackMgr)
        {
            SkTLazy<FontRunIterator> ret;
            ret.init(utf8, utf8Bytes, std::move(typeface), hbFace, std::move(fallbackMgr));
            return ret;
        }

        FontRunIterator(const char * utf8, size_t utf8Bytes, sk_sp<SkTypeface> typeface,
            hb_font_t * hbFace, sk_sp<SkFontMgr> fallbackMgr)
            : fCurrent(utf8), fEnd(fCurrent + utf8Bytes)
              , fFallbackMgr(std::move(fallbackMgr))
              , fHBFont(hbFace), fTypeface(std::move(typeface))
              , fFallbackHBFont(nullptr), fFallbackTypeface(nullptr)
              , fCurrentHBFont(fHBFont), fCurrentTypeface(fTypeface.get()) {}

        void run() override
        {
            SkASSERT(fCurrent < fEnd);
            SkUnichar u = SkUTF8_NextUnichar(&fCurrent);
            // If the starting typeface can handle this character, use it.
            if (fTypeface->charsToGlyphs(&u, SkTypeface::kUTF32_Encoding, nullptr, 1))
            {
                fFallbackTypeface.reset();
                // If not, try to find a fallback typeface
            }
            else
            {
                fFallbackTypeface.reset(fFallbackMgr->matchFamilyStyleCharacter(
                    nullptr, fTypeface->fontStyle(), nullptr, 0, u));
            }

            if (fFallbackTypeface)
            {
                fFallbackHBFont = create_hb_font(fFallbackTypeface.get());
                fCurrentTypeface = fFallbackTypeface.get();
                fCurrentHBFont = fFallbackHBFont.get();
            }
            else
            {
                fFallbackHBFont.reset();
                fCurrentTypeface = fTypeface.get();
                fCurrentHBFont = fHBFont;
            }

            while (fCurrent < fEnd)
            {
                const char * prev = fCurrent;
                u = SkUTF8_NextUnichar(&fCurrent);

                // If using a fallback and the initial typeface has this character, stop fallback.
                if (fFallbackTypeface &&
                    fTypeface->charsToGlyphs(&u, SkTypeface::kUTF32_Encoding, nullptr, 1))
                {
                    fCurrent = prev;
                    return;
                }
                // If the current typeface cannot handle this character, stop using it.
                if (!fCurrentTypeface->charsToGlyphs(&u, SkTypeface::kUTF32_Encoding, nullptr, 1))
                {
                    fCurrent = prev;
                    return;
                }
            }
        }

        const char * right() const override { return fCurrent; }
        bool eof() const override { return fCurrent == fEnd; }

        SkTypeface * currentTypeface() const { return fCurrentTypeface; }
        hb_font_t * currentHBFont() const { return fCurrentHBFont; }
    private:
        const char * fCurrent;
        const char * fEnd;
        sk_sp<SkFontMgr> fFallbackMgr;
        hb_font_t * fHBFont;
        sk_sp<SkTypeface> fTypeface;
        HBFont fFallbackHBFont;
        sk_sp<SkTypeface> fFallbackTypeface;
        hb_font_t * fCurrentHBFont;
        SkTypeface * fCurrentTypeface;
    };

    class RunIteratorQueue
    {
    public:
        void insert(RunIterator * runIterator) { fIterators.insert(runIterator); }

        bool advanceRuns()
        {
            const RunIterator * iterator = fIterators.peek();
            if (iterator->eof())
            {
                SkASSERT(this->allRunsAreAtEnd());
                return false;
            }
            const char * leastEnd = iterator->right();
            RunIterator * currentRun = nullptr;
            SkDEBUGCODE(const char* previousEndOfCurrentRun);
            while ((currentRun = fIterators.peek())->right() <= leastEnd)
            {
                fIterators.pop();
                SkDEBUGCODE(previousEndOfCurrentRun = currentRun->right());
                currentRun->run();
                SkASSERT(previousEndOfCurrentRun < currentRun->right());
                fIterators.insert(currentRun);
            }
            return true;
        }

        const char * endOfCurrentRun() const { return fIterators.peek()->right(); }

    private:
        bool allRunsAreAtEnd() const
        {
            for (int i = 0; i < fIterators.count(); ++i) { if (!fIterators.at(i)->eof()) { return false; } }
            return true;
        }

        static bool CompareRunIterator(RunIterator * const& a, RunIterator * const& b) { return *a < *b; }
        SkTDPQueue<RunIterator*, CompareRunIterator> fIterators;
    };

    struct ShapedGlyph
    {
        SkGlyphID fID;
        uint32_t fCluster;
        SkPoint fOffset;
        SkVector fAdvance;
        bool fMayLineBreakBefore;
        bool fMustLineBreakBefore;
        bool fHasVisual;
    };

    struct ShapedRun
    {
        ShapedRun(const char * utf8Start, const char * utf8End, int numGlyphs, const SkPaint & paint,
            UBiDiLevel level, std::unique_ptr<ShapedGlyph[]> glyphs)
            : fUtf8Start(utf8Start), fUtf8End(utf8End), fNumGlyphs(numGlyphs), fPaint(paint)
              , fLevel(level), fGlyphs(std::move(glyphs)) {}

        const char * fUtf8Start;
        const char * fUtf8End;
        int fNumGlyphs;
        SkPaint fPaint;
        UBiDiLevel fLevel;
        std::unique_ptr<ShapedGlyph[]> fGlyphs;
    };

    static constexpr bool is_LTR(UBiDiLevel level) { return (level & 1) == 0; }

    static void append(SkTextBlobBuilder & builder, const ShapedRun & run, int glyphStart, int glyphEnd, SkPoint & point)
    {
        int len = glyphEnd - glyphStart;
        SkTextBlobBuilder::RunBuffer runBuffer = builder.allocRunTextPos(run.fPaint, len, run.fUtf8End - run.fUtf8Start, SkString());
        memcpy(runBuffer.utf8text, run.fUtf8Start, run.fUtf8End - run.fUtf8Start);

        for (int iglyph = 0; iglyph < len; ++iglyph)
        {
            // Glyphs are in logical order, but output ltr since PDF readers seem to expect that.
            const ShapedGlyph & glyph = run.fGlyphs[is_LTR(run.fLevel) ? glyphStart + iglyph : glyphEnd - 1 - iglyph];
            runBuffer.glyphs[iglyph] = glyph.fID;
            runBuffer.clusters[iglyph] = glyph.fCluster;
            runBuffer.pos[iglyph * 2 + 0] = point.fX + glyph.fOffset.fX;
            runBuffer.pos[iglyph * 2 + 1] = point.fY - glyph.fOffset.fY;
            point.fX += glyph.fAdvance.fX;
            point.fY += glyph.fAdvance.fY;
        }
    }

    struct ShapedRunGlyphIterator
    {
        ShapedRunGlyphIterator(const SkTArray<ShapedRun> & origRuns)
            : fRuns(&origRuns), fRunIndex(0), fGlyphIndex(0) { }

        ShapedRunGlyphIterator(const ShapedRunGlyphIterator & that) = default;
        ShapedRunGlyphIterator & operator=(const ShapedRunGlyphIterator & that) = default;

        bool operator==(const ShapedRunGlyphIterator & that) const
        {
            return fRuns == that.fRuns &&
                fRunIndex == that.fRunIndex &&
                fGlyphIndex == that.fGlyphIndex;
        }

        bool operator!=(const ShapedRunGlyphIterator & that) const
        {
            return fRuns != that.fRuns ||
                fRunIndex != that.fRunIndex ||
                fGlyphIndex != that.fGlyphIndex;
        }

        ShapedGlyph * next()
        {
            const SkTArray<ShapedRun> & runs = *fRuns;
            SkASSERT(fRunIndex < runs.count());
            SkASSERT(fGlyphIndex < runs[fRunIndex].fNumGlyphs);

            ++fGlyphIndex;
            if (fGlyphIndex == runs[fRunIndex].fNumGlyphs)
            {
                fGlyphIndex = 0;
                ++fRunIndex;
                if (fRunIndex >= runs.count()) { return nullptr; }
            }
            return &runs[fRunIndex].fGlyphs[fGlyphIndex];
        }

        ShapedGlyph * current()
        {
            const SkTArray<ShapedRun> & runs = *fRuns;
            if (fRunIndex >= runs.count()) { return nullptr; }
            return &runs[fRunIndex].fGlyphs[fGlyphIndex];
        }

        const SkTArray<ShapedRun> * fRuns;
        int fRunIndex;
        int fGlyphIndex;
    };
} // namespace

using namespace skshaper;

struct SkShaper::Impl
{
    HBFont fHarfBuzzFont;
    HBBuffer fBuffer;
    sk_sp<SkTypeface> fTypeface;
    std::unique_ptr<icu::BreakIterator> fBreakIterator;
};

SkShaper::SkShaper(sk_sp<SkTypeface> tf) : fImpl(new Impl)
{
    SkOnce once;
    once([] { SkLoadICU(); });

    fImpl->fTypeface = tf ? std::move(tf) : SkTypeface::MakeDefault();
    fImpl->fHarfBuzzFont = create_hb_font(fImpl->fTypeface.get());
    SkASSERT(fImpl->fHarfBuzzFont);
    fImpl->fBuffer.reset(hb_buffer_create());
    SkASSERT(fImpl->fBuffer);

    //icu::Locale thai("th");
    UErrorCode status = U_ZERO_ERROR;
    fImpl->fBreakIterator.reset(icu::BreakIterator::createLineInstance(icu::Locale::getDefault(), status));
    if (U_FAILURE(status))
    {
        SkDebugf("Could not create break iterator: %s", u_errorName(status));
        SK_ABORT("");
    }
}

SkShaper::~SkShaper() {}

bool SkShaper::good() const
{
    return fImpl->fHarfBuzzFont &&
        fImpl->fBuffer &&
        fImpl->fTypeface &&
        fImpl->fBreakIterator;
}

SkPoint SkShaper::shape(SkTextBlobBuilder & builder,
    const SkPaint & srcPaint,
    const char * utf8,
    size_t utf8Bytes,
    bool leftToRight,
    SkPoint point,
    SkScalar width, SkRect & rect) const
{
    sk_sp<SkFontMgr> fontMgr = SkFontMgr::RefDefault();
    UBiDiLevel defaultLevel = leftToRight ? UBIDI_DEFAULT_LTR : UBIDI_DEFAULT_RTL;
    //hb_script_t script = ...

    SkTArray<ShapedRun> runs;
    {
        RunIteratorQueue runSegmenter;

        SkTLazy<BiDiRunIterator> __bidi(BiDiRunIterator::Make(utf8, utf8Bytes, defaultLevel));
        BiDiRunIterator * bidi = __bidi.getMaybeNull();
        if (!bidi) return point;
        runSegmenter.insert(bidi);

        hb_unicode_funcs_t * hb_unicode = hb_buffer_get_unicode_funcs(fImpl->fBuffer.get());
        SkTLazy<ScriptRunIterator> __script(ScriptRunIterator::Make(utf8, utf8Bytes, hb_unicode));
        ScriptRunIterator * script = __script.getMaybeNull();
        if (!script) return point;
        runSegmenter.insert(script);

        SkTLazy<FontRunIterator> _fontfallback(FontRunIterator::Make(utf8, utf8Bytes, fImpl->fTypeface, fImpl->fHarfBuzzFont.get(), std::move(fontMgr)));
        FontRunIterator * font = _fontfallback.getMaybeNull();
        if (!font) return point;
        runSegmenter.insert(font);

        icu::BreakIterator & breakIterator = *fImpl->fBreakIterator;
        {
            UErrorCode status = U_ZERO_ERROR;
            UText utf8UText = UTEXT_INITIALIZER;
            utext_openUTF8(&utf8UText, utf8, utf8Bytes, &status);
            std::unique_ptr<UText, SkFunctionWrapper<UText*, UText, utext_close>> autoClose(&utf8UText);
            if (U_FAILURE(status))
            {
                SkDebugf("Could not create utf8UText: %s", u_errorName(status));
                return point;
            }
            breakIterator.setText(&utf8UText, status);
            //utext_close(&utf8UText);
            if (U_FAILURE(status))
            {
                SkDebugf("Could not setText on break iterator: %s", u_errorName(status));
                return point;
            }
        }

        const char * utf8Start = nullptr;
        const char * utf8End = utf8;
        while (runSegmenter.advanceRuns())
        {
            utf8Start = utf8End;
            utf8End = runSegmenter.endOfCurrentRun();

            hb_buffer_t * hbbuffer = fImpl->fBuffer.get();
            SkAutoTCallVProc<hb_buffer_t, hb_buffer_clear_contents> autoClearBuffer(hbbuffer);
            hb_buffer_set_content_type(hbbuffer, HB_BUFFER_CONTENT_TYPE_UNICODE);
            hb_buffer_set_cluster_level(hbbuffer, HB_BUFFER_CLUSTER_LEVEL_MONOTONE_CHARACTERS);

            // Populate the hb_buffer directly with utf8 cluster indexes.
            const char * utf8Current = utf8Start;
            while (utf8Current < utf8End)
            {
                unsigned int cluster = utf8Current - utf8Start;
                hb_codepoint_t u = SkUTF8_NextUnichar(&utf8Current);
                hb_buffer_add(hbbuffer, u, cluster);
            }

            size_t utf8runLength = utf8End - utf8Start;
            if (!SkTFitsIn<int>(utf8runLength))
            {
                SkDebugf("Shaping error: utf8 too long");
                return point;
            }
            hb_buffer_set_script(hbbuffer, script->currentScript());
            hb_direction_t direction = is_LTR(bidi->currentLevel()) ? HB_DIRECTION_LTR : HB_DIRECTION_RTL;
            hb_buffer_set_direction(hbbuffer, direction);
            // TODO: language
            hb_buffer_guess_segment_properties(hbbuffer);
            // TODO: features
            hb_shape(font->currentHBFont(), hbbuffer, nullptr, 0);
            unsigned len = hb_buffer_get_length(hbbuffer);
            if (len == 0) 
                continue;
            if (!SkTFitsIn<int>(len))
            {
                SkDebugf("Shaping error: too many glyphs");
                return point;
            }

            if (direction == HB_DIRECTION_RTL)
            {
                // Put the clusters back in logical order.
                // Note that the advances remain ltr.
                hb_buffer_reverse(hbbuffer);
            }
            hb_glyph_info_t * info = hb_buffer_get_glyph_infos(hbbuffer, nullptr);
            hb_glyph_position_t * pos = hb_buffer_get_glyph_positions(hbbuffer, nullptr);

            SkPaint paint(srcPaint);
            paint.setTextEncoding(SkPaint::kGlyphID_TextEncoding);
            paint.setTypeface(sk_ref_sp(font->currentTypeface()));
            ShapedRun & run = runs.emplace_back(utf8Start, utf8End, len, paint, bidi->currentLevel(),
                std::unique_ptr<ShapedGlyph[]>(new ShapedGlyph[len]));

            int scaleX, scaleY;
            hb_font_get_scale(font->currentHBFont(), &scaleX, &scaleY);
            double textSizeY = run.fPaint.getTextSize() / scaleY;
            double textSizeX = run.fPaint.getTextSize() / scaleX * run.fPaint.getTextScaleX();
            for (unsigned i = 0; i < len; i++)
            {
                ShapedGlyph & glyph = run.fGlyphs[i];
                glyph.fID = info[i].codepoint;
                glyph.fCluster = info[i].cluster;
                glyph.fOffset.fX = pos[i].x_offset * textSizeX;
                glyph.fOffset.fY = pos[i].y_offset * textSizeY;
                glyph.fAdvance.fX = pos[i].x_advance * textSizeX;
                glyph.fAdvance.fY = pos[i].y_advance * textSizeY;
                glyph.fHasVisual = true; //!font->currentTypeface()->glyphBoundsAreZero(glyph.fID);
                //info->mask safe_to_break;
                glyph.fMustLineBreakBefore = false;
            }

            int32_t clusterOffset = utf8Start - utf8;
            uint32_t previousCluster = 0xFFFFFFFF;
            for (unsigned i = 0; i < len; ++i)
            {
                ShapedGlyph & glyph = run.fGlyphs[i];
                int32_t glyphCluster = glyph.fCluster + clusterOffset;
                int32_t breakIteratorCurrent = breakIterator.current();
                while (breakIteratorCurrent != icu::BreakIterator::DONE && breakIteratorCurrent < glyphCluster)
                    breakIteratorCurrent = breakIterator.next();
                glyph.fMayLineBreakBefore = glyph.fCluster != previousCluster && breakIteratorCurrent == glyphCluster;
                previousCluster = glyph.fCluster;
            }
        }
    }

    // Iterate over the glyphs in logical order to mark line endings.
    {
        SkScalar widthSoFar = 0;
        bool previousBreakValid = false; // Set when previousBreak is set to a valid candidate break.
        bool canAddBreakNow = false; // Disallow line breaks before the first glyph of a run.
        ShapedRunGlyphIterator previousBreak(runs);
        ShapedRunGlyphIterator glyphIterator(runs);
        while (ShapedGlyph * glyph = glyphIterator.current())
        {
            if (canAddBreakNow && glyph->fMayLineBreakBefore)
            {
                previousBreakValid = true;
                previousBreak = glyphIterator;
            }
            SkScalar glyphWidth = glyph->fAdvance.fX;
            if (width < 0 || widthSoFar + glyphWidth < width)
            {
                widthSoFar += glyphWidth;
                glyphIterator.next();
                canAddBreakNow = true;
                continue;
            }

            if (widthSoFar == 0)
            {
                // Adding just this glyph is too much, just break with this glyph
                glyphIterator.next();
                previousBreak = glyphIterator;
            }
            else if (!previousBreakValid)
            {
                // No break opprotunity found yet, just break without this glyph
                previousBreak = glyphIterator;
            }
            glyphIterator = previousBreak;
            glyph = glyphIterator.current();
            if (glyph) { glyph->fMustLineBreakBefore = true; }
            widthSoFar = 0;
            previousBreakValid = false;
            canAddBreakNow = false;
        }
    }

    // Reorder the runs and glyphs per line and write them out.
    SkPoint currentPoint = point;
    rect.fLeft = point.fX;
    rect.fTop = point.fY;
    {
        ShapedRunGlyphIterator previousBreak(runs);
        ShapedRunGlyphIterator glyphIterator(runs);
        SkScalar maxAscent = 0;
        SkScalar maxDescent = 0;
        SkScalar maxLeading = 0;
        int previousRunIndex = -1;
        while (glyphIterator.current())
        {
            int runIndex = glyphIterator.fRunIndex;
            int glyphIndex = glyphIterator.fGlyphIndex;
            ShapedGlyph * nextGlyph = glyphIterator.next();

            if (previousRunIndex != runIndex)
            {
                SkPaint::FontMetrics metrics;
                runs[runIndex].fPaint.getFontMetrics(&metrics);
                maxAscent = SkTMin(maxAscent, metrics.fAscent);
                maxDescent = SkTMax(maxDescent, metrics.fDescent);
                maxLeading = SkTMax(maxLeading, metrics.fLeading);
                previousRunIndex = runIndex;
            }

            // Nothing can be written until the baseline is known.
            if (!(nextGlyph == nullptr || nextGlyph->fMustLineBreakBefore))
                continue;

            currentPoint.fY -= maxAscent;

            int numRuns = runIndex - previousBreak.fRunIndex + 1;
            SkAutoSTMalloc<4, UBiDiLevel> runLevels(numRuns);
            for (int i = 0; i < numRuns; ++i) { runLevels[i] = runs[previousBreak.fRunIndex + i].fLevel; }
            SkAutoSTMalloc<4, int32_t> logicalFromVisual(numRuns);
            ubidi_reorderVisual(runLevels, numRuns, logicalFromVisual);

            rect.fLeft = SkTMin(rect.fLeft, currentPoint.fX);
            for (int i = 0; i < numRuns; ++i)
            {
                int logicalIndex = previousBreak.fRunIndex + logicalFromVisual[i];

                int startGlyphIndex = (logicalIndex == previousBreak.fRunIndex)
                                          ? previousBreak.fGlyphIndex
                                          : 0;
                int endGlyphIndex = (logicalIndex == runIndex)
                                        ? glyphIndex + 1
                                        : runs[logicalIndex].fNumGlyphs;
                append(builder, runs[logicalIndex], startGlyphIndex, endGlyphIndex, currentPoint);
            }
            rect.fRight = SkTMax(rect.fRight, currentPoint.fX);

            currentPoint.fY += maxDescent + maxLeading;
            currentPoint.fX = point.fX;
            maxAscent = 0;
            maxDescent = 0;
            maxLeading = 0;
            previousRunIndex = -1;
            previousBreak = glyphIterator;
        }
    }
    rect.fBottom = currentPoint.fY;
    return currentPoint;
}
