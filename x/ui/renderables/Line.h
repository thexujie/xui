#pragma once
#include "ui/component/Renderable.h"
#include "Shape.h"

namespace ui::renderables
{
    class Line : public Shape
    {
    public:
        Line(std::shared_ptr<ui::Control> control) : Shape(control) {}
        Line(std::shared_ptr<ui::Control> control, const core::pt32f & point0, const core::pt32f & point1) :Shape(control), _point0(point0), _point1(point1) {}
        ~Line() = default;

        void setPoints(const core::pt32f &  point0, const core::pt32f &  point1) { _point0 = point0; _point1 = point1; }
        void render(drawing::Graphics & graphics) const override;

    private:
        core::pt32f _point0;
        core::pt32f _point1;
    };
}
