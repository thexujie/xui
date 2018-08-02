#pragma once
#include "Component.h"

namespace controls::component
{
    class mosue_state
    {
    public:
        mosue_state(const core::pt32f pos) : _pos(pos) {}

        const core::pt32f & pos() const { return _pos; }

    private:
        core::pt32f _pos;
    };

    class Interactable : public component::Component
    {
    public:
        Interactable() : Component(ComponentType::Interactable) {}
        virtual ~Interactable() {}
    };

    class MouseArea : public component::Interactable
    {
    public:
        MouseArea()  {}
        ~MouseArea() = default;

        virtual core::error onHitTest(const core::pt32f & pos) const { return core::error_failed; }
        virtual void onMouseEnter(const mosue_state & state) { mouseEnter(state); }
        virtual void onMouseMove(const mosue_state & state) { mouseMove(state); }
        virtual void onMouseLeave(const mosue_state & state) { mouseLeave(state); }

        virtual void onMouseDown(const mosue_state & state) { mouseDown(state); }
        virtual void onMouseUp(const mosue_state & state) { mouseUp(state); }
        virtual void onMouseClick(const mosue_state & state) { mouseClick(state); }
        virtual void onMouseDBClick(const mosue_state & state) { mouseDBClick(state); }

    public:
        core::event<void(const mosue_state & state)> mouseEnter;
        core::event<void(const mosue_state & state)> mouseMove;
        core::event<void(const mosue_state & state)> mouseLeave;
        core::event<void(const mosue_state & state)> mouseDown;
        core::event<void(const mosue_state & state)> mouseUp;
        core::event<void(const mosue_state & state)> mouseClick;
        core::event<void(const mosue_state & state)> mouseDBClick;
    };
}
