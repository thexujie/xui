#pragma once
#include "graphics/font.h"
#include "win32/win32.h"
#include <usp10.h>

namespace win32::uniscribe
{
    struct view_font_t
    {
        SCRIPT_CACHE * cache;
        HFONT hfont;
        graphics::font font;
        graphics::fontmetrics metrics;
    };

    class ScriptService : public core::object
    {
    public:
        ScriptService();
        ~ScriptService();

        HDC GetHDC() const { return _hdc; }
        int32_t MapFont(const graphics::font & font);
        const view_font_t & GetFont(int32_t ifont) const;

        graphics::font GetFontFallBack(const graphics::font & font, int32_t iLanguage, const wchar_t * text, int32_t length);

    private:
        HDC _hdc = NULL;
        std::vector<view_font_t> _fonts;
    };
}
