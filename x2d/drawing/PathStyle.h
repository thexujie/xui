#pragma once

#include "drawing.h"

class SkPaint;

namespace drawing
{
    struct stroke_style_name
    {
        stroke_style style;
        const char8_t * name = nullptr;
    };

    const stroke_style_name stroke_style_names[] =
    {
        { stroke_style::none, u8"none" },
        { stroke_style::hidden, u8"hidden" },
        { stroke_style::dotted, u8"dotted" },
        { stroke_style::dashed, u8"dashed" },
        { stroke_style::solid, u8"solid" },
        { stroke_style::double_, u8"double" },
        { stroke_style::groove, u8"groove" },
        { stroke_style::ridge, u8"ridge" },
        { stroke_style::inset, u8"inset" },
        { stroke_style::outset, u8"outset" },
        { stroke_style::inherit, u8"inherit" },
    };

    stroke_style parseStrokeStyle(const std::u8string & str);
    core::vec2<stroke_style> parseStrokeStyle2D(const std::u8string & str);
    core::vec4<stroke_style> parseStrokeStyle4D(const std::u8string & str);

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

		PathStyle & blend(blend_mode bmode)
		{
			_blend_mode = bmode;
			return *this;
		}

        PathStyle & width(float32_t width)
        {
            _width = width;
            return *this;
        }

        PathStyle & fill(core::color color)
        {
            _mode = PathMode::Fill;
            _color = color;
            return *this;
        }

        PathStyle & stoke(core::color color, float32_t width = 1.0f, stroke_style stroke_style = stroke_style::solid)
        {
            _mode = PathMode::Stroke;
            _stroke_style = stroke_style;
            _color = color;
            _width = width;
            return *this;
        }

        PathStyle & aa(bool aa)
        {
            _aa = aa;
            return *this;
        }

        PathStyle & miter(float32_t miter)
        {
            _miter = miter;
            return *this;
        }

        PathStyle & join(join_style join)
        {
            _join_style = join;
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
		blend_mode _blend_mode = blend_mode::src_over;
        stroke_style _stroke_style = stroke_style::none;
        join_style _join_style = join_style::miter;
        float32_t _width = 1.0f;
        float32_t _miter = std::nanf("0");
        core::color _color = core::colors::Transparent;
        bool _aa = true;
    };


}

