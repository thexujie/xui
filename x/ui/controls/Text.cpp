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

    core::si32f Text::contentSize() const
    {
        _confirmBlob();
        return _textBlob ? _textBlob->size() : core::si32f();
    }

    void Text::draw(drawing::Graphics & graphics, const core::rc32f & clip) const
    {
        std::lock_guard l(*this);
        _drawBackground(graphics);
        if (_textBlob)
            graphics.drawTextBlob(*_textBlob, contentBox().leftTop());
        _drawBorder(graphics);
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
