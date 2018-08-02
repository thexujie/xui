#pragma once
#include "controls/component/Renderable.h"
#include "graphics/Path.h"

namespace controls::renderables
{
    class Shape : public component::Renderable
    {
    public:
        Shape(std::shared_ptr<component::View> view) : Renderable(view) {}
        ~Shape() = default;

        void setClipPath(std::shared_ptr<graphics::Path> path) { _clipPath = path; }
        void setPathStyle(const graphics::PathStyle & style) { _style = style; }
        graphics::PathStyle ShapeStyle() const { return _style; }

    protected:
        std::shared_ptr<graphics::Path> _clipPath;
        graphics::PathStyle _style;
    };
}
