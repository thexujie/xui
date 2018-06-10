#include "stdafx.h"
#include "ScriptService.h"
#include "core/unicode.h"

namespace win32::uniscribe
{
    static LOGFONTW MappingFont(const graphics::text::font & font)
    {
        LOGFONTW logfont = {};
        if (font.family[0])
            core::textcpy(logfont.lfFaceName, LF_FACESIZE, core::string::u8_ucs2(font.family).data(), -1);
        else
            core::textcpy(logfont.lfFaceName, LF_FACESIZE, core::string::u8_ucs2(win32::defaultFont().family).data(), -1);

        //int iDpiY = GetDeviceCaps(hdc, LOGPIXELSY);
        logfont.lfWidth = 0;
        //logFont.lfHeight = (int32_t)(-font.size * 72 / iDpiY);
        logfont.lfHeight = -static_cast<int32_t>(font.size);
        logfont.lfWeight = static_cast<int32_t>(font.weight);

        logfont.lfItalic = static_cast<uint8_t>(font.flags & graphics::text::font::italic);
        logfont.lfUnderline = static_cast<uint8_t>(font.flags & graphics::text::font::underline);
        //logfont.lfStrikeOut = static_cast<uint8_t>(font.flags & font::underline);
        //logFont.lfCharSet = static_cast<uint8_t>(font.charset);
        logfont.lfCharSet = DEFAULT_CHARSET;
        logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
        logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;

        if (font.flags & graphics::text::font::gray)
            logfont.lfQuality = NONANTIALIASED_QUALITY;
        else if (font.flags & graphics::text::font::anti)
            logfont.lfQuality = ANTIALIASED_QUALITY;
        else if (font.flags & graphics::text::font::cleartype)
            logfont.lfQuality = CLEARTYPE_QUALITY;
        else
            logfont.lfQuality = CLEARTYPE_QUALITY;
        logfont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
        return logfont;
    }


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

            LOGFONTW logFont = MappingFont(font);
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
            vfont.metrics.decent = tm.tmDescent;
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
