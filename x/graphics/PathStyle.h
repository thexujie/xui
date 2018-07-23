#pragma once

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
            _fillColor = color;
            return *this;
        }

        PathStyle & stoke(core::color32 color)
        {
            _stokeColor = color;
            return *this;
        }

        PathStyle & aa(bool aa)
        {
            _aa = aa;
            return *this;
        }

        PathMode _mode = PathMode::Stroke;
        float32_t _width = 1.0f;
        core::color32 _fillColor = core::colors::Transparent;
        core::color32 _stokeColor = core::colors::Black;
        bool _aa = true;
    };


}

