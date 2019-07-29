#pragma once
#include "drawing/font.h"
#include "win32/win32.h"
#include <usp10.h>

namespace win32::uniscribe
{
    struct view_font_t
    {
        SCRIPT_CACHE * cache;
        HFONT hfont;
        drawing::font font;
        drawing::fontmetrics metrics;
    };

    class ScriptService : public core::object
    {
    public:
        ScriptService();
        ~ScriptService();

        HDC GetHDC() const { return _hdc; }
        int32_t MapFont(const drawing::font & font);
        const view_font_t & GetFont(int32_t ifont) const;

        drawing::font GetFontFallBack(const drawing::font & font, int32_t iLanguage, const wchar_t * text, int32_t length);

    private:
        HDC _hdc = NULL;
        std::vector<view_font_t> _fonts;
    };
}
