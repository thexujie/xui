#pragma once

#include "GraphicsService.h"
#include "Bitmap.h"
#include "Image.h"

class SkCanvas;
class SkTextBlobBuilder;

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

    struct StringFormat
    {
        StringFormat(){}
        StringFormat(const font & font) : _font(font) {}

        StringFormat & font(const font & font)
        {
            _font = font;
            return *this;
        }

        StringFormat & color(core::color32 color)
        {
            _color = color;
            return *this;
        }

        StringFormat & lcd(bool lcd)
        {
            _lcd = lcd;
            return *this;
        }

        StringFormat & weight(int32_t weight)
        {
            _font.weight = (graphics::font_weight)weight;
            return *this;
        }

        StringFormat & align(core::align align)
        {
            _align = align;
            return *this;
        }

        StringFormat & hint(bool hint)
        {
            _hint = hint;
            return *this;
        }

        graphics::font _font;
        core::color32 _color = core::colors::Black;
        core::align _align = core::align::leftTop;
        bool _lcd = true;
        bool _hint = true;
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
        void drawLine(core::pt32f start, core::pt32f end, const Style & style);

        void drawEllipse(core::rc32f ellipse, const Style & style);
        void drawRectangle(core::rc32f rect, const Style & style);
        void drawRoundRect(core::rc32f rect, float32_t rx, float32_t ry, const Style & style);

        void drawString(const std::string & str, core::pt32f point, const StringFormat & format);
        void drawString(const std::string & str, core::rc32f rect, const StringFormat & format);
        void drawString(const IGraphicsString & str, core::pt32i point);

        void drawImage(const Image & image, core::pt32f point, int32_t flags);
        void drawImage(const Image & image, core::rc32f rect, int32_t flags);
        void drawImage(const Image & image, core::pt32f point, core::rc32i region, int32_t flags);
        void drawImage(const Image & image, core::pt32f point, core::rc32f region, int32_t flags);
        void drawImage(const Image & image, core::rc32f rect, core::rc32i region, int32_t flags);
        void drawImage(const Image & image, core::rc32f rect, core::rc32f region, int32_t flags);

        void drawImage(const Image & image, core::rc32f rect);
        void drawImage(const Image & image, core::rc32f rect, core::rc32i region);
        void drawImage(const Image & image, core::rc32f rect, core::rc32f region);

    public:
        void FillPath(graphics::raster::path & path, core::color32 color);
        fontmetrics GetFontMetrics(font font);
        core::si32i MeasureString(std::string str, font font);

    public:
        std::shared_ptr<SkCanvas> native_share() { return _native; }
        SkCanvas & native() { return *_native; }
        const SkCanvas & native() const { return *_native; }

    private:
        std::shared_ptr<Bitmap> _pixmap;
        std::shared_ptr<SkCanvas> _native;
        std::shared_ptr<SkTextBlobBuilder> _blobBuilder;
    };
}
