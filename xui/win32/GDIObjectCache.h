#pragma once
#include "graphics/font.h"
#include "win32/windows.h"

namespace win32
{
    class GDIObjectCache
    {
    public:
        GDIObjectCache(std::shared_ptr<HDC> hdc);
        ~GDIObjectCache() = default;

        std::shared_ptr<HPEN> GetPen(const core::color32 color, float32_t width);
        std::shared_ptr<HBRUSH> GetBrush(const core::color32 color);
        std::shared_ptr<HFONT> GetFont(const graphics::font & font);

    private:
        std::shared_ptr<HDC> _hdc = nullptr;
        std::map<graphics::font, std::shared_ptr<HFONT>> _fonts;
        std::map<std::tuple<const core::color32, float32_t>, std::shared_ptr<HPEN>> _pens;
        std::map<const core::color32, std::shared_ptr<HBRUSH>> _brushs;
    };
}
