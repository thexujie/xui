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

    core::si32f Text::contentSize() const
    {
        _confirmBlob();
        return _textBlob ? _textBlob->size() : core::si32f();
    }

    void Text::enteringScene(std::shared_ptr<component::Scene> & scene)
    {
        Control::enteringScene(scene);
    }

    void Text::enterScene(std::shared_ptr<component::Scene> & scene)
    {
        Control::enterScene(scene);
    }

    void Text::updateContent()
    {
        _confirmBlob();
        if(_textBlob)
        {
            auto v = view();
            auto item = std::make_shared<renderables::Text>(_textBlob);
            item->setPos(contentBox().pos);
            v->insert(0, item);
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
