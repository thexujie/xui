#pragma once
#include "Shape.h"

namespace ui::renderables
{
    class Rectangle : public Shape
    {
    public:
        Rectangle(std::shared_ptr<ui::Control> control) : Shape(control)  {}
        Rectangle(std::shared_ptr<ui::Control> control, const core::rc32f & rect) :Shape(control), _rectangle(rect){}
        ~Rectangle() = default;

        void setRectangle(const core::rc32f & rect) { if (_rectangle != rect) { _rectangle = rect; invalid(); }}

        void render(drawing::Graphics & graphics) const override;

    private:
        core::rc32f _rectangle;
    };
}
