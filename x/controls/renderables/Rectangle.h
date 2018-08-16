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

        void setRectangle(const core::rc32f & rect) { if (_rectangle != rect) { _rectangle = rect; invalid(); }}

        void render(graphics::Graphics & graphics) const override;

    private:
        core::rc32f _rectangle;
    };
}
