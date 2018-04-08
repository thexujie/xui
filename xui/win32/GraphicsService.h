#pragma once
#include "graphics/GraphicsService.h"
#include "win32/windows.h"

namespace win32
{
    class GraphicsService : public graphics::IGraphicsService
    {
    public:
        std::shared_ptr<graphics::IPixmap> CreatePixmap(core::math::si32_t size);
        std::shared_ptr<graphics::IGraphics> CreateGraphics(std::shared_ptr<core::Object> pixmap);
        std::shared_ptr<graphics::IImage> CreateImage(std::string path);

    private:
        std::shared_ptr<HDC> _hdc;
    };
}
