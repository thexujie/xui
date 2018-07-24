#include "stdafx.h"
#include "Rectangle.h"

namespace controls::renderables
{
    void Rectangle::render(graphics::Graphics & graphics) const
    {
        if (_size.empty())
            return;

        if(!_size.empty())
            graphics.drawRectangle({ pos() - _size * 0.5f, _size }, graphics::PathStyle(graphics::PathMode::Stroke));
    }
}
