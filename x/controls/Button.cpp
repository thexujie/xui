#include "stdafx.h"
#include "Button.h"
#include "renderables/Text.h"

namespace controls
{
    Button::Button()
    {
        
    }

    Button::~Button()
    {
        
    }

    core::si32f Button::contentSize() const
    {
        if (!_textBlob)
            return {};
        return _textBlob->size();
    }

    void Button::layoutContent()
    {
        if (!_textBlob)
        {
            graphics::StringFormat format(font());
            format.color(color());
            _textBlob = std::make_shared<graphics::TextBlob>(_text, format);
        }
    }

    void Button::update()
    {
        Control::update();
        if(_textBlob)
        {
            auto v = view();
            auto item = std::make_shared<renderables::Text>(_textBlob);
            item->setPos(contentBox().pos);
            v->insert(0, item);
        }
    }
}
