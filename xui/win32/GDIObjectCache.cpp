#include "stdafx.h"
#include "GDIObjectCache.h"
#include "win32/windows.h"

namespace win32
{
    using namespace core;
    using namespace graphics;

    void FontToLOGFONT(HDC hdc, const text::font & font, LOGFONTW & logFont)
    {
        if (font.family.empty())
            textcpy(logFont.lfFaceName, LF_FACESIZE, core::string::u8_ucs2(win32::defaultFont().family).c_str(), -1);
        else
            textcpy(logFont.lfFaceName, LF_FACESIZE, core::string::u8_ucs2(font.family).c_str(), -1);

        int iDpiY = GetDeviceCaps(hdc, LOGPIXELSY);
        logFont.lfWidth = 0;
        if(font.size < 0)
            logFont.lfHeight = (int32_t)(font.size * 72 / iDpiY);
        else
            logFont.lfWeight = font.size;

        logFont.lfWeight = font.weight;
        logFont.lfItalic = !!(font.flags & text::font::italic);
        logFont.lfUnderline = !!(font.flags & text::font::underline);
        logFont.lfStrikeOut = FALSE;
        //logFont.lfCharSet = (uint_8)(font.charset);
        logFont.lfCharSet = DEFAULT_CHARSET;
        logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
        logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
        if (font.flags & text::font::gray)
            logFont.lfQuality = DRAFT_QUALITY;
        else if (font.flags & text::font::anti)
            logFont.lfQuality = ANTIALIASED_QUALITY;
        else
            logFont.lfQuality = CLEARTYPE_QUALITY;
        logFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    }


    GDIObjectCache::GDIObjectCache(std::shared_ptr<HDC> hdc):_hdc(hdc)
    {
        
    }

    std::shared_ptr<HPEN> GDIObjectCache::GetPen(const core::color32 color, float32_t width)
    {
        auto iter = _pens.find({ color, width });
        if (iter != _pens.end())
            return iter->second;

        HPEN hPen = NULL;
        if (color == colors::Transparent)
            hPen = (HPEN)GetStockObject(NULL_PEN);
        else
            hPen = ::CreatePen(PS_SOLID, (int32_t)width, color & 0x00FFFFFF);

        std::shared_ptr<HPEN> penptr = std::make_shared<HPEN>(hPen);
        _pens[{color, width}] = penptr;
        return penptr;
    }

    std::shared_ptr<HBRUSH> GDIObjectCache::GetBrush(const core::color32 color)
    {
        auto iter = _brushs.find(color);
        if (iter != _brushs.end())
            return iter->second;

        HBRUSH hBrush = NULL;
        if (color == colors::Transparent)
            hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
        else
            hBrush = ::CreateSolidBrush(color & 0x00FFFFFF);

        std::shared_ptr<HBRUSH> brushptr = std::make_shared<HBRUSH>(hBrush);
        _brushs[color] = brushptr;
        return brushptr;
    }

    std::shared_ptr<HFONT> GDIObjectCache::GetFont(const graphics::text::font & font)
    {
        auto iter = _fonts.find(font);
        if (iter != _fonts.end())
            return iter->second;

        LOGFONTW logFont = {};
        FontToLOGFONT(*_hdc.get(), font, logFont);

        HFONT hFont = CreateFontIndirectW(&logFont);
        std::hash<graphics::text::font>{}(font);

        std::shared_ptr<HFONT> fontptr = std::make_shared<HFONT>(hFont);
        _fonts[font] = fontptr;
        return fontptr;
    }
}
