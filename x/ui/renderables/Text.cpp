#include "stdafx.h"
#include "Text.h"

namespace ui::renderables
{
    Text::~Text() { }

    void Text::setTextBlob(std::shared_ptr<drawing::TextBlob> text)
    {
        if(_text != text)
        {
            _text = text;
            invalid();
        }
    }

    void Text::render(drawing::Graphics & graphics) const
    {
        Renderable::render(graphics);
        if (!_text)
            return;

        graphics.drawTextBlob(*_text, rect().pos);
    }
}
