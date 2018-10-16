#pragma once
#include "ui/Control.h"

namespace ui::base
{
    class Button : public Control
    {
    public:
        Button();
        Button(std::string text);
        ~Button();

        static void propertyTableCallback(core::property_table & properties);
        void propertyTable(core::property_table & properties) override;

        std::string styleName() const override;

    public:
        void onMouseEnter(const input_state & state);
        void onMouseMove(const input_state & state);
        void onMouseLeave(const input_state & state);
        void onMouseDown(const input_state & state, ui::mouse_button button);
        void onMouseClick(const input_state & state, mouse_button button);
        void onMouseUp(const input_state & state, ui::mouse_button button);

    public:
        core::event<void()> click;
    };
}
