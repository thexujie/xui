#pragma once

class SkPaint;

namespace drawing
{
    enum class PathMode
    {
        Stroke = 0,
        Fill,
        StrokeFill,
    };

    enum class stroke_style
    {
        none = 0,
        hidden,
        dotted,
        dashed,
        solid,
        double_,
        groove,
        ridge,
        inset,
        outset,
        inherit,
    };

    struct stroke_style_name
    {
        stroke_style style;
        const char * name = nullptr;
    };

    const stroke_style_name stroke_style_names[] =
    {
        { stroke_style::none, "none" },
        { stroke_style::hidden, "hidden" },
        { stroke_style::dotted, "dotted" },
        { stroke_style::dashed, "dashed" },
        { stroke_style::solid, "solid" },
        { stroke_style::double_, "double" },
        { stroke_style::groove, "groove" },
        { stroke_style::ridge, "ridge" },
        { stroke_style::inset, "inset" },
        { stroke_style::outset, "outset" },
        { stroke_style::inherit, "inherit" },
    };

    stroke_style parseStrokeStyle(const std::string & str);
    core::vec2<stroke_style> parseStrokeStyle2D(const std::string & str);
    core::vec4<stroke_style> parseStrokeStyle4D(const std::string & str);

    struct PathStyle
    {
        PathStyle()
        {

        }

        PathStyle(PathMode mode) :_mode(mode)
        {

        }
        PathStyle & mode(PathMode mode)
        {
            _mode = mode;
            return *this;
        }

        PathStyle & width(float32_t width)
        {
            _width = width;
            return *this;
        }

        PathStyle & fill(core::color32 color)
        {
            _mode = PathMode::Fill;
            _color = color;
            return *this;
        }

        PathStyle & stoke(core::color32 color, stroke_style stroke_style = stroke_style::solid)
        {
            _mode = PathMode::Stroke;
            _stroke_style = stroke_style;
            _color = color;
            return *this;
        }

        PathStyle & aa(bool aa)
        {
            _aa = aa;
            return *this;
        }

        void apply(SkPaint & paint) const;

        bool operator == (const PathStyle & another) const
        {
            return _mode == another._mode &&
                _stroke_style == another._stroke_style &&
                std::abs(_width - another._width) < std::numeric_limits<float32_t>::epsilon() &&
                _color == another._color &&
                _aa == another._aa;
        }

        bool operator != (const PathStyle & another) const { return !operator==(another); }

        PathMode _mode = PathMode::Stroke;
        stroke_style _stroke_style = stroke_style::none;
        float32_t _width = 1.0f;
        core::color32 _color = core::colors::Transparent;
        bool _aa = true;
    };


}

