#pragma once
#include "controls/component/Renderable.h"

namespace controls::renderables
{
    class Rectangle : public component::Renderable
    {
    public:
        Rectangle() = default;
        Rectangle(const core::rc32f & rect) : Renderable(rect) {}
        ~Rectangle() = default;

        void setPathStyle(const graphics::PathStyle & style) { _style = style; }
        graphics::PathStyle pathStyle() const { return _style; }

        void render(graphics::Graphics & graphics) const;

    private:
        graphics::PathStyle _style;
    };
}
