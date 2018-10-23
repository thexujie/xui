#include "stdafx.h"
#include "Button.h"

namespace ui::base
{
    Button::Button()
    {
        _border = core::vec4<core::dimensionf>{ 1_px };
        _padding = { 1_em, 0.5_em };
    }

    Button::~Button()
    {

    }

    void Button::propertyTableCallback(core::property_table & properties)
    {
        Control::propertyTableCallback(properties);
    }

    void Button::propertyTable(core::property_table & properties)
    {
        Button::propertyTableCallback(properties);
    }

    std::string Button::styleName() const
    {
        if (_pressed)
            return "button:active";
        else if (_mousein)
            return "button:hover";
        else
            return "button";
    }

    void Button::onMouseEnter(const input_state & state)
    {
        Control::onMouseEnter(state);
        restyle();
    }

    void Button::onMouseMove(const input_state & state)
    {
        Control::onMouseMove(state);
    }

    void Button::onMouseLeave(const input_state & state)
    {
        Control::onMouseLeave(state);
        restyle();
    }


    void Button::onMouseDown(const input_state & state, ui::mouse_button button)
    {
        Control::onMouseDown(state, button);
        restyle();
    }

    void Button::onMouseClick(const input_state & state, mouse_button button)
    {
        click();
        Control::onMouseClick(state, button);
    }

    void Button::onMouseUp(const input_state & state, ui::mouse_button button)
    {
        Control::onMouseUp(state, button);
        restyle();
    }
}
