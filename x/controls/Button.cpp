#include "stdafx.h"
#include "Button.h"
#include "renderables/Text.h"
#include "interactables/MouseRectangle.h"

namespace controls
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

    void Button::propertyTable(std::vector<std::shared_ptr<core::property_builder>> & builders)
    {
        Control::propertyTable(builders);
        builders.push_back(core::make_builder("text", &Button::setText, &Button::text, core::parseString));
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

    void Button::onMouseEnter(const component::mosue_state & state)
    {
        _applyStyle();
        update();
    }

    void Button::onMouseMove(const component::mosue_state & state)
    {
        
    }

    void Button::onMouseLeave(const component::mosue_state & state)
    {
        _applyStyle();
        update();
    }

    
    void Button::onMouseDown(const component::mosue_state & state)
    {
        _applyStyle();
        update();
    }

    void Button::onMouseUp(const component::mosue_state & state)
    {
        _applyStyle();
        update();
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

    void Button::_applyStyle()
    {
        if (!_mosuerectangle || !_styleSheet)
            return;

        bool mousein = _mosuerectangle->mousein();
        bool pressed = _mosuerectangle->pressed();
        std::string name;
        if (mousein && pressed)
            name = "button hoving pressed";
        else if (pressed)
            name = "button pressed";
        else if (mousein)
            name = "button hoving";
        else
            name = "button";

        std::vector<std::shared_ptr<core::property_builder>> builders;
        Button::propertyTable(builders);

        std::shared_ptr<component::Style> style = _styleSheet->select(name);
        if(style)
        {
            for (auto & item : style->items)
            {
                for (auto & builder : builders)
                {
                    if (core::string::equal_ic(item.first, builder->name))
                    {
                        auto pb = builder->build(item.second);
                        if (pb)
                            pb->apply(*this);
                        break;
                    }
                }
            }
            update();
        }
    }
}
