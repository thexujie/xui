#include "stdafx.h"
#include "Rectangle.h"

namespace ui::renderables
{
    void Rectangle::render(graphics::Graphics & graphics) const
    {
        Renderable::render(graphics);
        if (_rectangle.empty())
            return;

        graphics.save();
        graphics.setClipPath(_clipPath);
        graphics.drawRectangle(_rectangle, _style);
        graphics.restore();
    }
}
