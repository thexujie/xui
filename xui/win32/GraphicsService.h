#pragma once
#include "graphics/GraphicsService.h"
#include "win32/windows.h"

namespace win32
{
    class GraphicsService : public graphics::IGraphicsService
    {
    public:
        std::shared_ptr<graphics::IGraphicsPixmap> CreatePixmap(core::math::si32_t size);
        std::shared_ptr<graphics::IGraphicsImage> CreateString(std::string str, graphics::text::font font, core::color32 color);
        std::shared_ptr<graphics::IGraphics> CreateGraphics(std::shared_ptr<core::Object> pixmap);

    private:
        std::shared_ptr<HDC> _hdc;
    };
}
