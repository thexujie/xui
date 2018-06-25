#include "stdafx.h"
#include "ScriptService.h"
#include "core/unicode.h"
#include "win32/windows.h"

namespace win32::uniscribe
{
    ScriptService::ScriptService()
    {
        HDC hdcScreen = GetDC(NULL);
        _hdc = CreateCompatibleDC(hdcScreen);
        ReleaseDC(NULL, hdcScreen);

        MapFont(win32::defaultFont());
    }

    ScriptService::~ScriptService()
    {
        for (view_font_t & vfont : _fonts)
        {
            DeleteObject(vfont.hfont);
            ScriptFreeCache(vfont.cache);
            delete vfont.cache;
        }

        if (_hdc)
        {
            ::DeleteDC(_hdc);
            _hdc = NULL;
        }
    }

    int32_t ScriptService::MapFont(const graphics::text::font & font)
    {
        int32_t ifont = -1;
        for (int32_t cnt = 0; cnt < _fonts.size(); ++cnt)
        {
            if (_fonts[cnt].font == font)
            {
                ifont = cnt;
                break;
            }
        }

        if (ifont < 0)
        {
            ifont = _fonts.size();

            LOGFONTW logFont = win32::MappingFont(font);
            HFONT hFont = CreateFontIndirectW(&logFont);
            if (!hFont)
                throw 0();

            view_font_t vfont;
            vfont.font = font;
            vfont.hfont = hFont;
            vfont.cache = new SCRIPT_CACHE;

            TEXTMETRICW tm = {};
            HGDIOBJ hFontOld = ::SelectObject(_hdc, hFont);
            ::GetTextMetricsW(_hdc, &tm);
            ::SelectObject(_hdc, hFontOld);

            vfont.metrics.size = font.size;
            vfont.metrics.height = tm.tmHeight;
            vfont.metrics.ascent = tm.tmAscent;
            vfont.metrics.descent = tm.tmDescent;
            vfont.metrics.linespace = tm.tmHeight;
            vfont.metrics.weight = tm.tmWeight;

            *vfont.cache = nullptr;
            _fonts.push_back(vfont);
        }
        return ifont;
    }

    const view_font_t &  ScriptService::GetFont(int32_t ifont) const
    {
        return _fonts[ifont];
    }

    graphics::text::font ScriptService::GetFontFallBack(const graphics::text::font & font, int32_t iLanguage, const wchar_t * text, int32_t length)
    {
        char32_t ch;
        // skip spaces
        while (length > 0 && *text == ' ')
        {
            ++text;
            --length;
        }

        int32_t len = core::utf16_to_unicode(reinterpret_cast<const char16_t *>(text), length, ch);
        if (len <= 0)
            return {};

        graphics::text::font fontfb = font;
        fontfb.family.clear();
        const core::unicodeplane_t & plane = core::unicodeplane_find(reinterpret_cast<const char16_t *>(text), len);
        switch (plane.plane)
        {
        case core::unicodeplane_hebrew:
            fontfb.family = "Courier New";
            break;
        case core::unicodeplane_arabic: // lang 26
            fontfb.family = "Times New Roman";
            break;

        case core::unicodeplane_sinhala: // lang 31
            fontfb.family = "Tahoma";
            break;

        case core::unicodeplane_hangul_syllables: // lang 19
            fontfb.family = "Malgun Gothic";
            break;

        case core::unicodeplane_cjk_unified_ideographs_ext_b: // lang 12
            fontfb.family = "SimSun-ExtB";
            break;

        default:
            break;
        }
        return fontfb;
    }
}
