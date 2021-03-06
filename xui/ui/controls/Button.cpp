#include "stdafx.h"
#include "Button.h"

namespace ui::controls
{
    Button::Button()
    {
        _border = core::vec4<core::dimenf>{ 1_px };
        _padding = {1_em, 0.5_em};
    }

    Button::Button(const std::u8string & text) : _text(text)
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
        properties[u8"text"] = core::make_accessor(&Button::setText, &Button::text);
    }

    void Button::propertyTable(core::property_table & properties)
    {
        Button::propertyTableCallback(properties);
    }

    void Button::setText(const std::u8string & text)
    {
		_text = text;
        refresh();
    }

	void Button::onEnterScene()
    {
		_text_object = scene()->createText();
		ui::base::Button::onEnterScene();
    }
	
	void Button::onLeaveScene()
    {
		_text_object.reset();
		ui::base::Button::onLeaveScene();
    }
	
    void Button::update()
    {
		_text_object->update(_text, font(), color());
        setContentSize(_text_object->bounds());
    }

    void Button::paint(drawing::Graphics & graphics, const core::rectf & clip) const
    {
        graphics.drawText(*_text_object, contentBox().leftTop(), drawing::StringFormat()._color(color()));
    }
}
