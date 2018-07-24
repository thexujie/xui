#pragma once
#include "Component.h"
#include "graphics/Graphics.h"

namespace controls::component
{
    class Renderable : public Component
    {
    public:
        Renderable() : Component(ComponentType::Renderable) {}
        Renderable(const core::pt32f & pos) : Component(ComponentType::Renderable, pos) {}
        ~Renderable() = default;

        virtual void render(graphics::Graphics & graphics) const;
    };
}
