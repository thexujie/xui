#pragma once
#include "ui/component/Renderable.h"
#include "graphics/Path.h"

namespace ui::renderables
{
    class Path : public component::Renderable
    {
    public:
        Path() {}
        Path(std::shared_ptr<graphics::Path> path) : _path(path) {}
        ~Path() = default;

        void setPath(std::shared_ptr<graphics::Path> path) { _path = path; invalid(); }
        void setClipPath(std::shared_ptr<graphics::Path> path) { _pathClip = path; invalid(); }
        void setPathStyle(const graphics::PathStyle & style) { if (_style != style) { _style = style; invalid(); }}
        graphics::PathStyle pathStyle() const { return _style; }

        void render(graphics::Graphics & graphics) const override;

    private:
        std::shared_ptr<graphics::Path> _path;
        std::shared_ptr<graphics::Path> _pathClip;
        graphics::PathStyle _style;
    };
}
