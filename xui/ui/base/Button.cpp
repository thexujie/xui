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

    std::u8string Button::styleName() const
    {
        if (_actived)
            return u8"button:active";
        else if (_hovered)
            return u8"button:hover";
        else
            return u8"button";
    }
}
