#include "stdafx.h"
#include "Text.h"


namespace ui::controls
{
    Text::Text()
    {
        
    }

    Text::Text(std::string text) : _text(text)
    {
        
    }

    Text::~Text()
    {
        
    }

    core::sizef Text::contentSize() const
    {
        _confirmBlob();
        return _textBlob ? _textBlob->size() : core::sizef();
    }

    void Text::paint(drawing::Graphics & graphics, const core::rectf & clip) const
    {
        if (_textBlob)
            graphics.drawTextBlob(*_textBlob, contentBox().leftTop(), drawing::StringFormat().color(color()));
    }

    void Text::setText(std::string text)
    {
        if (_text == text)
            return;

        _text = text;
        _textBlob.reset();
        rearrange();
    }

    void Text::_confirmBlob() const
    {
        if (!_textBlob)
        {
            drawing::StringFormat format(font());
            format.color(color());
            _textBlob = std::make_shared<drawing::TextBlob>(_text, format);
        }
    }
}
