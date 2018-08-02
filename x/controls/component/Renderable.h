#pragma once
#include "Component.h"
#include "graphics/Graphics.h"

namespace controls::component
{
    class Renderable : public Component
    {
    public:
        Renderable(std::shared_ptr<View> view) : Component(ComponentType::Renderable, view) {}
        Renderable(std::shared_ptr<View> view, const core::rc32f & rect) : Component(ComponentType::Renderable, view) {}
        ~Renderable() = default;

        virtual void render(graphics::Graphics & graphics) const;
    };
}
