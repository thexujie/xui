#pragma once
#include "ui/component/Renderable.h"
#include "graphics/Path.h"

namespace ui::renderables
{
    class Shape : public component::Renderable
    {
    public:
        Shape(std::shared_ptr<ui::Control> control) : component::Renderable(control) {}
        ~Shape() = default;

        void setClipPath(std::shared_ptr<graphics::Path> path) { _clipPath = path; invalid(); }
        void setPathStyle(const graphics::PathStyle & style) { if (_style != style) { _style = style; invalid(); }}
        graphics::PathStyle ShapeStyle() const { return _style; }

    protected:
        std::shared_ptr<graphics::Path> _clipPath;
        graphics::PathStyle _style;
    };
}
