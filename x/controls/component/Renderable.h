#pragma once
#include "Component.h"
#include "graphics/Graphics.h"

namespace controls::component
{
    class Renderable : public Component
    {
    public:
        Renderable() : Component(ComponentType::Renderable) {}
        Renderable(const core::rc32f & rect) : Component(ComponentType::Renderable, rect) {}
        ~Renderable() = default;

        virtual void render(graphics::Graphics & graphics) const = 0;
    };
}
