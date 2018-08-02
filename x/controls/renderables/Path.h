#pragma once
#include "controls/component/Renderable.h"
#include "graphics/Path.h"

namespace controls::renderables
{
    class Path : public component::Renderable
    {
    public:
        Path(std::shared_ptr<component::View> view) : Renderable(view) {}
        Path(std::shared_ptr<component::View> view, std::shared_ptr<graphics::Path> path) : Renderable(view), _path(path) {}
        ~Path() = default;

        void setPath(std::shared_ptr<graphics::Path> path) { _path = path; }
        void setClipPath(std::shared_ptr<graphics::Path> path) { _pathClip = path; }
        void setPathStyle(const graphics::PathStyle & style) { _style = style; }
        graphics::PathStyle pathStyle() const { return _style; }

        void render(graphics::Graphics & graphics) const;

    private:
        std::shared_ptr<graphics::Path> _path;
        std::shared_ptr<graphics::Path> _pathClip;
        graphics::PathStyle _style;
    };
}
