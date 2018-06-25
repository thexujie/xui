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

        Graphics(core::math::si32i size);
        Graphics(std::shared_ptr<IGraphics> graphics);
        Graphics(std::shared_ptr<Pixmap> pixmap);

    public:
        void PushOrign(core::math::pt32i point);
        core::math::pt32i GetOrign() const;
        void PopOrign();
        void PushClip(core::math::rc32i rect);
        core::math::rc32i GetClip() const;
        void PopClip();

        void Clear(core::color32 color);
        void DrawLine(core::math::pt32i start, core::math::pt32i end, core::color32 color, float32_t width);

        void DrawEllipse(core::math::rc32i ellipse, core::color32 color, float32_t width);
        void FillEllipse(core::math::rc32i ellipse, core::color32 color);

        void DrawRect(core::math::rc32i rect, core::color32 color, float32_t width);
        void FillRect(core::math::rc32i rect, core::color32 color);

        void DrawRoundRect(core::math::rc32i rect, core::color32 color, float32_t width, float32_t radius);
        void FillRoundRect(core::math::rc32i rect, core::color32 color, float32_t radius);

        void DrawString(const std::string & str, core::math::pt32i point, const text::font & font, core::color32 color, int32_t flags);
        void DrawString(const std::string & str, core::math::rc32i rect, const text::font & font, core::color32 color, int32_t flags);
        void DrawString(IGraphicsString & str, core::math::pt32i point, const text::font & font, core::color32 color, int32_t flags);
        void DrawString(IGraphicsString & str, core::math::pt32i point);

        void DrawImage(const Image & image, core::math::pt32i point, int32_t flags);
        void DrawImage(const Image & image, core::math::rc32i rect, int32_t flags);
        void DrawImage(const Image & image, core::math::pt32i point, core::math::rc32i region, int32_t flags);
        void DrawImage(const Image & image, core::math::rc32i rect, core::math::rc32i region, int32_t flags);

        void DrawImage(const Image & image, core::math::rc32i rect);
        void DrawImage(const Image & image, core::math::rc32i rect, core::math::rc32i region);

    public:
        void FillPath(graphics::raster::path & path, core::color32 color);
        text::fontmetrics GetFontMetrics(text::font font);
        core::math::si32i MeasureString(std::string str, text::font font);

    private:
        std::shared_ptr<IGraphics> _graphics;
    };
}
