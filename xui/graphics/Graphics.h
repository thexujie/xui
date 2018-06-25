#pragma once

#include "GraphicsService.h"
#include "Pixmap.h"
#include "Image.h"

namespace graphics
{
    class Graphics
    {
    public:
        Graphics() = default;
        ~Graphics() = default;

        Graphics(core::si32i size);
        Graphics(std::shared_ptr<IGraphics> graphics);
        Graphics(std::shared_ptr<Pixmap> pixmap);

    public:
        void PushOrign(core::pt32i point);
        core::pt32i GetOrign() const;
        void PopOrign();
        void PushClip(core::rc32i rect);
        core::rc32i GetClip() const;
        void PopClip();

        void Clear(core::color32 color);
        void DrawLine(core::pt32i start, core::pt32i end, core::color32 color, float32_t width);

        void DrawEllipse(core::rc32i ellipse, core::color32 color, float32_t width);
        void FillEllipse(core::rc32i ellipse, core::color32 color);

        void DrawRect(core::rc32i rect, core::color32 color, float32_t width);
        void FillRect(core::rc32i rect, core::color32 color);

        void DrawRoundRect(core::rc32i rect, core::color32 color, float32_t width, float32_t radius);
        void FillRoundRect(core::rc32i rect, core::color32 color, float32_t radius);

        void DrawString(const std::string & str, core::pt32i point, const text::font & font, core::color32 color, int32_t flags);
        void DrawString(const std::string & str, core::rc32i rect, const text::font & font, core::color32 color, int32_t flags);
        void DrawString(IGraphicsString & str, core::pt32i point, const text::font & font, core::color32 color, int32_t flags);
        void DrawString(IGraphicsString & str, core::pt32i point);

        void DrawImage(const Image & image, core::pt32i point, int32_t flags);
        void DrawImage(const Image & image, core::rc32i rect, int32_t flags);
        void DrawImage(const Image & image, core::pt32i point, core::rc32i region, int32_t flags);
        void DrawImage(const Image & image, core::rc32i rect, core::rc32i region, int32_t flags);

        void DrawImage(const Image & image, core::rc32i rect);
        void DrawImage(const Image & image, core::rc32i rect, core::rc32i region);

    public:
        void FillPath(graphics::raster::path & path, core::color32 color);
        text::fontmetrics GetFontMetrics(text::font font);
        core::si32i MeasureString(std::string str, text::font font);

    private:
        std::shared_ptr<IGraphics> _graphics;
    };
}
