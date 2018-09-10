#include "stdafx.h"
#include "Line.h"

namespace ui::renderables
{
    void Line::render(drawing::Graphics & graphics) const
    {
        Renderable::render(graphics);
        graphics.save();
        graphics.setClipPath(*_clipPath);
        graphics.drawLine(_point0, _point1, _style);
        graphics.restore();
    }
}
