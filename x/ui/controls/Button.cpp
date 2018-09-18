#include "stdafx.h"
#include "Button.h"

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

    std::string Button::styleName() const
    {
        if (_pressed)
            return "button:active";
        else if (_mousein)
            return "button:hover";
        else
            return "button";
    }

    void Button::draw(drawing::Graphics & graphics, const drawing::Region & region) const
    {
        std::lock_guard l(*this);
        _drawBackground(graphics);
        if (_textBlob)
            graphics.drawTextBlob(*_textBlob, contentBox().leftTop());
        _drawBorder(graphics);
    }

    void Button::onMouseEnter(const input_state & state)
    {
        Control::onMouseEnter(state);
        restyle();
    }

    void Button::onMouseMove(const input_state & state)
    {
        Control::onMouseMove(state);
    }

    void Button::onMouseLeave(const input_state & state)
    {
        Control::onMouseLeave(state);
        restyle();
    }

    
    void Button::onMouseDown(const input_state & state, ui::mouse_button button)
    {
        Control::onMouseDown(state, button);
        restyle();
    }

    void Button::onMouseUp(const input_state & state, ui::mouse_button button)
    {
        Control::onMouseUp(state, button);
        restyle();
    }

    void Button::_confirmBlob() const
    {
        if (!_textBlob)
        {
            drawing::StringFormat format(font());
            format.color(color());
            _textBlob = std::make_shared<drawing::TextBlob>(_text, format);
        }
    }
}
