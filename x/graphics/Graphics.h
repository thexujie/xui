#pragma once

#include "GraphicsService.h"
#include "Bitmap.h"
#include "Image.h"

class SkCanvas;

namespace graphics
{
    enum class DrawMode
    {
        Stroke = 0,
        Fill,
        StrokeFill,
    };

    struct Style
    {
        Style()
        {
            
        }

        Style(DrawMode mode):_mode(mode)
        {

        }

        Style & mode(DrawMode mode)
        {
            _mode = mode;
            return *this;
        }

        Style & width(float32_t width)
        {
            _width = width;
            return *this;
        }

        Style & fill(core::color32 color)
        {
            _fillColor = color;
            return *this;
        }

        Style & stoke(core::color32 color)
        {
            _stokeColor = color;
            return *this;
        }

        Style & aa(bool aa)
        {
            _aa = aa;
            return *this;
        }

        DrawMode _mode = DrawMode::Stroke;
        float32_t _width = 1.0f;
        core::color32 _fillColor = core::colors::Transparent;
        core::color32 _stokeColor = core::colors::Black;
        bool _aa = true;
    };

    class Graphics
    {
    public:
        Graphics() = default;
        ~Graphics() = default;

        //Graphics(core::si32i size);
        Graphics(std::shared_ptr<Bitmap> pixmap);

    public:
        void Clear(core::color32 color);
        void drawLine(core::pt32f start, core::pt32f end, core::color32 color, float32_t width);

        void drawEllipse(core::rc32f ellipse, const Style & style);
        void drawRectangle(core::rc32f rect, const Style & style);
        void drawRoundRect(core::rc32f rect, float32_t rx, float32_t ry, const Style & style);

        void DrawString(const std::string & str, core::pt32i point, const text::font & font, core::color32 color, int32_t flags);
        void DrawString(const std::string & str, core::rc32f rect, const text::font & font, core::color32 color, int32_t flags);
        void DrawString(const IGraphicsString & str, core::pt32i point);

        void DrawImage(const Image & image, core::pt32i point, int32_t flags);
        void DrawImage(const Image & image, core::rc32f rect, int32_t flags);
        void DrawImage(const Image & image, core::pt32i point, core::rc32f region, int32_t flags);
        void DrawImage(const Image & image, core::rc32f rect, core::rc32f region, int32_t flags);

        void DrawImage(const Image & image, core::rc32f rect);
        void DrawImage(const Image & image, core::rc32f rect, core::rc32f region);

    public:
        void FillPath(graphics::raster::path & path, core::color32 color);
        text::fontmetrics GetFontMetrics(text::font font);
        core::si32i MeasureString(std::string str, text::font font);

    private:
        std::shared_ptr<Bitmap> _pixmap;
        std::shared_ptr<SkCanvas> _native;
    };
}
