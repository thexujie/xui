#pragma once

class SkPaint;

namespace graphics
{
    enum class PathMode
    {
        Stroke = 0,
        Fill,
        StrokeFill,
    };

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

        PathStyle & stoke(core::color32 color)
        {
            _mode = PathMode::Stroke;
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
                _width == another._width &&
                _color == another._color &&
                _aa == another._aa;
        }

        bool operator != (const PathStyle & another) const { return !operator==(another); }

        PathMode _mode = PathMode::Stroke;
        float32_t _width = 1.0f;
        core::color32 _color = core::colors::Transparent;
        bool _aa = true;
    };


}

