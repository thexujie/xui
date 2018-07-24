#pragma once
#include "controls/component/Renderable.h"

namespace controls::renderables
{
    class Rectangle : public component::Renderable
    {
    public:
        Rectangle() = default;
        Rectangle(const core::rc32f & rect) : Renderable(rect.position), _size(rect.size) {}
        ~Rectangle() = default;

        void setSize(const core::si32f & size) { _size = size; }
        core::si32f size() const { return _size; }
        void setPathStyle(const graphics::PathStyle & style) { _style = style; }
        graphics::PathStyle pathStyle() const { return _style; }

        void render(graphics::Graphics & graphics) const;

    private:
        core::si32f _size;
        graphics::PathStyle _style;
    };
}
