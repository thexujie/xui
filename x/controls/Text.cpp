#include "stdafx.h"
#include "Text.h"
#include "renderables/Text.h"


namespace controls
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

    void Text::enterScene(std::shared_ptr<component::Scene> & scene)
    {
        if (!_textBlob)
        {
            graphics::StringFormat format(font());
            format.color(color());
            _textBlob = std::make_shared<graphics::TextBlob>(_text, format);
        }
    }

    core::si32f Text::contentSize() const
    {
        if (!_textBlob)
            return {};
        return _textBlob->size();
    }

    void Text::update()
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
