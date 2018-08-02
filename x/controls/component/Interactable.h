#pragma once
#include "Component.h"

namespace controls::component
{
    class mosue_state
    {
    public:
        const core::pt32f & pos() const { return _pos; }

    private:
        core::pt32f _pos;
    };

    class Interactable : public component::Component
    {
    public:
        Interactable(std::shared_ptr<View> view) : Component(ComponentType::Renderable, view) {}
        virtual ~Interactable() {}
    };

    class MouseArea : public component::Interactable
    {
    public:
        MouseArea(std::shared_ptr<View> view) : Interactable(view) {}
        ~MouseArea() = default;

        virtual void onMouseEnter(mosue_state & state) {}
        virtual void onMouseMove(mosue_state & state) {}
        virtual void onMouseLeave(mosue_state & state) {}

        virtual void onMouseDown(mosue_state & state) {}
        virtual void onMouseUp(mosue_state & state) {}
        virtual void onMouseClick(mosue_state & state) {}
        virtual void onMouseDBClick(mosue_state & state) {}
    };
}
