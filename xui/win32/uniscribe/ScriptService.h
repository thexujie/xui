#pragma once
#include "graphics/text/font.h"
#include "win32/win32.h"
#include <usp10.h>

namespace win32::uniscribe
{
    struct view_font_t
    {
        SCRIPT_CACHE * cache;
        HFONT hfont;
        graphics::text::font font;
        graphics::text::fontmetrics metrics;
    };

    class ScriptService : public core::Object
    {
    public:
        ScriptService();
        ~ScriptService();

        HDC GetHDC() const { return _hdc; }
        int32_t MapFont(const graphics::text::font & font);
        const view_font_t & GetFont(int32_t ifont) const;

        graphics::text::font GetFontFallBack(const graphics::text::font & font, int32_t iLanguage, const wchar_t * text, int32_t length);

    private:
        HDC _hdc = NULL;
        std::vector<view_font_t> _fonts;
    };
}
