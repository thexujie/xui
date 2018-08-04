#pragma once
#include "Component.h"

namespace controls::component
{
    enum mouse_button
    {
        left = 0x0001,
        middle = 0x0002,
        right = 0x0004,

        mask= 0x00ff,
    };

    class mosue_state
    {
    public:
        mosue_state() = default;
        mosue_state(const core::pt32f pos) : _pos(pos) {}

        void setPos(const core::pt32f & pos) { _pos = pos; }
        const core::pt32f & pos() const { return _pos; }

        void active(mouse_button button, bool a) { a ? _buttons |= button : _buttons &= ~button; }
        bool active(mouse_button button) const { return !!(_buttons & button); }

    private:
        core::pt32f _pos;
        int32_t _buttons = 0;
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
        virtual void onMouseEnter(const mosue_state & state) { _mousein = true;  mouseEnter(state); }
        virtual void onMouseMove(const mosue_state & state) { mouseMove(state); }
        virtual void onMouseLeave(const mosue_state & state) { _mousein = false; mouseLeave(state);  }

        virtual void onMouseDown(const mosue_state & state) { _pressed = true;  mouseDown(state); }
        virtual void onMouseUp(const mosue_state & state) { _pressed = false;  mouseUp(state); }
        virtual void onMouseClick(const mosue_state & state) { mouseClick(state); }
        virtual void onMouseDBClick(const mosue_state & state) { mouseDBClick(state); }

        bool mousein() const { return _mousein; }
        bool pressed() const { return _pressed; }

    public:
        core::event<void(const mosue_state & state)> mouseEnter;
        core::event<void(const mosue_state & state)> mouseMove;
        core::event<void(const mosue_state & state)> mouseLeave;
        core::event<void(const mosue_state & state)> mouseDown;
        core::event<void(const mosue_state & state)> mouseUp;
        core::event<void(const mosue_state & state)> mouseClick;
        core::event<void(const mosue_state & state)> mouseDBClick;

    private:
        bool _mousein= false;
        bool _pressed = false;
    };
}
