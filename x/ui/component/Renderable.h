#pragma once
#include "Component.h"
#include "graphics/Graphics.h"

namespace ui::component
{
    class Renderable : public Component
    {
    public:
        Renderable(std::shared_ptr<ui::Control> control) : Component(ComponentType::Renderable, control) {}
        ~Renderable() = default;

        void invalid();

        virtual void render(graphics::Graphics & graphics) const;
        virtual void onRectChanged(const core::rc32f & from, const core::rc32f & to);
    };
}
