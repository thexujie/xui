#include "stdafx.h"
#include "script.h"

namespace script
{
    core::error Shaper::reset(std::string text)
    {
        _text = text;
        itermize();
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

        bool leftToRight = true;
        UBiDiLevel defaultLevel = leftToRight ? UBIDI_DEFAULT_LTR : UBIDI_DEFAULT_RTL;
        // The required lifetime of utf16 isn't well documented.
        // It appears it isn't used after ubidi_setPara except through ubidi_getText.
        ubidi_setPara(bidi.get(), u16str.c_str(), u16str.length(), defaultLevel, nullptr, &status);
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
        do
        {
            char32_t ch = 0;
            bool flush = false;
            size_t nchar = core::utf16_to_unicode(u16str.c_str() + pos, core::npos, ch);

            UBiDiLevel level = ubidi_getLevelAt(bidi.get(), pos);
            hb_script_t script = hb_unicode_script(hb_unicode, ch);

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
                flush = true;
                // 把最后一个字符包括进去
                pos += nchar;
                nchar = 0;
            }
            else
            {
                if(level != level_last)
                    flush = true;

                if(script != script_last)
                {
                    if (script == HB_SCRIPT_INHERITED || script == HB_SCRIPT_COMMON)
                        script = script_last;
                    else if (script_last == HB_SCRIPT_INHERITED || script_last == HB_SCRIPT_COMMON)
                    {
                    }
                    else
                    {
                        flush = true;
                    }
                }
            }

            if(flush)
            {
                item item = {{ pos_start, pos - pos_start }, script_last, !!(level_last & 1)};
#ifdef _DEBUG
                item._text = u16str.substr(pos_start, pos - pos_start);
#endif
                _items.push_back(item);
                pos_start = pos;
                level_last = level;
                script_last = script;
            }

            pos += nchar;
        }
        while (pos < u16str.length());

        return core::error_ok;
    }

    core::error Shaper::clusterize()
    {
        return core::error_ok;
    }
}