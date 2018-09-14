#include "stdafx.h"
#include "Shaper.h"
#include "drawing/skia/skia.h"

#include <hb.h>
#include <hb-ot.h>
#include <unicode/brkiter.h>
#include <unicode/locid.h>
#include <unicode/ubidi.h>
#include <unicode/uchriter.h>

#include "SkFontMgr.h"
#include "SkStream.h"
#include "SkTextBlob.h"
#include "SkTypeface.h"
#include "skia/SkShaper.h"

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
    struct glyph_iterator
    {
        std::vector<glyph>  & glyphs;
        section32 range;
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

    core::error Shaper::itermize(std::string text, const drawing::font & font, core::color32 color)
    {
        //SkShaper shaper(SkTypeface::MakeFromName(_font_default.family.c_str(), skia::from(_font_default.style)));

        //SkPaint paint;
        //SkTextBlobBuilder builder;
        //SkRect rcBlob;
        //SkPoint end = shaper.shape(builder, paint, text.c_str(), text.length(), true, {}, 999999999, rcBlob);

        _items.clear();

        _glyphs.clear();
        _clusters.clear();
        _segments.clear();
        _rows.clear();

        _text = text;

        _rtf_font_indices.assign(_text.length(), fontIndex(font));
        _rtf_colors.assign(_text.length(), color);

        //_font_indices.clear();
        //_fonts.clear();
#ifdef _DEBUG
        _u32text.clear();
#endif
        if(!_hbbuffer)
            _hbbuffer = { hb_buffer_create(), hb_buffer_destroy };
        UErrorCode status = U_ZERO_ERROR;
        if (!_breaker_world)
            _breaker_world = { icu::BreakIterator::createLineInstance(icu::Locale::getDefault(), status), [](icu::BreakIterator * ptr) { delete ptr; } };
        if (!_breaker_character)
            _breaker_character = { icu::BreakIterator::createCharacterInstance(icu::Locale::getDefault(), status), [](icu::BreakIterator * ptr) { delete ptr; } };

        if(_text.empty())
            return core::error_ok;

        std::u16string u16str = core::u8str_u16str(_text);

        std::unique_ptr<UBiDi, decltype(&ubidi_close)> bidi(ubidi_openSized((int32_t)u16str.length(), 0, &status), &ubidi_close);
        if (U_FAILURE(status))
        {
            core::logger::war() << __FUNCTIONW__ L" ubidi_openSized failed, " << status << ": " << u_errorName(status);
            return core::error_inner;
        }
        assert(bidi);
        if (!bidi)
            return core::error_outofmemory;

        // The required lifetime of utf16 isn't well documented.
        // It appears it isn't used after ubidi_setPara except through ubidi_getText.
        ubidi_setPara(bidi.get(), u16str.c_str(), (int32_t)u16str.length(), _defaultBidiLevel, nullptr, &status);
        if (U_FAILURE(status))
        {
            //SkDebugf("Bidi error: %s", u_errorName(status));
        }

        struct bidilevel
        {
            section32 range;
            UBiDiLevel level;
        };
        std::vector<bidilevel> levels;

        hb_buffer_clear_contents(_hbbuffer.get());
        hb_buffer_set_content_type(_hbbuffer.get(), HB_BUFFER_CONTENT_TYPE_UNICODE);
        hb_buffer_set_cluster_level(_hbbuffer.get(), HB_BUFFER_CLUSTER_LEVEL_MONOTONE_CHARACTERS);

        hb_unicode_funcs_t * hb_unicode = hb_buffer_get_unicode_funcs(_hbbuffer.get());

        _breaker_world->setText(u16str.c_str());
        _breaker_character->setText(u16str.c_str());

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
        };
        typedef core::bitflag<flushflag> flushflags;

        std::vector<section32> utf16_ranges;
        _items.push_back({});
        while(utf16_pos < u16str.length())
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

            for(size_t iutf16 = 0; iutf16 < nutf16; ++iutf16)
                utf16_ranges.push_back({ (uint32_t)utf8_pos, (uint32_t)nutf8 });

            UBiDiLevel level = ubidi_getLevelAt(bidi.get(), (int32_t)utf16_pos);
            hb_script script = (hb_script)hb_unicode_script(hb_unicode, cp_utf8);
            uint16_t font_index = _rtf_font_indices[utf8_pos];
            uint32_t color = _rtf_colors[utf8_pos];

            // font 的 flush 需要考虑 fallback 的字体，测试一下
            auto & font_cache = _fonts[font_index];
            if (font_index_fb_last != core::nposu16 && 
                (script == _items.back().script || script == hb_script::inherited || script == hb_script::common) &&
                _fonts[font_index_fb_last].skfont->charsToGlyphs(&cp_utf8, SkTypeface::kUTF32_Encoding, nullptr, 1))
            {
                // 使用上一个 fallback 的字体搞定
                font_index = font_index_fb_last;
            }
            else if (font_cache.skfont->charsToGlyphs(&cp_utf8, SkTypeface::kUTF32_Encoding, nullptr, 1))
            {
                // 使用预期的字体搞定
                font_index_fb_last = 0;
            }
            else
            {
                // 慢慢找吧，fallbacking......
                font_index_fb_last = 0;
                sk_sp<SkFontMgr> fontMgr = SkFontMgr::RefDefault();
                auto tf = fontMgr->matchFamilyStyleCharacter(nullptr, drawing::skia::from(font_cache.font.style), nullptr, 0, cp_utf8);
                if (tf)
                {
                    drawing::font font_fb = drawing::skia::to(*tf, font_cache.font.size);
                    uint16_t font_index_fb = fontIndex(font_fb);
                    font_index_fb_last = font_index_fb;
                    font_index = font_index_fb;
                }
            }

            if (utf8_pos == 0)
            {
                assert(script != hb_script::inherited);
                item & first = _items.back();
                first.script = script;
                first.level = level;
                first.font = font_index;
                first.color = color;
            }
            else
            {

            }

            if (_items.back().script == hb_script::inherited || _items.back().script == hb_script::common)
            {
                // 之前部分不要的
                _items.back().font = font_index;
                _items.back().script = script;
            }

            if ((script != hb_script::inherited && script != hb_script::common) && (script != _items.back().script))
                flush |= flushflag::script;

            if (level != _items.back().level)
                flush |= flushflag::level;

            if (font_index != _items.back().font)
                flush |= flushflag::font;

            if (color != _items.back().color)
                flush |= flushflag::color;

            if (flush.any())
            {
                assert(_items.back().trange.length > 0);
                _items.push_back({});
                item & item_new = _items.back();
                item_new.trange.index = (uint32_t)utf8_pos;
                item_new.script = script;
                item_new.level = level;
                item_new.font = font_index;
                item_new.color = color;
            }
            else
            {
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
            item._font = _fonts[item.font].font;
        }
#endif

        for (auto & item : _items)
        {
            hb_buffer_clear_contents(_hbbuffer.get());
            hb_buffer_add_utf8(_hbbuffer.get(), _text.c_str(), (int32_t)_text.length(), item.trange.index, item.trange.length);
            // TODO: features
            auto & font_cache = _fonts[item.font];
            const drawing::font & font = font_at(item.font);
            auto hbfont = hbfont_at(item.font);

            hb_buffer_set_script(_hbbuffer.get(), hb_script_t(item.script));
            hb_buffer_set_direction(_hbbuffer.get(), (item.level & 1) ? HB_DIRECTION_RTL : HB_DIRECTION_LTR);
            // TODO: language
            hb_buffer_guess_segment_properties(_hbbuffer.get());

            hb_shape(hbfont.get(), _hbbuffer.get(), nullptr, 0);
            uint32_t gcount = hb_buffer_get_length(_hbbuffer.get());
            if (!gcount)
                continue;

            if (item.level & 1)
            {
                // Put the clusters back in logical order.
                // Note that the advances remain ltr.
                hb_buffer_reverse(_hbbuffer.get());
            }

            int32_t scaleX = 0, scaleY = 0;
            hb_font_get_scale(hbfont.get(), &scaleX, &scaleY);
            float32_t coefX = font.size *  1.0f / scaleX;
            float32_t coefY = font.size * 1.0f / scaleY;

            size_t gindex_base = _glyphs.size();
            _glyphs.resize(_glyphs.size() + gcount);
            hb_glyph_info_t * infos = hb_buffer_get_glyph_infos(_hbbuffer.get(), nullptr);
            hb_glyph_position_t * poses = hb_buffer_get_glyph_positions(_hbbuffer.get(), nullptr);
            for (uint32_t gindex = 0; gindex < gcount; ++gindex)
            {
                const hb_glyph_info_t & info = infos[gindex];
                const hb_glyph_position_t & pos = poses[gindex];
                glyph & glyph = _glyphs[gindex_base + gindex];
                glyph.gindex = gindex_base + gindex;
                glyph.trange = { info.cluster, 1 };
                if (gindex < gcount - 1)
                    glyph.trange.length = infos[gindex + 1].cluster - info.cluster;
                else
                    glyph.trange.length = item.trange.end() - info.cluster;
                glyph.gid = uint16_t(info.codepoint);
                glyph.advance = { float32_t(pos.x_advance * coefX), float32_t(pos.y_advance * coefY) };
                glyph.offset = { float32_t(pos.x_offset * coefX), float32_t(pos.y_offset * coefY) };
                int32_t word_stop_utf16 = _breaker_world->current();
                if (word_stop_utf16 != icu::BreakIterator::DONE && word_stop_utf16 < utf16_ranges.size())
                {
                    if (utf16_ranges[word_stop_utf16].index < glyph.trange.index)
                        word_stop_utf16 = _breaker_world->next();

                    if (word_stop_utf16 < utf16_ranges.size())
                    {
                        int32_t word_stop_utf8 = utf16_ranges[word_stop_utf16].index;
                        glyph.wordbreak = word_stop_utf8 == glyph.trange.index;
                    }
                }

                int32_t char_stop_utf16 = _breaker_character->current();
                if (char_stop_utf16 != icu::BreakIterator::DONE && char_stop_utf16 < utf16_ranges.size())
                {
                    if (utf16_ranges[char_stop_utf16].index < glyph.trange.index)
                        char_stop_utf16 = _breaker_character->next();

                    if (char_stop_utf16 < utf16_ranges.size())
                    {
                        int32_t char_stop_utf8 = utf16_ranges[char_stop_utf16].index;
                        if (char_stop_utf8 == glyph.trange.index)
                        {
                            cluster cl;
                            cl.cindex = (uint16_t)_clusters.size();
                            cl.grange.index = glyph.gindex;
                            cl.trange.index = glyph.trange.index;
                            cl.rtl = !!(item.level & 1);
                            _clusters.push_back(cl);
                        }
                    }
                }

                glyph.standalone = !(info.mask & HB_GLYPH_FLAG_UNSAFE_TO_BREAK);
#ifdef _DEBUG
                glyph._text = _text.substr(glyph.trange.index, glyph.trange.length);
#endif
                glyph.cindex = uint16_t(_clusters.size() - 1);
                _clusters.back().trange.length += glyph.trange.length;
                _clusters.back().grange.length += 1;
            }
        }

        // 计算 range2
        for (size_t cindex = 0; cindex < _clusters.size(); ++cindex)
        {
            cluster & cl = _clusters[cindex];
            glyph & header = _glyphs[cl.grange.index];
            cl.advance = header.advance;
#ifdef _DEBUG
            cl._text = _text.substr(cl.trange.index, cl.trange.length);
#endif
        }

        return core::error_ok;
    }

    core::error Shaper::wrap(float32_t end, wrap_mode mode)
    {
        _rows.clear();
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

        for(size_t cindex = 0, iindex = 0; cindex < _clusters.size(); ++cindex)
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

            if(flush.any() || !cindex)
            {
                segment seg = {};
                seg.sindex = _segments.size();
                seg.item = (uint32_t)iindex;
                seg.line = (uint32_t)line;
                seg.trange.index = cl.trange.index;
                seg.crange.index = (uint32_t)cindex;
                _segments.push_back(seg);

                if (flush.any(flushflag::width) || !cindex)
                {
                    row new_row = {};
                    new_row.trange.index = cl.trange.index;
                    new_row.crange.index = (uint32_t)cindex;
                    new_row.srange = { (uint32_t)(_segments.size() - 1), 0};
                    new_row.line = line;
                    _rows.push_back(new_row);
                    curr = 0;
                    ++line;
                }

                ++_rows.back().srange.length;
            }
            else
            {
            }
            curr += cl.advance.cx;

            segment & seg_last = _segments.back();
            seg_last.trange.length += cl.trange.length;
            seg_last.crange.length += 1;
            seg_last.width += cl.advance.cx;

            row & row_this = _rows.back();
            auto & fmetrics = _fonts[_items[iindex].font].fmetrics;
            row_this.trange.length += cl.trange.length;
            row_this.crange.length += 1;
            row_this.width += cl.advance.cx;
            row_this.ascent = std::max(row_this.ascent, fmetrics.ascent);
            row_this.descent = std::max(row_this.descent, fmetrics.descent);
        }

#ifdef _DEBUG
        for (auto & seg : _segments)
            seg._text = _text.substr(seg.trange.index, seg.trange.length);
        for(auto & row : _rows)
            row._text = _text.substr(row.trange.index, row.trange.length);
#endif

        std::vector<UBiDiLevel> _bidis;
        std::vector<int32_t> visual_indices;
        for (auto & row : _rows)
        {
            _bidis.resize(row.srange.length);
            visual_indices.resize(row.srange.length, 0);

            for (size_t sindex = 0; sindex < row.srange.length; ++sindex)
                _bidis[sindex] = _items[_segments[sindex + row.srange.index].item].level;

            ubidi_reorderLogical(_bidis.data(), (int32_t)_bidis.size(), visual_indices.data());

            std::sort(_segments.begin() + row.srange.index, _segments.begin() + row.srange.end(), [&row, &visual_indices](const segment & lhs, const segment & rhs) { return visual_indices[lhs.sindex - row.srange.index] < visual_indices[rhs.sindex - row.srange.index]; });

            float32_t offset = 0;
            for (size_t sindex = 0; sindex < row.srange.length; ++sindex)
            {
                auto & seg = _segments[sindex + row.srange.index];
                auto & item = _items[seg.item];
                seg.offset = offset;
                offset += seg.width;

                auto & fmetrics = _fonts[_items[seg.item].font].fmetrics;
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

    core::error Shaper::build(SkTextBlobBuilder & builder, uint32_t index)
    {
        if (_text.empty())
            return core::error_ok;

        row & row = _rows[index];

        float32_t offset_y = 0;
        for (size_t sindex = 0; sindex < row.srange.length; ++sindex)
        {
            segment & seg = _segments[row.srange.index + sindex];
            item & item = _items[seg.item];
            bool ltr = !(item.level & 1);

            auto & font_cache = _fonts[item.font];
            float32_t offset_x = seg.offset;
            SkPaint paint;
            paint.setTypeface(sk_ref_sp(font_cache.skfont.get()));
            paint.setTextSize(font_cache.font.size);
            paint.setAntiAlias(true);
            paint.setLCDRenderText(true);
            paint.setColor(item.color);
            paint.setAutohinted(true);
            paint.setTextEncoding(SkPaint::kGlyphID_TextEncoding);

            //SkTextBlobBuilder::RunBuffer runBuffer = builder.allocRunTextPos(paint, seg.grange.length, seg.trange.length, SkString(), nullptr);
            SkTextBlobBuilder::RunBuffer runBuffer = builder.allocRunPos(paint, seg.grange.length, nullptr);
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
                    runBuffer.pos[iglyph * 2 + 1] = offset_y - gl.offset.y + row.ascent;
                    offset_y += gl.advance.cy;
                    offset_x += gl.advance.cx;
                }
            }
        }
        return core::error_ok;
    }

    std::shared_ptr<SkTextBlob> Shaper::build(uint32_t index)
    {
        if (!_builder)
            _builder = std::make_shared<SkTextBlobBuilder>();
        auto err = build(*_builder, index);
        if (err)
            return nullptr;
        return std::shared_ptr<SkTextBlob>(_builder->make().release(), skia::skia_unref<SkTextBlob>);
    }

    std::shared_ptr<SkTextBlob> Shaper::shape(std::string text, const drawing::font & font, core::color32 color, core::si32f & size)
    {
        if (!_builder)
            _builder = std::make_shared<SkTextBlobBuilder>();

        itermize(text, font, color);
        wrap(std::numeric_limits<float32_t>::max(), wrap_mode::word);
        size = lineSize(0);
        return build(0);
    }

    core::si32f Shaper::lineSize(uint32_t index)
    {
        if (_text.empty())
            return { 0, _fonts[0].fmetrics.height };

        row & row = _rows[index];
        core::si32f size = { 0, row.ascent + row.descent };

        for (size_t sindex = row.srange.index; sindex < row.srange.end(); ++sindex)
        {
            segment & seg = _segments[sindex];
            item & item = _items[seg.item];
            size.cx += seg.width;
        }
        return size;
    }

    void Shaper::setFont(section32 range, const drawing::font & font)
    {
        uint16_t index = fontIndex(font);
        std::fill(_rtf_font_indices.begin() + range.index, _rtf_font_indices.begin() + range.end(), index);
    }

    void Shaper::setColor(section32 range, uint32_t color)
    {
        std::fill(_rtf_colors.begin() + range.index, _rtf_colors.begin() + range.end(), color);
    }

    uint16_t Shaper::fontIndex(const drawing::font & font)
    {
        uint16_t index = 0;
        auto iter = _font_indices.find(font);
        if (iter == _font_indices.end())
        {
            auto skfont = std::shared_ptr<SkTypeface>(SkTypeface::MakeFromName(font.family.c_str(), drawing::skia::from(font.style)).release(), drawing::skia::skia_unref<>);
            auto hbfont = create_hb_font(skfont.get());
            if(!skfont || !hbfont)
                throw core::error_not_supported;

            _fonts.push_back({ font, skfont, hbfont, drawing::fontmetrics(font) });
            if (_fonts.size() > 0xfffe)
                throw core::error_outofbound;
            index = uint16_t(_fonts.size() - 1);
            _font_indices[font] = index;
        }
        else
            index = iter->second;
        return index;
    }

    const glyph & Shaper::findGlyph(size_t index) const
    {
        static const glyph empty;
        if (_glyphs.empty())
            return empty;

        index &= cursor_pos_mask;
        if (index == cursor_pos_mask)
            return _glyphs.back();

        auto iter = std::lower_bound(_glyphs.begin(), _glyphs.end(), index, [](const glyph & g, size_t value) { return  g.trange.end() <= value; });
        if (iter == _glyphs.end() || iter->trange.end() <= index)
            return empty;
        return *iter;
    }

    const cluster & Shaper::findCluster(size_t tindex) const
    {
        auto iter = std::upper_bound(_clusters.begin(), _clusters.end(), tindex, [](size_t tindex, const cluster & cl) { return  tindex < cl.trange.end(); });
        if (iter == _clusters.end())
            return empty_cluster;
        return *iter;
    }

    const cluster & Shaper::findCluster(float32_t pos, size_t lindex) const
    {
        auto & row = _rows[lindex];

        auto iter_seg = std::upper_bound(_segments.begin() + row.srange.index, _segments.begin() + row.srange.end(), pos, [](float32_t pos, const segment & seg) { return  pos < seg.offset + seg.width; });
        if (iter_seg == _segments.begin() + row.srange.end() || iter_seg->offset + iter_seg->width <= pos)
            return empty_cluster;


        auto iter = std::upper_bound(_clusters.begin() + iter_seg->crange.index, _clusters.begin() + iter_seg->crange.end(), pos, [](float32_t pos, const cluster & cl) { return  pos < cl.rect.right(); });
        if (iter == _clusters.begin() + iter_seg->crange.end())
            return empty_cluster;
        return *iter;
    }
}
