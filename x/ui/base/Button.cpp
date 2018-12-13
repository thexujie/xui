#include "stdafx.h"
#include "Button.h"

namespace ui::base
{
    Button::Button()
    {
        _border = core::vec4<core::dimenf>{ 1_px };
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
        if (_actived)
            return "button:active";
        else if (_hovered)
            return "button:hover";
        else
            return "button";
    }
}
