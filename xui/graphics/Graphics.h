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
        void PushOrign(pt32_t point);
        pt32_t GetOrign() const;
        void PopOrign();
        void PushClip(rc32_t rect);
        rc32_t GetClip() const;
        void PopClip();

        void Clear(color32 color);
        void DrawLine(pt32_t start, pt32_t end, color32 color, float32_t width);

        void DrawEllipse(rc32_t ellipse, color32 color, float32_t width);
        void FillEllipse(rc32_t ellipse, color32 color);

        void DrawRect(rc32_t rect, color32 color, float32_t width);
        void FillRect(rc32_t rect, color32 color);

        void DrawRoundRect(core::math::rc32_t rect, core::color32 color, float32_t width, float32_t radius);
        void FillRoundRect(core::math::rc32_t rect, core::color32 color, float32_t radius);

        void DrawString(const std::string & str, pt32_t point, const text::font & font, color32 color, int32_t flags);
        void DrawString(const std::string & str, rc32_t rect, const text::font & font, color32 color, int32_t flags);
        void DrawString(IGraphicsString & str, pt32_t point, const text::font & font, color32 color, int32_t flags);
        void DrawString(IGraphicsString & str, pt32_t point);

        void DrawImage(const Image & image, pt32_t point, int32_t flags);
        void DrawImage(const Image & image, rc32_t rect, int32_t flags);
        void DrawImage(const Image & image, pt32_t point, rc32_t region, int32_t flags);
        void DrawImage(const Image & image, rc32_t rect, rc32_t region, int32_t flags);

        void DrawImage(const Image & image, rc32_t rect);
        void DrawImage(const Image & image, rc32_t rect, rc32_t region);

    public:
        void FillPath(graphics::raster::path & path, core::color32 color);
        text::fontmetrics GetFontMetrics(text::font font);
        si32_t MeasureString(std::string str, text::font font);

    private:
        std::shared_ptr<IGraphics> _graphics;
    };
}
