#include "stdafx.h"
#include "Rectangle.h"

namespace controls::renderables
{
    void Rectangle::render(graphics::Graphics & graphics) const
    {
        if (_rect.empty())
            return;

        graphics.drawRectangle(_rect, _style);
    }
}
