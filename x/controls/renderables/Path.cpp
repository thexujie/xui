#include "stdafx.h"
#include "Path.h"

namespace controls::renderables
{
    void Path::render(graphics::Graphics & graphics) const
    {
        Renderable::render(graphics);
        graphics.drawPath(_path, _style);
    }
}