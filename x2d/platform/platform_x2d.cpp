#include "stdafx.h"
#include "win32.h"

namespace win32
{
    drawing::font defaultFont()
    {
        NONCLIENTMETRICSW metrics = { sizeof(NONCLIENTMETRICSW) };
        SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, metrics.cbSize, &metrics, 0);

        drawing::font font;
        font.family = core::wstr_u8str(metrics.lfMessageFont.lfFaceName);
        font.size = float(metrics.lfMessageFont.lfHeight);
        return font;
    }

    LOGFONTW MappingFont(HDC hdc, const drawing::font & font)
    {
        LOGFONTW logfont = {};
        if (font.family[0])
            core::textcpy(logfont.lfFaceName, LF_FACESIZE, core::u8str_wstr(font.family).data(), -1);
        else
            core::textcpy(logfont.lfFaceName, LF_FACESIZE, core::u8str_wstr(win32::defaultFont().family).data(), -1);

        logfont.lfWidth = 0;
        if (font.size > 0)
        {
            int iDpiY = GetDeviceCaps(hdc, LOGPIXELSY);
            logfont.lfHeight = -static_cast<int32_t>(font.size * iDpiY / 72);
        }
        else
            logfont.lfHeight = std::lroundf(font.size);

        logfont.lfWeight = static_cast<int32_t>(font.style.weight);

        logfont.lfItalic = static_cast<uint8_t>(font.style.slant);
        logfont.lfUnderline = static_cast<uint8_t>(0);
        //logfont.lfStrikeOut = static_cast<uint8_t>(font.flags & font::underline);
        //logFont.lfCharSet = static_cast<uint8_t>(font.charset);
        logfont.lfCharSet = DEFAULT_CHARSET;
        logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
        logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;

        logfont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
        return logfont;
    }

    LOGFONTW MappingFont(const drawing::font & font)
    {
        HDC hdcScreen = GetDC(NULL);
        LOGFONTW logfont = MappingFont(hdcScreen, font);
        ReleaseDC(NULL, hdcScreen);
        return logfont;
    }
}
