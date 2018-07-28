#pragma once
#include "controls/component/Renderable.h"
#include "graphics/Path.h"

namespace controls::renderables
{
    class Path : public component::Renderable
    {
    public:
        Path() = default;
        Path(std::shared_ptr<graphics::Path> path) :_path(path) {}
        ~Path() = default;

        void setPath(std::shared_ptr<graphics::Path> path) { _path = path; }
        void setPathStyle(const graphics::PathStyle & style) { _style = style; }
        graphics::PathStyle pathStyle() const { return _style; }

        void render(graphics::Graphics & graphics) const;

    private:
        std::shared_ptr<graphics::Path> _path;
        graphics::PathStyle _style;
    };
}
