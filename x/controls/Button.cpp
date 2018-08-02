#include "stdafx.h"
#include "Button.h"
#include "renderables/Text.h"
#include "interactables/MouseRectangle.h"

namespace controls
{
    Button::Button()
    {
        _padding = {1_em, 0.5_em};
    }

    Button::Button(std::string text) : _text(text)
    {
        _padding = { 1_em, 0.5_em };
    }

    Button::~Button()
    {
        
    }

    core::si32f Button::contentSize() const
    {
        _confirmBlob();
        return _textBlob ? _textBlob->size() : core::si32f();
    }

    void Button::updateContent(std::shared_ptr<component::View> & view)
    {
        _confirmBlob();
        if (_textBlob)
        {
            if(!_text_obj)
            {
                _text_obj = std::make_shared<renderables::Text>(view, _textBlob);
                view->insert(_text_obj);
            }
            _text_obj->setPos(contentBox().pos);
        }

        if(!_mosuerectangle)
            _mosuerectangle = std::make_shared<interactables::MouseRectangle>(view);
    }

    void Button::_confirmBlob() const
    {
        if (!_textBlob)
        {
            graphics::StringFormat format(font());
            format.color(color());
            const_cast<std::shared_ptr<graphics::TextBlob> &>(_textBlob) = std::make_shared<graphics::TextBlob>(_text, format);
        }
    }
}
