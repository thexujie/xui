#include "stdafx.h"
#include "Button.h"
#include "ui/renderables/Text.h"
#include "ui/interactables/MouseRectangle.h"

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
        Control::propertyTable(properties);
        properties["text"] = core::make_accessor(&Button::setText, &Button::text, core::parseString, nullptr);
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

    void Button::updateContent(std::shared_ptr<component::View> & view)
    {
        _confirmBlob();
        if (_textBlob)
        {
            if(!_text_obj)
            {
                _text_obj = std::make_shared<renderables::Text>();
                view->insert(DEPTH_CONTENT, _text_obj);
            }
            _text_obj->setTextBlob(_textBlob);
            _text_obj->setRect(contentBox());
        }

        if(!_mosuerectangle)
        {
            _mosuerectangle = std::make_shared<interactables::MouseRectangle>();
            view->insert(_mosuerectangle);

            _mosuerectangle->mouseEnter += std::weak_binder(std::mem_fn(&Button::onMouseEnter), shared_from_this());
            _mosuerectangle->mouseMove += std::weak_bind(&Button::onMouseMove, share_ref<Button>(), std::placeholders::_1);
            _mosuerectangle->mouseLeave += std::weak_bind(&Button::onMouseLeave, share_ref<Button>(), std::placeholders::_1);
            _mosuerectangle->mouseDown += std::weak_binder(std::mem_fn(&Button::onMouseDown), shared_from_this());
            _mosuerectangle->mouseUp += std::weak_binder(std::mem_fn(&Button::onMouseUp), shared_from_this());
        }

        _mosuerectangle->setRect(box());
    }


    std::string Button::styleName() const
    {
        bool mousein = false;
        bool pressed = false;
        if (_mosuerectangle)
        {
            mousein = _mosuerectangle->mousein();
            pressed = _mosuerectangle->pressed();
        }

        if (pressed)
            return "button:active";
        else if (mousein)
            return "button:hover";
        else
            return "button";
    }

    void Button::onMouseEnter(const component::mosue_state & state)
    {
        updateStyle();
    }

    void Button::onMouseMove(const component::mosue_state & state)
    {
        
    }

    void Button::onMouseLeave(const component::mosue_state & state)
    {
        updateStyle();
    }

    
    void Button::onMouseDown(const component::mosue_state & state)
    {
        updateStyle();
    }

    void Button::onMouseUp(const component::mosue_state & state)
    {
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
