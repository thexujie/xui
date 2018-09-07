#include "stdafx.h"
#include "script.h"
#include "drawing/skia/skia.h"

namespace drawing::script
{
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

    core::error Shaper::reset(std::string text)
    {
        _text = text;
        _rtf_font_indices.assign(_text.length(), font_default);
        _rtf_colors.assign(_text.length(), _color);

        _font_indices.clear();
        _fonts.clear();
        fontIndex(_font);

        _hbbuffer.reset(hb_buffer_create());
        UErrorCode status = U_ZERO_ERROR;
        _breaker.reset(icu::BreakIterator::createLineInstance(icu::Locale::getDefault(), status));
        return core::error_ok;
    }

    core::error Shaper::itermize()
    {
        if(_text.empty())
            return core::error_ok;

        std::u16string u16str = core::u8str_u16str(_text);

        UErrorCode status = U_ZERO_ERROR;
        std::unique_ptr<UBiDi, decltype(&ubidi_close)> bidi(ubidi_openSized(u16str.length(), 0, &status), &ubidi_close);
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
        ubidi_setPara(bidi.get(), u16str.c_str(), u16str.length(), _defaultBidiLevel, nullptr, &status);
        if (U_FAILURE(status))
        {
            //SkDebugf("Bidi error: %s", u_errorName(status));
        }

        struct bidilevel
        {
            section range;
            UBiDiLevel level;
        };
        std::vector<bidilevel> levels;

        hb_buffer_clear_contents(_hbbuffer.get());
        hb_buffer_set_content_type(_hbbuffer.get(), HB_BUFFER_CONTENT_TYPE_UNICODE);
        hb_buffer_set_cluster_level(_hbbuffer.get(), HB_BUFFER_CLUSTER_LEVEL_MONOTONE_CHARACTERS);

        hb_unicode_funcs_t * hb_unicode = hb_buffer_get_unicode_funcs(_hbbuffer.get());

        auto str = icu::UnicodeString::fromUTF8({ _text.c_str(), (int32_t)_text.length() });
        _breaker->setText(str);

        size_t utf8_start = 0;
        size_t utf16_start = 0;
        size_t utf32_start = 0;
        size_t utf8_pos = 0;
        size_t utf16_pos = 0;
        size_t utf32_pos = 0;
        char32_t ch = 0;

        UBiDiLevel level_last = 0;
        hb_script_t script_last = HB_SCRIPT_UNKNOWN;

        uint32_t color_last = _color;
        uint16_t font_index_last = core::nposu16;
        uint16_t font_index_fb_last = core::nposu16;

        UBiDiLevel level = 0;
        hb_script_t script = HB_SCRIPT_UNKNOWN;

        uint32_t color = _color;
        uint16_t font_index = core::nposu16;

        enum class flushflag
        {
            eof = 0x0001,
            level = 0x0002,
            script = 0x0004,
            font = 0x0008,
            color = 0x0010,
        };
        typedef core::bitflag<flushflag> flushflags;

        do
        {
            flushflags flush = nullptr;

            size_t nutf8 = 1;
            size_t nutf16 = 1;
            size_t nutf32 = 1;

            if (utf16_pos >= u16str.length())
            {
                flush |= flushflag::eof;
            }
            else
            {
                char32_t ch2 = 0;
                nutf8 = core::utf8_to_utf32(_text.c_str() + utf8_pos, _text.length() - utf8_pos, ch2);
                nutf16 = core::utf16_to_utf32(u16str.c_str() + utf16_pos, u16str.length() - utf16_pos, ch);
                nutf32 = 1;
#ifdef _DEBUG
                _u32text.push_back(ch);
#endif
                // 可能是删除字符时搞错了，比如 utf8 4 char，删掉一个变成了 3char，就非法了，结束排版
                if (ch != ch2)
                    continue;

                _chars.push_back({ utf8_pos, nutf8 });
                level = ubidi_getLevelAt(bidi.get(), utf16_pos);
                script = hb_unicode_script(hb_unicode, ch);

                font_index = _rtf_font_indices[utf16_pos];
                color = _rtf_colors[utf16_pos];

                if (utf16_pos == 0)
                {
                    level_last = level;
                    script_last = script;
                }
                else
                {

                }

                if (level != level_last)
                    flush |= flushflag::level;

                if (script != script_last)
                {
                    if (script == HB_SCRIPT_INHERITED || script == HB_SCRIPT_COMMON)
                        script = script_last;
                    else if (script_last == HB_SCRIPT_INHERITED || script_last == HB_SCRIPT_COMMON)
                    {
                    }
                    else
                    {
                        flush |= flushflag::script;
                    }
                }

                auto & font_cache = _fonts[font_index];
                if (font_index_fb_last != core::nposu16 && !flush.any(flushflag::script) && _fonts[font_index_fb_last].skfont->charsToGlyphs(&ch, SkTypeface::kUTF32_Encoding, nullptr, 1))
                {
                    // 使用上一个 fb 的字体搞定了
                    font_index = font_index_fb_last;
                }
                else if (font_cache.skfont->charsToGlyphs(&ch, SkTypeface::kUTF32_Encoding, nullptr, 1))
                {
                    font_index_fb_last = 0;
                }
                else
                {
                    font_index_fb_last = 0;
                    sk_sp<SkFontMgr> fontMgr = SkFontMgr::RefDefault();
                    auto tf = fontMgr->matchFamilyStyleCharacter(nullptr, drawing::skia::from(font_cache.font.style), nullptr, 0, ch);
                    if (tf)
                    {
                        drawing::font font_fb = drawing::skia::to(*tf, font_cache.font.size);
                        uint16_t font_index_fb = fontIndex(font_fb);
                        font_index_fb_last = font_index_fb;
                        font_index = font_index_fb;
                    }
                }

                if (font_index_last == core::nposu16)
                    font_index_last = font_index;

                if (font_index != font_index_last)
                    flush |= flushflag::font;
                if (color != color_last)
                    flush |= flushflag::color;
            }

            if (flush.any() && utf8_pos > utf8_start)
            {
                item item = { { utf8_start, utf8_pos - utf8_start }, script_last, !!(level_last & 1), font_index_last, color_last };
#ifdef _DEBUG
                item._text = _text.substr(utf8_start, utf8_pos - utf8_start);
                auto iter = std::find_if(_font_indices.begin(), _font_indices.end(), [font_index_last](const auto & vt) { return vt.second == font_index_last; });
                item._font = iter->first;
#endif
                _items.push_back(item);
                utf8_start = utf8_pos;
                utf16_start = utf16_pos;
                utf32_start = utf32_pos;
                level_last = level;
                script_last = script;
                font_index_last = font_index;
                color_last = color;

                //--------------------------------------------------- shape
                {
                    // TODO: features
                    const drawing::font & font = font_at(item.font);
                    auto hbfont = hbfont_at(item.font);

                    hb_buffer_set_script(_hbbuffer.get(), item.script);
                    hb_buffer_set_direction(_hbbuffer.get(), item.rtl ? HB_DIRECTION_RTL : HB_DIRECTION_LTR);
                    // TODO: language
                    hb_buffer_guess_segment_properties(_hbbuffer.get());

                    hb_shape(hbfont.get(), _hbbuffer.get(), nullptr, 0);
                    uint32_t len = hb_buffer_get_length(_hbbuffer.get());
                    size_t index_base = _glyphs.size();
                    _glyphs.resize(_glyphs.size() + len);
                    if (item.rtl)
                    {
                        // Put the clusters back in logical order.
                        // Note that the advances remain ltr.
                        hb_buffer_reverse(_hbbuffer.get());
                    }

                    int32_t scaleX = 0, scaleY = 0;
                    hb_font_get_scale(hbfont.get(), &scaleX, &scaleY);
                    float32_t coefX = font.size *  1.0f / scaleX;
                    float32_t coefY = font.size * 1.0f / scaleY;

                    hb_glyph_info_t * infos = hb_buffer_get_glyph_infos(_hbbuffer.get(), nullptr);
                    hb_glyph_position_t * poses = hb_buffer_get_glyph_positions(_hbbuffer.get(), nullptr);
                    uint32_t ig_last = 0;
                    for (uint32_t ig = 0; ig < len; ++ig)
                    {
                        const hb_glyph_info_t & info = infos[ig];
                        const hb_glyph_position_t & pos = poses[ig];
                        glyph & glyph = _glyphs[index_base + ig];
                        glyph.trange = { info.cluster, 1 };
                        if (ig < len - 1)
                            glyph.trange.length = infos[ig + 1].cluster - info.cluster;
                        else
                            glyph.trange.length = utf8_pos - info.cluster;
#ifdef _DEBUG
                        glyph._text = _text.substr(glyph.trange.index, glyph.trange.length);
#endif
                        glyph.gid = uint16_t(info.codepoint);
                        glyph.advance = { float32_t(pos.x_advance * coefX), float32_t(pos.y_advance * coefY) };
                        glyph.offset = { float32_t(pos.x_offset * coefX), float32_t(pos.y_offset * coefY) };

                        int32_t index = _breaker->current();
                        if (index != icu::BreakIterator::DONE && index < glyph.trange.index)
                            index = _breaker->next();

                        glyph.softbreak = index == glyph.trange.index;
                        glyph.standalone = !(info.mask & HB_GLYPH_FLAG_UNSAFE_TO_BREAK);
                    }

                    hb_buffer_clear_contents(_hbbuffer.get());
                }
            }
            else
            {
            }

            hb_buffer_add(_hbbuffer.get(), ch, utf8_pos);
            utf8_pos += nutf8;
            utf16_pos += nutf16;
            utf32_pos += nutf32;
        }
        while (utf16_pos <= u16str.length());

        return core::error_ok;
    }

    core::error Shaper::wrap(float32_t end, wrap_mode mode)
    {
        _rows.push_back({});
        _segments.push_back({});
        ++_rows.back().srange.length;
        if (_text.empty())
            return core::error_ok;

        float32_t last = 0;
        float32_t curr = 0;
        size_t gstart = 0;
        uint32_t line = 0;

        enum class flushflag
        {
            eof = 0x0001,
            width = 0x0002,
            item = 0x0004,
        };
        typedef core::bitflag<flushflag> flushflags;

        for(size_t gindex = 0, iindex = 0; gindex < _glyphs.size(); ++gindex)
        {
            flushflags flush = nullptr;
            glyph & glyph = _glyphs[gindex];
            if (glyph.trange.index >= _items[iindex].trange.end())
            {
                ++iindex;
                flush |= flushflag::item;
            }

            if (curr + glyph.advance.cx > end && glyph.standalone && _segments.back().grange.length > 0)
                flush |= flushflag::width;
            else {}

            if(flush.any())
            {
                segment seg = {};
                seg.item = iindex;
                seg.line = line;
                seg.offset = curr;
                seg.trange.index = glyph.trange.index;
                seg.grange.index = gindex;
                _segments.push_back(seg);

                if (flush.any(flushflag::width))
                {
                    row new_row = {};
                    new_row.trange.index = glyph.trange.index;
                    new_row.grange.index = gindex;
                    new_row.srange = { _segments.size() - 1, 0};
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
            curr += glyph.advance.cx;

            segment & seg_last = _segments.back();
            seg_last.trange.length += glyph.trange.length;
            seg_last.grange.length += 1;
            seg_last.width += glyph.advance.cx;

            row & row_this = _rows.back();
            auto & fmetrics = _fonts[_items[iindex].font].fmetrics;
            row_this.trange.length += glyph.trange.length;
            row_this.grange.length += 1;
            row_this.width += glyph.advance.cx;
            row_this.ascent = std::max(row_this.ascent, fmetrics.ascent);
            row_this.descent = std::max(row_this.descent, fmetrics.descent);
        }

#ifdef _DEBUG
        for (auto & seg : _segments)
            seg._text = _text.substr(seg.trange.index, seg.trange.length);
        for(auto & row : _rows)
            row._text = _text.substr(row.trange.index, row.trange.length);
#endif
        return core::error_ok;
    }

    core::error Shaper::shape(SkTextBlobBuilder & builder, uint32_t index)
    {
        if (_text.empty())
            return core::error_ok;

        row & row = _rows[index];
        float32_t offset_y = 0;
        for(size_t sindex = row.srange.index; sindex < row.srange.end(); ++sindex)
        {
            segment & seg = _segments[sindex];
            item & item = _items[seg.item];

            auto & font_cache = _fonts[item.font];
            float32_t offset_x = seg.offset;
            SkPaint paint;
            paint.setTypeface(sk_ref_sp(font_cache.skfont.get()));
            paint.setTextSize(font_cache.font.size);
            paint.setAntiAlias(true);
            paint.setLCDRenderText(true);
            paint.setColor(item.color);
            paint.setTextSize(_font.size);
            paint.setAutohinted(true);
            paint.setTextEncoding(SkPaint::kGlyphID_TextEncoding);

            SkTextBlobBuilder::RunBuffer runBuffer = builder.allocRunTextPos(paint, seg.grange.length, seg.trange.length, SkString(), nullptr);
            memcpy(runBuffer.utf8text, _text.c_str() + seg.trange.index, seg.trange.length);

            for (size_t gindex = seg.grange.index, iglyph = 0; gindex < seg.grange.end(); ++gindex, ++iglyph)
            {
                glyph & glyph = _glyphs[item.rtl ? (seg.grange.end() - 1 - iglyph) : gindex];

                runBuffer.glyphs[iglyph] = glyph.gid;
                runBuffer.clusters[iglyph] = glyph.trange.index;
                runBuffer.pos[iglyph * 2 + 0] = offset_x + glyph.offset.x;
                runBuffer.pos[iglyph * 2 + 1] = offset_y - glyph.offset.y + row.ascent;
                offset_x += glyph.advance.cx;
                offset_y += glyph.advance.cy;
            }

        }
        return core::error_ok;
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

    void Shaper::setFont(section range, const drawing::font & font)
    {
        uint16_t index = fontIndex(font);
        std::fill(_rtf_font_indices.begin() + range.index, _rtf_font_indices.begin() + range.end(), index);
    }

    void Shaper::setColor(section range, uint32_t color)
    {
        std::fill(_rtf_colors.begin() + range.index, _rtf_colors.begin() + range.end(), color);
    }

    uint16_t Shaper::fontIndex(const drawing::font & font)
    {
        uint16_t index = font_default;
        auto iter = _font_indices.find(font);
        if (iter == _font_indices.end())
        {
            auto skfont = std::shared_ptr<SkTypeface>(SkTypeface::MakeFromName(font.family.c_str(), drawing::skia::from(font.style)).release(), drawing::skia::skia_unref);
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
}
