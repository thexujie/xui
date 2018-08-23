#include "stdafx.h"
#include "Renderable.h"
#include "ui/Control.h"

namespace ui::component
{
    void Renderable::render(graphics::Graphics & graphics) const
    {
        //graphics.drawRectangle(_rect, graphics::PathStyle().stoke(core::colors::Red));
    }

    void Renderable::invalid()
    {
        if (auto v = _control.lock())
            v->invalid_rect(_rect);
    }

    void Renderable::onRectChanged(const core::rc32f & from, const core::rc32f & to)
    {
        if (auto v = _control.lock())
        {
            v->invalid_rect(from);
            v->invalid_rect(to);
        }
    }
}
