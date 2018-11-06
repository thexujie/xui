#include "stdafx.h"
#include "Button.h"

namespace ui::controls
{
    Button::Button()
    {
        _border = core::vec4<core::dimensionf>{ 1_px };
        _padding = {1_em, 0.5_em};
    }

    Button::Button(std::string text) : _text(text)
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
        properties["text"] = core::make_accessor(&Button::setText, &Button::text);
    }

    void Button::propertyTable(core::property_table & properties)
    {
        Button::propertyTableCallback(properties);
    }

    void Button::setText(const std::string & text)
    {
        _textBlob = nullptr;
        _text = text;
    }

    core::sizef Button::contentSize() const
    {
        _confirmBlob();
        return _textBlob ? _textBlob->size() : core::sizef();
    }

    void Button::paint(drawing::Graphics & graphics, const core::rectf & clip) const
    {
        if (_textBlob)
            graphics.drawTextBlob(*_textBlob, contentBox().leftTop(), drawing::StringFormat().color(color()));
    }

    void Button::_confirmBlob() const
    {
        if (!_textBlob)
        {
            drawing::StringFormat format(font());
            format.color(color());
            _textBlob = std::make_shared<drawing::TextBlob>(_text, format);
        }
    }
}
