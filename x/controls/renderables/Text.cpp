#include "stdafx.h"
#include "Text.h"

namespace controls::renderables
{
    Text::Text() { }

    Text::Text(std::shared_ptr<graphics::TextBlob> text) : _text(text) { }

    Text::~Text() { }

    void Text::setTextBlob(std::shared_ptr<graphics::TextBlob> text)
    {
        if(_text != text)
        {
            _text = text;
            invalid();
        }
    }

    void Text::render(graphics::Graphics & graphics) const
    {
        Renderable::render(graphics);
        if (!_text)
            return;

        graphics.drawTextBlob(*_text, pos());
    }
}
