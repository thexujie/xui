#include "stdafx.h"
#include "Button.h"
#include "ui/renderables/Text.h"

namespace ui::controls
{
    Button::Button()
    {
        _border = core::vec4<core::dimensionf>{ 1_px };
        _padding = {1_em, 0.5_em};
    }

    Button::Button(std::string text) : _text(text)
    {
        _border = core::vec4<core::dimensionf>{ 1_px };
        _padding = { 1_em, 0.5_em };
    }

    Button::~Button()
    {
        
    }

    void Button::propertyTableCallback(core::property_table & properties)
    {
        Control::propertyTableCallback(properties);
        properties["text"] = core::make_accessor(&Button::setText, &Button::text, core::parseString, nullptr);
    }

    void Button::propertyTable(core::property_table & properties)
    {
        Button::propertyTableCallback(properties);
    }

    void Button::setText(const std::string & text)
    {
        _textBlob = nullptr;
        _text = text;
    }

    core::si32f Button::contentSize() const
    {
        _confirmBlob();
        return _textBlob ? _textBlob->size() : core::si32f();
    }

    void Button::updateContent()
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
    }


    std::string Button::styleName() const
    {
        if (_pressed)
            return "button:active";
        else if (_mousein)
            return "button:hover";
        else
            return "button";
    }

    void Button::onMouseEnter(const mosue_state & state)
    {
        Control::onMouseEnter(state);
        updateStyle();
    }

    void Button::onMouseMove(const mosue_state & state)
    {
        Control::onMouseMove(state);
    }

    void Button::onMouseLeave(const mosue_state & state)
    {
        Control::onMouseLeave(state);
        updateStyle();
    }

    
    void Button::onMouseDown(const mosue_state & state)
    {
        Control::onMouseDown(state);
        updateStyle();
    }

    void Button::onMouseUp(const mosue_state & state)
    {
        Control::onMouseUp(state);
        updateStyle();
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
