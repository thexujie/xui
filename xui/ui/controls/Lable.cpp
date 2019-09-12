#include "stdafx.h"
#include "Lable.h"


namespace ui::controls
{
    Lable::Lable()
    {
        
    }

    Lable::~Lable()
    {
        
    }

	void Lable::onEnterScene()
	{
		_text_object = scene()->createText();
		Control::onEnterScene();
	}

	void Lable::onLeaveScene()
	{
		_text_object.reset();
		Control::onLeaveScene();
	}
	
    void Lable::update()
    {
		_text_object->update(_text, font(), color());
        setContentSize(_text_object->bounds());
    }

    void Lable::paint(drawing::Graphics & graphics, const core::rectf & clip) const
    {
        graphics.drawText(*_text_object, contentBox().leftTop(), drawing::StringFormat()._color(color()));
    }

    void Lable::setText(const std::u8string & text)
    {
		_text = text;
        refresh();
    }
}
