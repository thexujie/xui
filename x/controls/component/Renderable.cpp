#include "stdafx.h"
#include "Renderable.h"

namespace controls::component
{
    void Renderable::render(graphics::Graphics & graphics) const
    {
        graphics.drawRectangle(_rect, graphics::PathStyle().stoke(core::colors::Red));
    }
}
