#pragma once
#include "ui/component/Renderable.h"
#include "drawing/Path.h"

namespace ui::renderables
{
    class Shape : public component::Renderable
    {
    public:
        Shape(std::shared_ptr<ui::Control> control) : component::Renderable(control) {}
        ~Shape() = default;

        void setClipPath(std::shared_ptr<drawing::Path> path) { _clipPath = path; invalid(); }
        void setPathStyle(const drawing::PathStyle & style) { if (_style != style) { _style = style; invalid(); }}
        drawing::PathStyle ShapeStyle() const { return _style; }

    protected:
        std::shared_ptr<drawing::Path> _clipPath;
        drawing::PathStyle _style;
    };
}
