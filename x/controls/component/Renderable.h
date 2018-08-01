#pragma once
#include "Component.h"
#include "graphics/Graphics.h"

namespace controls {
    class View;
}

namespace controls::component
{
    class Renderable : public Component
    {
    public:
        Renderable() : Component(ComponentType::Renderable) {}
        Renderable(const core::rc32f & rect) : Component(ComponentType::Renderable, rect) {}
        ~Renderable() = default;

        void enterView(std::shared_ptr<View> view) { _view = view; }
        void leaveView(std::shared_ptr<View> view) { _view.reset(); }
        std::shared_ptr<View> view() const { return _view.lock(); }

        virtual void render(graphics::Graphics & graphics) const;

    private:
        std::weak_ptr<View> _view;
    };
}
