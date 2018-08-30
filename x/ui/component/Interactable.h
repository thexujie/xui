#pragma once
#include "Component.h"

namespace ui::component
{
    enum mouse_button
    {
        none = 0,
        left = 0x0001,
        middle = 0x0002,
        right = 0x0004,

        mask= 0x00ff,
    };

    template<> struct enable_bitmasks<mouse_button> { static const bool enable = true; };

    enum class mouse_action
    {
        none = 0,
        entering,
        moving,
        pressing,
        clicking,
        releasing,
        dbclicking,
        leaving,

        v_wheeling,
    };

    class mosue_state
    {
    public:
        mosue_state() = default;
        mosue_state(const core::pt32f pos, mouse_action action) : _pos(pos), _action(action){}

        void setPos(const core::pt32f & pos) { _pos = pos; }
        const core::pt32f & pos() const { return _pos; }

        void setButton(mouse_button button, bool active) { active ? (_buttons |= button)  : (_buttons &= ~button); }
        mouse_button buttons() const { return _buttons; }

        void setAction(mouse_action action) { _action = action; }
        mouse_action action() const { return _action; }

        void setWheelLines(int32_t lines) { _wheel_lines = lines; }
        int32_t wheelLines() const { return _wheel_lines; }

    private:
        core::pt32f _pos;
        mouse_action _action = mouse_action::none;
        mouse_button _buttons = mouse_button::none;
        int32_t _wheel_lines = 0;
    };

    class Interactable : public component::Component
    {
    public:
        Interactable(std::shared_ptr<ui::Control> control) :Component(ComponentType::Interactable, control) {}
        ~Interactable() = default;

        virtual core::error onHitTest(const core::pt32f & pos) const { return _rect.contains(pos) ? core::error_ok : core::error_failed; }
        virtual void onMouseEnter(const mosue_state & state) { _mousein = true;  mouseEnter(state); }
        virtual void onMouseMove(const mosue_state & state) { mouseMove(state); }
        virtual void onMouseLeave(const mosue_state & state) { _pressed = false;  _mousein = false; mouseLeave(state); }

        virtual void onMouseDown(const mosue_state & state) { _pressed = true;  mouseDown(state); }
        virtual void onMouseUp(const mosue_state & state) { _pressed = false;  mouseUp(state); }
        virtual void onMouseClick(const mosue_state & state) { mouseClick(state); }
        virtual void onMouseDBClick(const mosue_state & state) { mouseDBClick(state); }
        virtual void onMouseWheel(const mosue_state & state) { mouseWheel(state); }

        bool mousein() const { return _mousein; }
        bool pressed() const { return _pressed; }

        void setCaptureButtons(mouse_button buttons) { _capture_buttons = buttons; }
        mouse_button captureButtons() const { return _capture_buttons; }

        void setAcceptWheelV(bool b) { _accept_wheel_v = b; }
        bool acceptWheelV() const { return _accept_wheel_v; }

    public:
        core::event<void(const mosue_state & state)> mouseEnter;
        core::event<void(const mosue_state & state)> mouseMove;
        core::event<void(const mosue_state & state)> mouseLeave;
        core::event<void(const mosue_state & state)> mouseDown;
        core::event<void(const mosue_state & state)> mouseUp;
        core::event<void(const mosue_state & state)> mouseClick;
        core::event<void(const mosue_state & state)> mouseDBClick;
        core::event<void(const mosue_state & state)> mouseWheel;

    private:
        bool _mousein= false;
        bool _pressed = false;
        mouse_button _capture_buttons = mouse_button::none;
        bool _accept_wheel_v = false;
    };
}
