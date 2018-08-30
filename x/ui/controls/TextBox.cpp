#include "stdafx.h"
#include "TextBox.h"
#include "ui/renderables/Text.h"
#include "ui/interactables/MouseRectangle.h"

namespace ui::controls
{
    TextBox::TextBox()
    {
    }

    TextBox::TextBox(std::string text) : _text(text)
    {
    }

    TextBox::~TextBox()
    {
        
    }

    void TextBox::propertyTableCallback(core::property_table & properties)
    {
        Control::propertyTableCallback(properties);
        properties["text"] = core::make_accessor(&TextBox::setText, &TextBox::text, core::parseString, nullptr);
    }

    void TextBox::propertyTable(core::property_table & properties)
    {
        TextBox::propertyTableCallback(properties);
    }

    void TextBox::setText(const std::string & text)
    {
        _textBlob = nullptr;
        _text = text;
    }

    core::si32f TextBox::contentSize() const
    {
        _confirmBlob();
        return _textBlob ? _textBlob->size() : core::si32f();
    }

    void TextBox::updateContent()
    {
        _confirmBlob();
        if (_textBlob)
        {
            if(!_text_obj)
            {
                _text_obj = std::make_shared<renderables::Text>(control_ref());
                insert(LOCAL_DEPTH_CONTENT, _text_obj);
            }
            _text_obj->setTextBlob(_textBlob);
            _text_obj->setRect(contentBox());
        }

        if(!_mrc_obj)
        {
            _mrc_obj = std::make_shared<interactables::MouseRectangle>(control_ref());
            insert(_mrc_obj);

            _mrc_obj->mouseEnter += std::weak_bind(&TextBox::onMouseEnter, share_ref<TextBox>(), std::placeholders::_1);
            _mrc_obj->mouseMove += std::weak_bind(&TextBox::onMouseMove, share_ref<TextBox>(), std::placeholders::_1);
            _mrc_obj->mouseLeave += std::weak_bind(&TextBox::onMouseLeave, share_ref<TextBox>(), std::placeholders::_1);
            _mrc_obj->mouseDown += std::weak_bind(&TextBox::onMouseDown, share_ref<TextBox>(), std::placeholders::_1);
            _mrc_obj->mouseUp += std::weak_bind(&TextBox::onMouseUp, share_ref<TextBox>(), std::placeholders::_1);
        }

        _mrc_obj->setRect(box());
    }


    std::string TextBox::styleName() const
    {
        bool mousein = false;
        bool pressed = false;
        if (_mrc_obj)
        {
            mousein = _mrc_obj->mousein();
            pressed = _mrc_obj->pressed();
        }

        if (pressed)
            return "textbox:active";
        else if (mousein)
            return "textbox:hover";
        else
            return "textbox";
    }

    void TextBox::onMouseEnter(const component::mosue_state & state)
    {
        updateStyle();
    }

    void TextBox::onMouseMove(const component::mosue_state & state)
    {
        
    }

    void TextBox::onMouseLeave(const component::mosue_state & state)
    {
        updateStyle();
    }

    
    void TextBox::onMouseDown(const component::mosue_state & state)
    {
        updateStyle();
    }

    void TextBox::onMouseUp(const component::mosue_state & state)
    {
        updateStyle();
    }

    void TextBox::_confirmBlob() const
    {
        if (!_textBlob)
        {
            graphics::StringFormat format(font());
            format.color(color());
            const_cast<std::shared_ptr<graphics::TextBlob> &>(_textBlob) = std::make_shared<graphics::TextBlob>(_text, format);
        }
    }
}
