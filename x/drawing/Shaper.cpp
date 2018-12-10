#include "stdafx.h"
#include "Shaper.h"
#include "drawing/skia/skia.h"

#include <hb.h>
#include <hb-ot.h>
#include <hb-icu.h>
#include <unicode/brkiter.h>
#include <unicode/locid.h>
#include <unicode/ubidi.h>
#include <unicode/uchriter.h>

#include "SkFontMgr.h"
#include "SkStream.h"
#include "SkTextBlob.h"
#include "SkTypeface.h"

static inline size_t utf32_to_utf8(const char32_t & ch, char * text)
{
    if (ch <= 0x7F)
    {
        text[0] = (char)ch;
        return 1;
    }
    if (ch <= 0x7FF)
    {
        text[0] = 0xC0 | (ch >> 6);            /* 110xxxxx */
        text[1] = 0x80 | (ch & 0x3F);          /* 10xxxxxx */
        return 2;
    }
    if (ch <= 0xFFFF)
    {
        text[0] = 0xE0 | (ch >> 12);           /* 1110xxxx */
        text[1] = 0x80 | ((ch >> 6) & 0x3F);   /* 10xxxxxx */
        text[2] = 0x80 | (ch & 0x3F);          /* 10xxxxxx */
        return 3;
    }
    if (ch <= 0x10FFFF)
    {
        text[0] = 0xF0 | (ch >> 18);           /* 11110xxx */
        text[1] = 0x80 | ((ch >> 12) & 0x3F);  /* 10xxxxxx */
        text[2] = 0x80 | ((ch >> 6) & 0x3F);   /* 10xxxxxx */
        text[3] = 0x80 | (ch & 0x3F);          /* 10xxxxxx */
        return 4;
    }
    return 0;
}

static inline size_t utf8_to_utf32(const char * text, size_t size, char32_t & ch)
{
    if (!size)
    {
        ch = 0;
        return 0;
    }

    const uint8_t * start = reinterpret_cast<const uint8_t *>(text);
    const uint8_t * curr = reinterpret_cast<const uint8_t *>(text);
    uint32_t crtl = *start;
    ch = *curr++;

    uint32_t mask = ~0x7F;
    if (crtl & 0x80)
    {
        mask = ~0x3F;
        crtl <<= 1;
        do
        {
            if (curr - start > size)
            {
                ch = 0;
                return 0;
            }

            ch <<= 6;
            ch |= ((*curr++) & 0x3F);
            crtl <<= 1;

            mask <<= 5;
        } while (crtl & 0x80);
    }
    ch &= ~mask;
    return curr - start;
}


namespace drawing
{
    static inline bidirection level_to_bidi(UBiDiLevel level)
    {
        return (level & 1) ? bidirection::rtl : bidirection::ltr;
    }

    struct glyph_iterator
    {
        std::vector<glyph>  & glyphs;
        span32 range;
        bool rtl = false;
        size_t curr = 0;

        size_t index() const
        {
            return curr;
        }

        void next()
        {
            ++curr;
        }

        bool ok()
        {
            return curr < range.end();
        }
    };

    std::shared_ptr<hb_font_t> create_hb_font(SkTypeface * tf)
    {
        int index = 0;
        std::unique_ptr<SkStreamAsset> asset(tf->openStream(&index));
        size_t size = asset->getLength();
        core::block_ptr<hb_blob_t, hb_blob_destroy> blob;
        if (const void * base = asset->getMemoryBase())
        {
            blob.reset(hb_blob_create((char*)base, SkToUInt(size), HB_MEMORY_MODE_READONLY, asset.release(), [](void * p) { delete (SkStreamAsset*)p; }));
        }
        else
        {
            // SkDebugf("Extra SkStreamAsset copy\n");
            void * ptr = size ? sk_malloc_throw(size) : nullptr;
            asset->read(ptr, size);
            blob.reset(hb_blob_create((char*)ptr, SkToUInt(size), HB_MEMORY_MODE_READONLY, ptr, sk_free));
        }
        assert(blob);
        hb_blob_make_immutable(blob.get());

        core::block_ptr<hb_face_t, hb_face_destroy> face(hb_face_create(blob.get(), (unsigned)index));
        assert(face);
        if (!face)
            return nullptr;
        hb_face_set_index(face.get(), (unsigned)index);
        hb_face_set_upem(face.get(), tf->getUnitsPerEm());

        std::shared_ptr<hb_font_t> font(hb_font_create(face.get()), hb_font_destroy);
        assert(font);
        if (!font)
            return nullptr;
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

    static thread_local std::unique_ptr<Shaper> shaper;
    Shaper::Shaper()
    {
        UErrorCode status = U_ZERO_ERROR;
        _breaker_world = { icu::BreakIterator::createLineInstance(icu::Locale::getDefault(), status), [](icu::BreakIterator * ptr) { delete ptr; } };
        _breaker_character = { icu::BreakIterator::createCharacterInstance(icu::Locale::getDefault(), status), [](icu::BreakIterator * ptr) { delete ptr; } };
    }

    Shaper & Shaper::instance()
    {
        if (!shaper)
            shaper.reset(new Shaper());
        return *shaper;
    }

    uint16_t Shaper::indexFont(const drawing::font & font)
    {
        uint16_t index = 0;
        auto iter = _font_indices.find(font);
        if (iter == _font_indices.end())
        {
            auto sktypeface = std::shared_ptr<SkTypeface>(SkTypeface::MakeFromName(font.family.c_str(), drawing::skia::from(font.style)).release(), drawing::skia::skia_unref<>);
            auto hbfont = create_hb_font(sktypeface.get());
            if(!sktypeface || !hbfont)
                throw core::error_not_supported;

            _fonts.push_back({ font, sktypeface, hbfont, drawing::fontmetrics(font) });
            if (_fonts.size() > 0xfffe)
                throw core::error_outofbound;
            index = uint16_t(_fonts.size() - 1);
            _font_indices[font] = index;
        }
        else
            index = iter->second;
        return index;
    }


    void TextClusterizer::setFont(span32 range, const drawing::font & font)
    {
        uint16_t index = _shaper.indexFont(font);
        std::fill(_rtf_font_indices.begin() + range.index, _rtf_font_indices.begin() + range.end(), index);
    }

    void TextClusterizer::setColor(span32 range, uint32_t color)
    {
        std::fill(_rtf_colors.begin() + range.index, _rtf_colors.begin() + range.end(), color);
    }

    core::error TextClusterizer::itermize(std::string text, const drawing::font & font_default, core::color color_default)
    {

        //SkShaper shaper(SkTypeface::MakeFromName(_font_default.family.c_str(), skia::from(_font_default.style)));

        //SkPaint paint;
        //SkTextBlobBuilder builder;
        //SkRect rcBlob;
        //SkPoint end = shaper.shape(builder, paint, text.c_str(), text.length(), true, {}, 999999999, rcBlob);

        _items.clear();

        _glyphs.clear();
        _clusters.clear();

        _text = text;

        _font_default = _shaper.indexFont(font_default);
        _rtf_font_indices.assign(_text.length(), _font_default);
        _rtf_colors.assign(_text.length(), color_default);

        //_font_indices.clear();
        //_fonts.clear();
#ifdef _DEBUG
        _u32text.clear();
#endif
        std::unique_ptr<hb_buffer_t, void(*)(hb_buffer_t *)> _hbbuffer(hb_buffer_create(), hb_buffer_destroy);
        UErrorCode status = U_ZERO_ERROR;
        auto & breaker_world = _shaper.breaker_world();

        if (_text.empty())
            return core::error_ok;

        std::u16string u16str = core::u8str_u16str(_text);

        std::unique_ptr<UBiDi, decltype(&ubidi_close)> ubidi(ubidi_openSized((int32_t)u16str.length(), 0, &status), &ubidi_close);
        if (U_FAILURE(status))
        {
            core::logger::war() << __FUNCTIONW__ L" ubidi_openSized failed, " << status << ": " << u_errorName(status);
            return core::error_inner;
        }
        assert(ubidi);
        if (!ubidi)
            return core::error_outofmemory;

        // The required lifetime of utf16 isn't well documented.
        // It appears it isn't used after ubidi_setPara except through ubidi_getText.
        ubidi_setPara(ubidi.get(), u16str.c_str(), (int32_t)u16str.length(), _defaultBidiLevel, nullptr, &status);
        if (U_FAILURE(status))
        {
            //SkDebugf("Bidi error: %s", u_errorName(status));
        }

        struct bidilevel
        {
            span32 range;
            UBiDiLevel level;
        };
        std::vector<bidilevel> levels;

        hb_buffer_clear_contents(_hbbuffer.get());
        hb_buffer_set_content_type(_hbbuffer.get(), HB_BUFFER_CONTENT_TYPE_UNICODE);
        //hb_buffer_set_cluster_level(_hbbuffer.get(), HB_BUFFER_CLUSTER_LEVEL_MONOTONE_CHARACTERS);
        hb_buffer_set_cluster_level(_hbbuffer.get(), HB_BUFFER_CLUSTER_LEVEL_MONOTONE_GRAPHEMES);

        hb_unicode_funcs_t * hb_unicode = hb_buffer_get_unicode_funcs(_hbbuffer.get());
        //hb_unicode_funcs_t * hb_unicode = hb_icu_get_unicode_funcs();
        //std::unique_ptr<hb_unicode_funcs_t, void(*)(hb_unicode_funcs_t *)> hb_unicode(hb_unicode_funcs_create(), hb_unicode_funcs_destroy);

        breaker_world.setText(u16str.c_str());

        size_t utf8_pos = 0;
        size_t utf16_pos = 0;
        size_t utf32_pos = 0;

        uint16_t font_index_fb_last = core::nposu16;

        enum class flushflag
        {
            eof = 0x0001,
            level = 0x0002,
            script = 0x0004,
            font = 0x0008,
            color = 0x0010,
            first = 0x0020,
        };
        typedef core::bitflag<flushflag> flushflags;

        std::vector<span32> utf16_ranges;
        std::vector<span32> nutf32_spans;
        while (utf16_pos < u16str.length())
        {
            flushflags flush = nullptr;

            char32_t cp_utf8 = 0;
            char32_t cp_utf16 = 0;
            size_t nutf8 = core::utf8_to_utf32(_text.c_str() + utf8_pos, _text.length() - utf8_pos, cp_utf8);
            size_t nutf16 = core::utf16_to_utf32(u16str.c_str() + utf16_pos, u16str.length() - utf16_pos, cp_utf16);
            if (cp_utf8 != cp_utf16)
            {
                assert(false);
                break;
            }

#ifdef _DEBUG
            _u32text.append(1, cp_utf8);
#endif
            size_t nutf32 = 1;

            for (size_t iutf16 = 0; iutf16 < nutf16; ++iutf16)
                utf16_ranges.push_back({ (uint32_t)utf8_pos, (uint32_t)nutf8 });
            nutf32_spans.push_back({ (uint32_t)utf8_pos, (uint32_t)nutf8 });

            UBiDiLevel level = ubidi_getLevelAt(ubidi.get(), (int32_t)utf16_pos);
            hb_script script = (hb_script)hb_unicode_script(hb_unicode, cp_utf8);
            uint16_t font_index = _rtf_font_indices[utf8_pos];
            uint32_t color = _rtf_colors[utf8_pos];

            // font 的 flush 需要考虑 fallback 的字体，测试一下
            auto & font_cache = _shaper.cache(font_index);
            if (font_index_fb_last != core::nposu16 &&
                (script == _items.back().script || script == hb_script::inherited || script == hb_script::common) &&
                _shaper.sktypeface(font_index_fb_last)->charsToGlyphs(&cp_utf8, SkTypeface::kUTF32_Encoding, nullptr, 1))
            {
                // 使用上一个 fallback 的字体搞定
                font_index = font_index_fb_last;
            }
            else if (font_cache.sktypeface->charsToGlyphs(&cp_utf8, SkTypeface::kUTF32_Encoding, nullptr, 1))
            {
                // 使用预期的字体搞定
                font_index_fb_last = core::nposu16;
            }
            else
            {
                // 慢慢找吧，fallbacking......
                font_index_fb_last = core::nposu16;
                sk_sp<SkFontMgr> fontMgr = SkFontMgr::RefDefault();
                auto tf = fontMgr->matchFamilyStyleCharacter(nullptr, drawing::skia::from(font_cache.font.style), nullptr, 0, cp_utf8);
                if (tf)
                {
                    drawing::font font_fb = drawing::skia::to(*tf, font_cache.font.size);
                    fontmetrics fmetrics_fb(font_fb);
                    fontmetrics fmetrics(font_cache.font);
                    if(fmetrics_fb.ascent - fmetrics.ascent > 0.49f || fmetrics_fb.descent - fmetrics.descent > 0.49f)
                    {
                        //float32_t rate = fmetrics.height / fmetrics_fb.height;
                        float32_t rate = std::min(fmetrics.ascent / fmetrics_fb.ascent, fmetrics.descent / fmetrics_fb.descent);
                        //if (rate < rate0)
                        //    rate = rate0;
                        //float32_t rate = fmetrics.height / fmetrics_fb.height;
                        font_fb = drawing::skia::to(*tf, font_cache.font.size * rate);
                    }
                    uint16_t font_index_fb = _shaper.indexFont(font_fb);
                    font_index_fb_last = font_index_fb;
                    font_index = font_index_fb;
                }
            }

            if (utf8_pos == 0)
            {
                assert(script != hb_script::inherited);
                flush |= flushflag::first;
            }
            else
            {
                if ((script != hb_script::inherited && script != hb_script::common) && (script != _items.back().script))
                    flush |= flushflag::script;

                if (level != _items.back().level)
                    flush |= flushflag::level;

                if (font_index != _items.back().font)
                    flush |= flushflag::font;

                if (color != _items.back().color)
                    flush |= flushflag::color;
            }

            if (flush.any())
            {
                item it;
                it.iindex = _items.size();
                it.trange.index = (uint32_t)utf8_pos;
                it.script = script;
                it.level = level;
                it.bidi = level_to_bidi(level);
                it.font = font_index;
                it.color = color;
                _items.push_back(it);
            }
            else
            {
                if (_items.back().script == hb_script::inherited || _items.back().script == hb_script::common)
                {
                    _items.back().font = font_index;
                    _items.back().script = script;
                }
            }

            item & item_last = _items.back();
            item_last.trange.length += (uint32_t)nutf8;

            utf8_pos += nutf8;
            utf16_pos += nutf16;
            utf32_pos += nutf32;
        }

#ifdef _DEBUG
        for (auto & item : _items)
        {
            item._text = _text.substr(item.trange.index, item.trange.length);
            item._font = _shaper.font(item.font);
        }
#endif

        for (auto & item : _items)
        {
            hb_buffer_clear_contents(_hbbuffer.get());
            hb_buffer_add_utf8(_hbbuffer.get(), _text.c_str(), (int32_t)_text.length(), item.trange.index, item.trange.length);
            // TODO: features
            auto & font_cache = _shaper.cache(item.font);
            auto & hbfont = _shaper.hbfont(item.font);

            hb_buffer_set_script(_hbbuffer.get(), hb_script_t(item.script));
            hb_buffer_set_direction(_hbbuffer.get(), (item.bidi == bidirection::rtl) ? HB_DIRECTION_RTL : HB_DIRECTION_LTR);
            // TODO: language
            hb_buffer_guess_segment_properties(_hbbuffer.get());

            hb_shape(hbfont.get(), _hbbuffer.get(), nullptr, 0);
            uint32_t gcount = hb_buffer_get_length(_hbbuffer.get());
            if (!gcount)
                continue;

#ifdef _DEBUG
            //const span32 & span_beg = nutf32_spans[_glyphs.size()];
            //const span32 & span_end = nutf32_spans[_glyphs.size() + gcount - 1];
            //if(item.trange.length != span_end.end() - span_beg.index)
            //{
            //assert(item.trange.length == span_end.end() - span_beg.index);
            //}
#endif

            if (item.bidi == bidirection::rtl)
            {
                // Put the clusters back in logical order.
                // Note that the advances remain ltr.
                hb_buffer_reverse(_hbbuffer.get());
            }

            int32_t scaleX = 0, scaleY = 0;
            hb_font_get_scale(hbfont.get(), &scaleX, &scaleY);
            float32_t coefX = font_cache.font.size *  1.0f / scaleX;
            float32_t coefY = font_cache.font.size * 1.0f / scaleY;

            size_t gindex_base = _glyphs.size();
            size_t cindex_base = _clusters.size();
            uint32_t ccount = 0;
            uint32_t cluster_last = core::nposu32;

            _glyphs.resize(_glyphs.size() + gcount);
            hb_glyph_info_t * infos = hb_buffer_get_glyph_infos(_hbbuffer.get(), nullptr);
            hb_glyph_position_t * poses = hb_buffer_get_glyph_positions(_hbbuffer.get(), nullptr);
            for (uint32_t gindex = 0; gindex < gcount; ++gindex)
            {
                const hb_glyph_info_t & info = infos[gindex];
                const hb_glyph_position_t & pos = poses[gindex];
                glyph & glyph = _glyphs[gindex_base + gindex];
                glyph.gindex = gindex_base + gindex;
                glyph.codepoint = info.codepoint;

                //if (gindex < gcount - 1)
                //    glyph.trange.length = infos[gindex + 1].cluster - info.cluster;
                //else
                //    glyph.trange.length = item.trange.end() - info.cluster;
                glyph.gid = uint16_t(info.codepoint);
                glyph.advance = { float32_t(pos.x_advance * coefX), float32_t(pos.y_advance * coefY) };
                glyph.offset = { float32_t(pos.x_offset * coefX), float32_t(pos.y_offset * coefY) };

                if(!ccount || cluster_last != info.cluster)
                {
                    cluster cl;
                    cl.cindex = (uint16_t)_clusters.size();
                    cl.iindex = item.iindex;
                    cl.grange.index = glyph.gindex;
                    cl.trange.index = info.cluster;
                    cl.bidi = item.bidi;
                    cl.wordbreak = false;

                    _clusters.push_back(cl);
                    ++ccount;
                    cluster_last = info.cluster;
                }

                glyph.standalone = !(info.mask & HB_GLYPH_FLAG_UNSAFE_TO_BREAK);
                glyph.cindex = uint16_t(_clusters.size() - 1);
                //_clusters.back().trange.length += glyph.trange.length;
                _clusters.back().grange.length += 1;
            }

            item.grange = { uint32_t(gindex_base), gcount };
            item.crange = { uint32_t(cindex_base), ccount };
        }

        // 计算 range2
        for (size_t cindex = 0; cindex < _clusters.size(); ++cindex)
        {
            cluster & cl = _clusters[cindex];
            if(cindex < _clusters.size() - 1)
                cl.trange.length = _clusters[cindex + 1].trange.index - cl.trange.index;
            else
                cl.trange.length = uint32_t(_text.length()) - cl.trange.index;
            item & it = _items[cl.iindex];
            glyph & header = _glyphs[cl.grange.index];
            cl.advance = header.advance;
            it.advance += header.advance;
            int32_t word_stop_utf16 = breaker_world.current();
            if (word_stop_utf16 != icu::BreakIterator::DONE && word_stop_utf16 < utf16_ranges.size())
            {
                if (utf16_ranges[word_stop_utf16].index < cl.trange.index)
                    word_stop_utf16 = breaker_world.next();

                if (word_stop_utf16 < utf16_ranges.size())
                {
                    int32_t word_stop_utf8 = utf16_ranges[word_stop_utf16].index;
                    cl.wordbreak = word_stop_utf8 == cl.trange.index;
                }
            }


#ifdef _DEBUG
            cl._text = _text.substr(cl.trange.index, cl.trange.length);
#endif
        }

        return core::error_ok;
    }

    core::error TextClusterizer::layout()
    {
        _segments.clear();
        _ascent = 0.0f;
        _descent = 0.0f;
        if (_text.empty())
            return core::error_ok;

        std::vector<UBiDiLevel> _bidis(_items.size());
        std::vector<int32_t> visual_indices(_items.size());
        for(size_t iindex = 0; iindex < _items.size(); ++iindex)
        {
            auto & it = _items[iindex];
            auto & fmetrics = _shaper.fontmetrics(it.font);

            segment seg = {};
            seg.sindex = _segments.size();
            seg.iindex = (uint32_t)iindex;
            seg.lindex = 0;
            seg.trange = it.trange;
            seg.crange = it.crange;
            seg.bidi = it.bidi;
            seg.advance = it.advance;
            _segments.push_back(seg);

            _ascent = std::max(_ascent, fmetrics.ascent);
            _descent = std::max(_descent, fmetrics.descent);
            _bidis[iindex] = it.level;
        }

#ifdef _DEBUG
        for (auto & seg : _segments)
            seg._text = _text.substr(seg.trange.index, seg.trange.length);
#endif

        ubidi_reorderLogical(_bidis.data(), (int32_t)_bidis.size(), visual_indices.data());
        std::sort(_segments.begin(), _segments.end(), [&visual_indices](const segment & lhs, const segment & rhs) { return visual_indices[lhs.sindex] < visual_indices[rhs.sindex]; });

        float32_t offset = 0;
        for (size_t sindex = 0; sindex < _segments.size(); ++sindex)
        {
            auto & seg = _segments[sindex];
            auto & item = _items[seg.iindex];
            seg.offset = offset;
            offset += seg.advance.cx;

            auto & fmetrics = _shaper.fontmetrics(_items[seg.iindex].font);
            bool ltr = !(item.level & 1);

            float32_t offset = 0;
            for (size_t cindex = seg.crange.index; cindex != seg.crange.end(); ++cindex)
            {
                auto & cluster = _clusters[cindex];
                cluster.sindex = (uint16_t)sindex;
                cluster.rect.x = ltr ? seg.offset + offset : seg.offset + seg.advance.cx - offset - cluster.advance.cx;
                cluster.rect.y = _ascent - fmetrics.ascent;
                cluster.rect.cx = cluster.advance.cx;
                cluster.rect.cy = fmetrics.height;
                offset += cluster.advance.cx;
                seg.grange += cluster.grange;
            }
        }

		_ascent = _shaper.fontmetrics(_font_default).ascent;
		_descent = _shaper.fontmetrics(_font_default).descent;
        return core::error_ok;
    }

    core::sizef TextClusterizer::bounds() const
    {
        float32_t ascent = 0.0f;
        float32_t descent = 0.0f;
        float32_t advance = 0.0f;
        for(auto & seg : _segments)
        {
            advance += seg.advance.cx;
            ascent = std::max(ascent, _shaper.fontmetrics(_items[seg.iindex].font).ascent);
            descent = std::max(descent, _shaper.fontmetrics(_items[seg.iindex].font).descent);
        }
        return { advance, ascent + descent };
    }

    std::shared_ptr<SkTextBlob> TextClusterizer::build(float32_t width)
    {
        if (_text.empty())
            return nullptr;

        if(width > 0 && !_elid.gid)
        {
            char dot[] = ".";
            uint16_t glyph = 0;
            if(_shaper.sktypeface(_font_default)->charsToGlyphs(dot, SkTypeface::kUTF8_Encoding, &glyph, 1))
            {
                //_elid.gid = glyph;
                std::unique_ptr<hb_buffer_t, void(*)(hb_buffer_t *)> _hbbuffer(hb_buffer_create(), hb_buffer_destroy);
                hb_buffer_clear_contents(_hbbuffer.get());
                hb_buffer_add_utf8(_hbbuffer.get(), dot, 1, 0, 1);
                // TODO: features
                auto & font_cache = _shaper.cache(_font_default);
                auto & hbfont = _shaper.hbfont(_font_default);

                hb_buffer_set_script(_hbbuffer.get(), HB_SCRIPT_LATIN);
                hb_buffer_set_direction(_hbbuffer.get(), HB_DIRECTION_LTR);
                // TODO: language
                hb_buffer_guess_segment_properties(_hbbuffer.get());

                hb_shape(hbfont.get(), _hbbuffer.get(), nullptr, 0);
                uint32_t gcount = hb_buffer_get_length(_hbbuffer.get());
                if (gcount)
                {
                    int32_t scaleX = 0, scaleY = 0;
                    hb_font_get_scale(hbfont.get(), &scaleX, &scaleY);
                    float32_t coefX = font_cache.font.size *  1.0f / scaleX;
                    float32_t coefY = font_cache.font.size * 1.0f / scaleY;
                    hb_glyph_info_t * infos = hb_buffer_get_glyph_infos(_hbbuffer.get(), nullptr);
                    hb_glyph_position_t * poses = hb_buffer_get_glyph_positions(_hbbuffer.get(), nullptr);
                    const hb_glyph_info_t & info = infos[0];
                    const hb_glyph_position_t & pos = poses[0];
                    _elid.gid = uint16_t(info.codepoint);
                    _elid.advance = { float32_t(pos.x_advance * coefX), float32_t(pos.y_advance * coefY) };
                    _elid.offset = { float32_t(pos.x_offset * coefX), float32_t(pos.y_offset * coefY) };
                }
            }
        }

        if (!_builder)
            _builder = std::make_shared<SkTextBlobBuilder>();

        float32_t offset_x = 0;
        float32_t offset_y = 0;
        for (size_t sindex = 0; sindex < _segments.size(); ++sindex)
        {
            auto & seg = _segments[sindex];
            item & item = _items[seg.iindex];
            bool ltr = !(item.level & 1);
            auto & font_cache = _shaper.cache(item.font);

            if(width > 0 && offset_x + seg.advance.cx + _elid.advance.cx * 3 >= width)
            {
                // 在这个 segment 越界，现在就 elid
                float32_t offset = offset_x;
                auto iter = std::find_if(_clusters.begin() + seg.crange.index, _clusters.begin() + seg.crange.end(), [&offset, &width, this](const cluster & cl)
                {
                    if (offset + cl.advance.cx + _elid.advance.cx * 3 > width)
                        return true;
                    offset += cl.advance.cx;
                    return false;
                });

                //auto iter = std::lower_bound(_clusters.begin() + seg.crange.index, _clusters.begin() + seg.crange.end(), width - _elid.advance.cx * 3, [](const cluster & cl, float32_t width) { return cl.rect.x + cl.rect.cx >= width; });
                size_t nclusters = iter == _clusters.end() ? seg.crange.length : iter->cindex - seg.crange.index;
                if(nclusters > 0)
                {
                    auto & cluster_tail = _clusters[seg.crange.index + nclusters - 1];
                    size_t nglyphs = cluster_tail.grange.end() - _clusters[seg.crange.index].grange.index;
                    SkFont font(sk_ref_sp(font_cache.sktypeface.get()), font_cache.font.size);
                    SkTextBlobBuilder::RunBuffer runBuffer = _builder->allocRunPos(font, nglyphs, nullptr);

                    for (size_t cindex = 0, iglyph = 0; cindex != nclusters; ++cindex)
                    {
                        cluster & cl = _clusters[ltr ? seg.crange.index + cindex : seg.crange.end() - 1 - cindex];
                        for (size_t gindex = cl.grange.index; gindex != cl.grange.end(); ++gindex, ++iglyph)
                        {
                            glyph & gl = _glyphs[gindex];
                            runBuffer.glyphs[iglyph] = gl.gid;
                            runBuffer.pos[iglyph * 2 + 0] = offset_x + gl.offset.x;
                            runBuffer.pos[iglyph * 2 + 1] = offset_y - gl.offset.y + _ascent; // 基线对齐
                            //runBuffer.pos[iglyph * 2 + 1] = offset_y - gl.offset.y + font_cache.fmetrics.ascent; // 顶对齐
                            offset_y += gl.advance.cy;
                            offset_x += gl.advance.cx;
                        }
                    }
                }
                else
                {
                }

                {
                    auto & font_cache_elid = _shaper.cache(_font_default);
                    SkFont font(sk_ref_sp(font_cache_elid.sktypeface.get()), font_cache_elid.font.size);
                    SkTextBlobBuilder::RunBuffer runBuffer = _builder->allocRunPos(font, 3, nullptr);
                    for (size_t ig = 0; ig < 3; ++ig)
                    {
                        runBuffer.glyphs[ig] = _elid.gid;
                        runBuffer.pos[ig * 2 + 0] = offset_x + _elid.offset.x;
                        runBuffer.pos[ig * 2 + 1] = offset_y - _elid.offset.y + _ascent; // 基线对齐
                        offset_y += _elid.advance.cy;
                        offset_x += _elid.advance.cx;
                    }
                }
                break;
            }
            else
            {
                SkFont font(sk_ref_sp(font_cache.sktypeface.get()), font_cache.font.size);
                SkTextBlobBuilder::RunBuffer runBuffer = _builder->allocRunPos(font, seg.grange.length, nullptr);
                for (size_t cindex = (ltr ? seg.crange.index : seg.crange.end()), iglyph = 0; cindex != (ltr ? seg.crange.end() : seg.crange.index); ltr ? ++cindex : --cindex)
                {
                    cluster & cl = _clusters[ltr ? cindex : cindex - 1];
                    for (size_t gindex = cl.grange.index; gindex != cl.grange.end(); ++gindex, ++iglyph)
                    {
                        glyph & gl = _glyphs[gindex];

                        runBuffer.glyphs[iglyph] = gl.gid;
                        //runBuffer.clusters[iglyph] = glyph.trange.index;
                        runBuffer.pos[iglyph * 2 + 0] = offset_x + gl.offset.x;
                        runBuffer.pos[iglyph * 2 + 1] = offset_y - gl.offset.y + _ascent; // 基线对齐
                        //runBuffer.pos[iglyph * 2 + 1] = offset_y - gl.offset.y + font_cache.fmetrics.ascent; // 顶对齐
                        offset_y += gl.advance.cy;
                        offset_x += gl.advance.cx;
                    }
                }
            }
        }
        return { _builder->make().release(), skia::skia_unref<SkTextBlob> };
    }

    const cluster & TextClusterizer::findCluster(size_t tindex) const
    {
        auto iter = std::upper_bound(_clusters.begin(), _clusters.end(), tindex, [](size_t tindex, const cluster & cl) { return  tindex < cl.trange.end(); });
        if (iter == _clusters.end())
            return Shaper::empty_cluster;
        return *iter;
    }

    const cluster & TextClusterizer::findCluster(float32_t pos) const
    {
        auto iter_seg = std::upper_bound(_segments.begin(), _segments.end(), pos, [](float32_t pos, const segment & seg) { return  pos < seg.offset + seg.advance.cx; });
        if (iter_seg == _segments.end())
            return Shaper::empty_cluster;

        if(iter_seg->bidi == bidirection::rtl)
        {
            auto iter = std::upper_bound(_clusters.begin() + iter_seg->crange.index, _clusters.begin() + iter_seg->crange.end(), pos, [](float32_t pos, const cluster & cl) { return  pos >= cl.rect.x; });
            if (iter == _clusters.begin() + iter_seg->crange.end())
                return Shaper::empty_cluster;
            return *iter;
        }
        else
        {
            auto iter = std::upper_bound(_clusters.begin() + iter_seg->crange.index, _clusters.begin() + iter_seg->crange.end(), pos, [](float32_t pos, const cluster & cl) { return  pos < cl.rect.right(); });
            if (iter == _clusters.begin() + iter_seg->crange.end())
                return Shaper::empty_cluster;
            return *iter;
        }
    }

    std::tuple<size_t, core::rectf> TextClusterizer::textRect(size_t toffset, size_t tlength)
    {
        if(!tlength)
            return { core::npos, {} };

        auto & cluster_header = findCluster(toffset);
        if (!cluster_header)
            return { core::npos, {} };

        auto & cluster_tailer = findCluster(toffset + tlength - 1);
        if (!cluster_tailer)
            return { core::npos, {} };

        auto & seg_first = _segments[cluster_header.sindex];
        auto & seg_last = _segments[cluster_tailer.sindex];

        if(seg_first.bidi == bidirection::ltr)
        {
            if (seg_first.sindex == seg_last.sindex)
                return { core::npos, { cluster_header.rect.x, 0, cluster_tailer.rect.right() - cluster_header.rect.x, _ascent + _descent } };
            else
            {
                auto & cluster_last = _clusters[seg_first.crange.end() - 1];
                return { seg_first.trange.end(), { cluster_header.rect.x, 0, cluster_last.rect.right() - cluster_header.rect.x, _ascent + _descent } };
            }
        }
        else
        {
            if (seg_first.sindex == seg_last.sindex)
                return { core::npos, { cluster_tailer.rect.x, 0, cluster_header.rect.right() - cluster_tailer.rect.x, _ascent + _descent } };
            else
            {
                auto & cluster_last = _clusters[seg_first.crange.end() - 1];
                return { seg_first.trange.end(), { cluster_last.rect.x, 0, cluster_header.rect.right() - cluster_last.rect.x, _ascent + _descent } };
            }
        }
    }

    core::error TextWraper::layout(float32_t end, wrap_mode mode)
    {
        _lines.clear();
        _segments.clear();

        if (_text.empty())
            return core::error_ok;

        float32_t curr = 0;
        uint32_t line = 0;

        enum class flushflag
        {
            eof = 0x0001,
            width = 0x0002,
            item = 0x0004,
        };
        typedef core::bitflag<flushflag> flushflags;

        for (size_t cindex = 0, iindex = 0; cindex < _clusters.size(); ++cindex)
        {
            flushflags flush = nullptr;
            cluster & cl = _clusters[cindex];
            if (cl.trange.index >= _items[iindex].trange.end())
            {
                ++iindex;
                flush |= flushflag::item;
            }

            if (curr + cl.advance.cx > end && _segments.back().crange.length > 0)
                flush |= flushflag::width;
            else {}

            if (flush.any() || !cindex)
            {
                segment seg = {};
                seg.sindex = _segments.size();
                seg.iindex = (uint32_t)iindex;
                seg.lindex = (uint32_t)line;
                seg.trange.index = cl.trange.index;
                seg.crange.index = (uint32_t)cindex;
                _segments.push_back(seg);

                if (flush.any(flushflag::width) || !cindex)
                {
                    row new_row = {};
                    new_row.trange.index = cl.trange.index;
                    new_row.crange.index = (uint32_t)cindex;
                    new_row.srange = { (uint32_t)(_segments.size() - 1), 0 };
                    new_row.line = line;
                    _lines.push_back(new_row);
                    curr = 0;
                    ++line;
                }

                ++_lines.back().srange.length;
            }
            else
            {
            }
            curr += cl.advance.cx;

            segment & seg_last = _segments.back();
            seg_last.trange.length += cl.trange.length;
            seg_last.crange.length += 1;
            seg_last.advance += cl.advance;

            row & row_this = _lines.back();
            auto & fmetrics = _shaper.fontmetrics(_items[iindex].font);
            row_this.trange.length += cl.trange.length;
            row_this.crange.length += 1;
            row_this.width += cl.advance.cx;
            row_this.ascent = std::max(row_this.ascent, fmetrics.ascent);
            row_this.descent = std::max(row_this.descent, fmetrics.descent);
        }

#ifdef _DEBUG
        for (auto & seg : _segments)
            seg._text = _text.substr(seg.trange.index, seg.trange.length);
        for (auto & row : _lines)
            row._text = _text.substr(row.trange.index, row.trange.length);
#endif

        std::vector<UBiDiLevel> _bidis;
        std::vector<int32_t> visual_indices;
        for (auto & row : _lines)
        {
            _bidis.resize(row.srange.length);
            visual_indices.resize(row.srange.length, 0);

            for (size_t sindex = 0; sindex < row.srange.length; ++sindex)
                _bidis[sindex] = _items[_segments[sindex + row.srange.index].iindex].level;

            ubidi_reorderLogical(_bidis.data(), (int32_t)_bidis.size(), visual_indices.data());

            std::sort(_segments.begin() + row.srange.index, _segments.begin() + row.srange.end(), [&row, &visual_indices](const segment & lhs, const segment & rhs) { return visual_indices[lhs.sindex - row.srange.index] < visual_indices[rhs.sindex - row.srange.index]; });

            float32_t offset = 0;
            for (size_t sindex = 0; sindex < row.srange.length; ++sindex)
            {
                auto & seg = _segments[sindex + row.srange.index];
                auto & item = _items[seg.iindex];
                seg.offset = offset;
                offset += seg.advance.cx;

                auto & fmetrics = _shaper.fontmetrics(_items[seg.iindex].font);
                float32_t pos = seg.offset;

                bool ltr = !(item.level & 1);
                for (size_t cindex = (ltr ? seg.crange.index : seg.crange.end());
                    cindex != (ltr ? seg.crange.end() : seg.crange.index);
                    ltr ? ++cindex : --cindex)
                {
                    auto & cluster = _clusters[ltr ? cindex : cindex - 1];
                    cluster.sindex = (uint16_t)sindex;
                    cluster.rect.x = pos;
                    cluster.rect.y = row.ascent - fmetrics.ascent;
                    cluster.rect.cx = cluster.advance.cx;
                    cluster.rect.cy = fmetrics.height;
                    pos += cluster.advance.cx;
                    seg.grange += cluster.grange;
                }
            }
        }

        return core::error_ok;
    }

    core::sizef TextWraper::bounds() const
    {
        if (_text.empty())
            return {};

        core::sizef size;
        for (size_t lindex = 0; lindex < _lines.size(); ++lindex)
        {
            auto & ln = _lines[lindex];
            float32_t width = 0;
            for (size_t sindex = ln.srange.index; sindex < ln.srange.end(); ++sindex)
            {
                auto & seg = _segments[sindex];
                width += seg.advance.cx;
            }

            size.cx = std::max(size.cx, width);
            size.cy += ln.ascent + ln.descent;
        }

        size.cy += _shaper.fontmetrics(_font_default).leading * (_lines.size() - 1);
        return size;
    }

    std::shared_ptr<SkTextBlob> TextWraper::build()
    {
        if (_text.empty())
            return nullptr;

        if (!_builder)
            _builder = std::make_shared<SkTextBlobBuilder>();

        float32_t offset_y = 0;
        for(size_t lindex = 0; lindex < _lines.size(); ++lindex)
        {
            auto & ln = _lines[lindex];

            for (size_t sindex = 0; sindex < ln.srange.length; ++sindex)
            {
                segment & seg = _segments[ln.srange.index + sindex];
                item & item = _items[seg.iindex];
                bool ltr = !(item.level & 1);

                auto & font_cache = _shaper.cache(item.font);
                float32_t offset_x = seg.offset;
                //SkPaint paint;
                //paint.setTypeface(sk_ref_sp(font_cache.sktypeface.get()));
                //paint.setTextSize(font_cache.font.size);
                //paint.setAntiAlias(true);
                //paint.setLCDRenderText(true);
                //paint.setColor(item.color);
                //paint.setAutohinted(true);
                //paint.setTextEncoding(SkTextEncoding::kGlyphID);

                SkFont font(sk_ref_sp(font_cache.sktypeface.get()), font_cache.font.size);
                SkTextBlobBuilder::RunBuffer runBuffer = _builder->allocRunPos(font, seg.grange.length, nullptr);
                //memcpy(runBuffer.utf8text, _text.c_str() + seg.trange.index, seg.trange.length);

                for (size_t cindex = (ltr ? seg.crange.index : seg.crange.end()), iglyph = 0; cindex != (ltr ? seg.crange.end() : seg.crange.index); ltr ? ++cindex : --cindex)
                {
                    cluster & cl = _clusters[ltr ? cindex : cindex - 1];
                    for (size_t gindex = cl.grange.index; gindex != cl.grange.end(); ++gindex, ++iglyph)
                    {
                        glyph & gl = _glyphs[gindex];

                        runBuffer.glyphs[iglyph] = gl.gid;
                        //runBuffer.clusters[iglyph] = glyph.trange.index;
                        runBuffer.pos[iglyph * 2 + 0] = offset_x + gl.offset.x;
                        runBuffer.pos[iglyph * 2 + 1] = offset_y - gl.offset.y + ln.ascent;
                        offset_x += gl.advance.cx;
                    }
                }
            }

            offset_y += ln.ascent + ln.descent;
            offset_y += _shaper.fontmetrics(_font_default).leading;
        }

        return { _builder->make().release(), skia::skia_unref<SkTextBlob> };
    }

    const cluster & TextWraper::findCluster(float32_t pos, size_t lindex) const
    {
        auto & row = _lines[lindex];

        auto iter_seg = std::upper_bound(_segments.begin() + row.srange.index, _segments.begin() + row.srange.end(), pos, [](float32_t pos, const segment & seg) { return  pos < seg.offset + seg.advance.cx; });
        if (iter_seg == _segments.begin() + row.srange.end())
            return Shaper::empty_cluster;


        auto iter = std::upper_bound(_clusters.begin() + iter_seg->crange.index, _clusters.begin() + iter_seg->crange.end(), pos, [](float32_t pos, const cluster & cl) { return  pos < cl.rect.right(); });
        if (iter == _clusters.begin() + iter_seg->crange.end())
            return Shaper::empty_cluster;
        return *iter;
    }
}
