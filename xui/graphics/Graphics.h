#pragma once

#include "GraphicsService.h"
#include "Pixmap.h"
#include "Image.h"
#include "core/math/rect.h"

namespace graphics
{
    class Graphics
    {
    public:
        Graphics() = default;
        ~Graphics() = default;

        Graphics(si32_t size);
        Graphics(std::shared_ptr<IGraphics> graphics);
        Graphics(std::shared_ptr<Pixmap> pixmap);

    public:
        void Clear(color32 color);
        void DrawLine(pt32_t start, pt32_t end, color32 color, float32_t width);

        void DrawRect(rc32_t rect, color32 color, float32_t width);
        void FillRect(rc32_t rect, color32 color);

        void DrawString(std::string str, core::color32 color, font font, pt32_t point, int32_t flags);
        void DrawString(std::string str, core::color32 color, font font, rc32_t rect, int32_t flags);
        void DrawImage(graphics::Image & image, pt32_t point, int32_t flags);

    private:
        core::error_e _state = core::error_ok;
        std::shared_ptr<IGraphics> _graphics;
    };
}
