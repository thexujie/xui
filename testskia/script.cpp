#include "stdafx.h"
#include "script.h"
#include "drawing/skia/skia.h"

namespace script
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
        _rtf_colors.assign(_text.length(), color_default);

        _font_indices.clear();
        _fonts.clear();
        fontIndex(_font);

        _hbbuffer.reset(hb_buffer_create(), hb_buffer_destroy);
        return core::error_ok;
    }

    core::error Shaper::itermize()
    {
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
            range range;
            UBiDiLevel level;
        };
        std::vector<bidilevel> levels;
        std::vector<item> items;

        hb_buffer_clear_contents(_hbbuffer.get());
        hb_buffer_set_content_type(_hbbuffer.get(), HB_BUFFER_CONTENT_TYPE_UNICODE);
        hb_buffer_set_cluster_level(_hbbuffer.get(), HB_BUFFER_CLUSTER_LEVEL_MONOTONE_CHARACTERS);

        hb_unicode_funcs_t * hb_unicode = hb_buffer_get_unicode_funcs(_hbbuffer.get());

        int32_t pos_start = 0;
        int32_t utf16_pos = 0;
        int32_t utf8_pos = 0;

        UBiDiLevel level_last = 0;
        hb_script_t script_last = HB_SCRIPT_UNKNOWN;
        uint16_t font_index_last = 0;
        uint16_t font_index_last_fb = 0;
        uint32_t color_last = color_default;
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
            char32_t ch = 0, ch2 = 0;
            size_t nutf16 = core::utf16_to_utf32(u16str.c_str() + utf16_pos, u16str.length() - utf16_pos, ch);
            size_t nutf8 = core::utf8_to_utf32(_text.c_str() + utf8_pos, _text.length() - utf8_pos, ch2);
            assert(ch == ch2);
            _chars.push_back({ utf8_pos, (int32_t)nutf8 });
            UBiDiLevel level = ubidi_getLevelAt(bidi.get(), utf16_pos);
            hb_script_t script = hb_unicode_script(hb_unicode, ch);

            flushflags flush = nullptr;

            uint16_t font_index = _rtf_font_indices[utf16_pos];
            uint32_t color = _rtf_colors[utf16_pos];

            if (utf16_pos == 0)
            {
                level_last = level;
                script_last = script;
            }
            else
            {
                
            }

            if(utf16_pos + nutf16 >= u16str.length())
            {
                flush |= flushflag::eof;
                hb_buffer_add(_hbbuffer.get(), ch, utf8_pos);
                // 把最后一个字符包括进去
                utf16_pos += nutf16;
                nutf16 = 0;
            }
            else
            {
                if(level != level_last)
                    flush |= flushflag::level;

                if(script != script_last)
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
            }

            auto [font, skfont, hbfont] = _fonts[font_index];
            if (font_index_last_fb > 0 && !flush.any(flushflag::script) && std::get<1>(_fonts[font_index_last_fb])->charsToGlyphs(&ch, SkTypeface::kUTF32_Encoding, nullptr, 1))
            {
                // 使用上一个 fb 的字体搞定了
                font_index = font_index_last_fb;
            }
            else if (skfont->charsToGlyphs(&ch, SkTypeface::kUTF32_Encoding, nullptr, 1))
            {
                font_index_last_fb = 0;
            }
            else
            {
                font_index_last_fb = 0;
                sk_sp<SkFontMgr> fontMgr = SkFontMgr::RefDefault(); 
                auto tf = fontMgr->matchFamilyStyleCharacter( nullptr, drawing::skia::from(font.style), nullptr, 0, ch);
                if(tf)
                {
                    drawing::font font_fb = drawing::skia::to(*tf, font.size);
                    uint16_t font_index_fb = fontIndex(font_fb);
                    font_index_last_fb = font_index_fb;
                    font_index = font_index_fb;
                }
            }

            if (font_index != font_index_last)
                flush |= flushflag::font;
            if (color != color_last)
                flush |= flushflag::color;

            if (flush.any() && utf16_pos > pos_start)
            {
                item item = {{ pos_start, utf16_pos - pos_start }, script_last, !!(level_last & 1), font_index_last, color_last};
#ifdef _DEBUG
                item._text = u16str.substr(pos_start, utf16_pos - pos_start);
                auto iter = std::find_if(_font_indices.begin(), _font_indices.end(), [font_index_last](const auto & vt) { return vt.second == font_index_last; });
                item._font = iter->first;
#endif
                _items.push_back(item);
                pos_start = utf16_pos;
                level_last = level;
                script_last = script;
                font_index_last = font_index;
                color_last = color;

                //--------------------------------------------------- shape
                {
                    // TODO: features
                    auto hbfont = hbfont_at(item.font);

                    hb_buffer_set_script(_hbbuffer.get(), item.script);
                    hb_buffer_set_direction(_hbbuffer.get(), item.rtl ? HB_DIRECTION_RTL : HB_DIRECTION_LTR);
                    // TODO: language
                    hb_buffer_guess_segment_properties(_hbbuffer.get());

                    hb_shape(hbfont.get(), _hbbuffer.get(), nullptr, 0);
                    uint32_t len = hb_buffer_get_length(_hbbuffer.get());

                    if (item.rtl)
                    {
                        // Put the clusters back in logical order.
                        // Note that the advances remain ltr.
                        hb_buffer_reverse(_hbbuffer.get());
                    }
                    hb_glyph_info_t * info = hb_buffer_get_glyph_infos(_hbbuffer.get(), nullptr);
                    hb_glyph_position_t * pos = hb_buffer_get_glyph_positions(_hbbuffer.get(), nullptr);
                    for(uint32_t cnt = 0; cnt < len; ++cnt)
                    {
                        
                    }

                    hb_buffer_clear_contents(_hbbuffer.get());
                }
            }
            else
            {
            }

            hb_buffer_add(_hbbuffer.get(), ch, utf8_pos);
            utf16_pos += nutf16;
            utf8_pos += nutf8;
        }
        while (utf16_pos < u16str.length());

        return core::error_ok;
    }


    core::error Shaper::shape()
    {
        for(size_t index = 0; index < _items.size(); ++index)
        {
            auto & item = _items[index];


        }
        return core::error_ok;
    }

    void Shaper::setFont(range range, const drawing::font & font)
    {
        uint16_t index = fontIndex(font);
        std::fill(_rtf_font_indices.begin() + range.index, _rtf_font_indices.begin() + range.end(), index);
    }

    void Shaper::setColor(range range, uint32_t color)
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

            _fonts.push_back(std::make_tuple(font, skfont, hbfont));
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
