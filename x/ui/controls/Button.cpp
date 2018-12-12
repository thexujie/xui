#include "stdafx.h"
#include "Button.h"

namespace ui::controls
{
    Button::Button()
    {
        _border = core::vec4<core::dimenf>{ 1_px };
        _padding = {1_em, 0.5_em};
    }

    Button::Button(std::string text) : _text(text)
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
        properties["text"] = core::make_accessor(&Button::setText, &Button::text);
    }

    void Button::propertyTable(core::property_table & properties)
    {
        Button::propertyTableCallback(properties);
    }

    void Button::setText(const std::string & text)
    {
        _text.setText(text);
        _text.update(font(), color());
    }

    core::sizef Button::contentSize() const
    {
        return _text.bounds();
    }

    void Button::paint(drawing::Graphics & graphics, const core::rectf & clip) const
    {
            graphics.drawText(_text, contentBox().leftTop(), drawing::StringFormat().color(color()));
    }
}
