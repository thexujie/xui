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
        void PushOrign(core::math::pt32_t point);
        core::math::pt32_t GetOrign() const;
        void PopOrign();
        void PushClip(core::math::rc32_t rect);
        core::math::rc32_t GetClip() const;
        void PopClip();

        void Clear(color32 color);
        void DrawLine(pt32_t start, pt32_t end, color32 color, float32_t width);

        void DrawRect(rc32_t rect, color32 color, float32_t width);
        void FillRect(rc32_t rect, color32 color);

        void DrawString(std::string str, pt32_t point, font font, core::color32 color, int32_t flags);
        void DrawString(std::string str, rc32_t rect, font font, core::color32 color, int32_t flags);

        void DrawImage(graphics::Image & image, pt32_t point, int32_t flags);
        void DrawImage(graphics::Image & image, rc32_t rect, int32_t flags);
        void DrawImage(graphics::Image & image, pt32_t point, core::math::rc32_t region, int32_t flags);
        void DrawImage(graphics::Image & image, rc32_t rect, core::math::rc32_t region, int32_t flags);

        void DrawImage(graphics::Image & image, rc32_t rect);
        void DrawImage(graphics::Image & image, core::math::rc32_t rect, core::math::rc32_t region);

    public:
        graphics::fontmetrics GetFontMetrics(graphics::font font);
        core::math::si32_t MeasureString(std::string str, graphics::font font);

    private:
        std::shared_ptr<IGraphics> _graphics;
    };
}
