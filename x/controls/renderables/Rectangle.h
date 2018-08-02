#pragma once
#include "Shape.h"

namespace controls::renderables
{
    class Rectangle : public Shape
    {
    public:
        Rectangle() {}
        Rectangle(const core::rc32f & rect) :_rectangle(rect){}
        ~Rectangle() = default;

        void setRectangle(const core::rc32f & rect) { _rectangle = rect; }

        void render(graphics::Graphics & graphics) const;

    private:
        core::rc32f _rectangle;
    };
}
