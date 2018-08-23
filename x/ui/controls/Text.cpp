#include "stdafx.h"
#include "Text.h"
#include "ui/renderables/Text.h"


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

    void Text::updateContent()
    {
        _confirmBlob();
        if(_textBlob)
        {
            if(!_text_obj)
            {
                _text_obj = std::make_shared<renderables::Text>(ref(), _textBlob);
                insert(_text_obj);
            }
            _text_obj->setRect(contentBox());
        }
    }

    void Text::_confirmBlob() const
    {
        if (!_textBlob)
        {
            graphics::StringFormat format(font());
            format.color(color());
            const_cast<std::shared_ptr<graphics::TextBlob> &>(_textBlob) = std::make_shared<graphics::TextBlob>(_text, format);
        }
    }
}
