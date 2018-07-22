#pragma once

#include "graphics/GraphicsService.h"

namespace graphics::skia
{
    class Bitmap;

    class SkiaGraphics : public IGraphics
    {
    public:
        SkiaGraphics();
        ~SkiaGraphics();


        void PushOrign(core::pt32i point) override;
        core::pt32i GetOrign() const override;
        void PopOrign() override;
        void PushClip(core::rc32i rect) override;
        core::rc32i GetClip() const override;
        void PopClip() override;
        void Clear(core::color32 color) override;
        void DrawString(std::string str, core::pt32i point, graphics::text::font font, core::color32 color) override;
        void DrawImage(const graphics::IGraphicsImage & image, core::pt32i point) override;
        void DrawImage(const graphics::IGraphicsImage & image, core::pt32i point, core::rc32i region) override;
        void DrawImage(const graphics::IGraphicsImage & image, core::rc32i rect) override;
        void DrawImage(const graphics::IGraphicsImage & image, core::rc32i rect, core::rc32i region) override;
        void DrawString(const std::string & str, core::pt32i point, const text::font & font, core::color32 color,
            int32_t flags) override;
        void DrawString(const std::string & str, core::rc32i rect, const text::font & font, core::color32 color,
            int32_t flags) override;
        void DrawString(const graphics::IGraphicsString & str, core::pt32i point) override;
        void FillPath(graphics::raster::path & path, core::color32 color) override;
        graphics::text::fontmetrics GetFontMetrics(graphics::text::font font) override;
        core::si32i MeasureString(std::string str, graphics::text::font font) override;

    private:
        std::stack<core::pt32i> _origns;
        std::stack<core::rc32i> _clips;
    };
}
