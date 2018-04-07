#pragma once
#include "graphics/font.h"
#include "win32.h"

namespace win32
{
    class GDIObjectCache
    {
    public:
        GDIObjectCache(handle_t hdc);
        ~GDIObjectCache() = default;

        handle_t GetPen(const core::color32 color, float32_t width);
        handle_t GetBrush(const core::color32 color);
        handle_t GetFont(const graphics::font & font);

    private:
        handle_t _hdc = nullptr;
        std::map<graphics::font, handle_t> _fonts;
        std::map<std::tuple<const core::color32, float32_t>, handle_t> _pens;
        std::map<const core::color32, handle_t> _brushs;
    };
}
