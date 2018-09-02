#include "stdafx.h"
#include "script.h"
#include "drawing/skia/skia.h"

namespace script
{

    core::error Shaper::reset(std::string text)
    {
        _text = text;
        _rtf_font_indices.assign(_text.length(), font_default);
        _rtf_colors.assign(_text.length(), color_default);

        _font_indices.clear();
        _fonts.clear();

        auto font_ptr = std::shared_ptr<SkTypeface>(SkTypeface::MakeFromName(_font.family.c_str(), drawing::skia::from(_font.style)).release(), drawing::skia::skia_unref);
        _fonts.push_back({ _font, font_ptr });
        _font_indices[_font] = 0;
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

        std::unique_ptr<hb_buffer_t, decltype(&hb_buffer_destroy)> hbbuffer(hb_buffer_create(), hb_buffer_destroy);
        hb_buffer_set_content_type(hbbuffer.get(), HB_BUFFER_CONTENT_TYPE_UNICODE);
        hb_buffer_set_cluster_level(hbbuffer.get(), HB_BUFFER_CLUSTER_LEVEL_MONOTONE_CHARACTERS);

        hb_unicode_funcs_t * hb_unicode = hb_buffer_get_unicode_funcs(hbbuffer.get());

        int32_t pos_start = 0;
        int32_t pos = 0;

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
            char32_t ch = 0;
            flushflags flush = nullptr;
            size_t nchar = core::utf16_to_unicode(u16str.c_str() + pos, core::npos, ch);

            UBiDiLevel level = ubidi_getLevelAt(bidi.get(), pos);
            hb_script_t script = hb_unicode_script(hb_unicode, ch);
            uint16_t font_index = _rtf_font_indices[pos];
            uint32_t color = _rtf_colors[pos];

            if (pos == 0)
            {
                level_last = level;
                script_last = script;
            }
            else
            {
                
            }

            if(pos + nchar >= u16str.length())
            {
                flush |= flushflag::eof;
                // 把最后一个字符包括进去
                pos += nchar;
                nchar = 0;
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

            auto & font = _fonts[font_index];
            if (font_index_last_fb > 0 && !flush.any(flushflag::script) && _fonts[font_index_last_fb].second->charsToGlyphs(&ch, SkTypeface::kUTF32_Encoding, nullptr, 1))
            {
                // 使用上一个 fb 的字体搞定了
                font_index = font_index_last_fb;
            }
            else if (font.second->charsToGlyphs(&ch, SkTypeface::kUTF32_Encoding, nullptr, 1))
            {
                font_index_last_fb = 0;
            }
            else
            {
                font_index_last_fb = 0;
                sk_sp<SkFontMgr> fontMgr = SkFontMgr::RefDefault(); 
                auto tf = fontMgr->matchFamilyStyleCharacter( nullptr, drawing::skia::from(font.first.style), nullptr, 0, ch);
                if(tf)
                {
                    drawing::font font_fb = drawing::skia::to(*tf, font.first.size);
                    uint16_t font_index_fb = fontIndex(font_fb);
                    font_index_last_fb = font_index_fb;
                    font_index = font_index_fb;
                }
            }

            if (font_index != font_index_last)
                flush |= flushflag::font;
            if (color != color_last)
                flush |= flushflag::color;

            if (flush.any())
            {
                item item = {{ pos_start, pos - pos_start }, script_last, !!(level_last & 1), font_index_last, color_last};
#ifdef _DEBUG
                item._text = u16str.substr(pos_start, pos - pos_start);
                auto iter = std::find_if(_font_indices.begin(), _font_indices.end(), [font_index_last](const auto & vt) { return vt.second == font_index_last; });
                item._font = iter->first;
#endif
                _items.push_back(item);
                pos_start = pos;
                level_last = level;
                script_last = script;
                font_index_last = font_index;
                color_last = color;
            }

            pos += nchar;
        }
        while (pos < u16str.length());

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
            auto font_ptr = std::shared_ptr<SkTypeface>(SkTypeface::MakeFromName(font.family.c_str(), drawing::skia::from(font.style)).release(), drawing::skia::skia_unref);
            _fonts.push_back({ font, font_ptr });
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
