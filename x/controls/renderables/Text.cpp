#include "stdafx.h"
#include "Text.h"

namespace controls::renderables
{
    Text::Text(std::shared_ptr<component::View> view) : Renderable(view) { }

    Text::Text(std::shared_ptr<component::View> view, std::shared_ptr<graphics::TextBlob> text) : Renderable(view), _text(text) { }

    Text::~Text() { }

    void Text::setText(std::shared_ptr<graphics::TextBlob> text)
    {
        _text = text;
    }

    void Text::render(graphics::Graphics & graphics) const
    {
        Renderable::render(graphics);
        if (!_text)
            return;

        graphics.drawTextBlob(*_text, pos());
    }
}
