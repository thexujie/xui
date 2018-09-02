#pragma once
#include "ui/component/Renderable.h"
#include "drawing/Path.h"

namespace ui::renderables
{
    class Path : public component::Renderable
    {
    public:
        Path(std::shared_ptr<ui::Control> control) : component::Renderable(control) {}
        Path(std::shared_ptr<ui::Control> control, std::shared_ptr<drawing::Path> path) : component::Renderable(control), _path(path) {}
        ~Path() = default;

        void setPath(std::shared_ptr<drawing::Path> path) { _path = path; invalid(); }
        void setClipPath(std::shared_ptr<drawing::Path> path) { _pathClip = path; invalid(); }
        void setPathStyle(const drawing::PathStyle & style) { if (_style != style) { _style = style; invalid(); }}
        drawing::PathStyle pathStyle() const { return _style; }

        void render(drawing::Graphics & graphics) const override;

    private:
        std::shared_ptr<drawing::Path> _path;
        std::shared_ptr<drawing::Path> _pathClip;
        drawing::PathStyle _style;
    };
}
