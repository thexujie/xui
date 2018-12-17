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

    void Lable::update()
    {
        _text.update(font(), color());
        setContentSize(_text.bounds());
    }

    void Lable::paint(drawing::Graphics & graphics, const core::rectf & clip) const
    {
        graphics.drawText(_text, contentBox().leftTop(), drawing::StringFormat().color(color()));
    }

    void Lable::setText(std::string text)
    {
        _text.setText(text);
        refresh();
    }
}
