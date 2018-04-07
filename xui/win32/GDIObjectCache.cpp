#include "stdafx.h"
#include "GDIObjectCache.h"
#include "win32/windows.h"

namespace win32
{
    using namespace core;
    using namespace graphics;

    void FontToLOGFONT(HDC hdc, const font & font, LOGFONTW & logFont)
    {
        if (font.family.empty())
            textcpy(logFont.lfFaceName, LF_FACESIZE, core::string::u8_ucs2(win32::defaultFont().family).c_str(), -1);
        else
            textcpy(logFont.lfFaceName, LF_FACESIZE, core::string::u8_ucs2(font.family).c_str(), -1);

        int iDpiY = GetDeviceCaps(hdc, LOGPIXELSY);
        logFont.lfWidth = 0;
        logFont.lfHeight = (int32_t)(-font.size * 72 / iDpiY);
        logFont.lfWeight = (int32_t)font.weight;

        logFont.lfItalic = !!(font.flags & font::italic);
        logFont.lfUnderline = !!(font.flags & font::underline);
        logFont.lfStrikeOut = FALSE;
        //logFont.lfCharSet = (uint_8)(font.charset);
        logFont.lfCharSet = DEFAULT_CHARSET;
        logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
        logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
        if (font.flags & font::gray)
            logFont.lfQuality = DRAFT_QUALITY;
        else if (font.flags & font::anti)
            logFont.lfQuality = ANTIALIASED_QUALITY;
        else
            logFont.lfQuality = CLEARTYPE_QUALITY;
        logFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    }


    GDIObjectCache::GDIObjectCache(handle_t hdc):_hdc(hdc)
    {
        
    }

    handle_t GDIObjectCache::GetPen(const core::color32 color, float32_t width)
    {
        auto iter = _pens.find({ color, width });
        if (iter != _pens.end())
            return iter->second;

        HPEN hPen = NULL;
        if (color == colors::Transparent)
            hPen = (HPEN)GetStockObject(NULL_PEN);
        else
            hPen = ::CreatePen(PS_SOLID, (int32_t)width, color & 0x00FFFFFF);

        _pens[{color, width}] = hPen;
        return hPen;
    }

    handle_t GDIObjectCache::GetBrush(const core::color32 color)
    {
        auto iter = _brushs.find(color);
        if (iter != _brushs.end())
            return iter->second;

        HBRUSH hBrush = NULL;
        if (color == colors::Transparent)
            hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
        else
            hBrush = ::CreateSolidBrush(color & 0x00FFFFFF);

        _brushs[color] = hBrush;
        return hBrush;
    }

    handle_t GDIObjectCache::GetFont(const graphics::font & font)
    {
        auto iter = _fonts.find(font);
        if (iter != _fonts.end())
            return iter->second;

        LOGFONTW logFont = {};
        FontToLOGFONT((HDC)_hdc, font, logFont);

        HFONT hFont = CreateFontIndirectW(&logFont);
        std::hash<graphics::font>{}(font);

        _fonts[font] = hFont;
        return hFont;
    }
}
