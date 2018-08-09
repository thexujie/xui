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

    void Button::propertyTable(core::property_table & properties)
    {
        Control::propertyTable(properties);
        properties["text"] = core::make_property(&Button::setText, &Button::text, core::parseString);
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
        applyStyle();
        update();
    }

    void Button::onMouseMove(const component::mosue_state & state)
    {
        
    }

    void Button::onMouseLeave(const component::mosue_state & state)
    {
        applyStyle();
        update();
    }

    
    void Button::onMouseDown(const component::mosue_state & state)
    {
        applyStyle();
        update();
    }

    void Button::onMouseUp(const component::mosue_state & state)
    {
        applyStyle();
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

    void Button::applyStyle()
    {
        auto ss = styleSheet();
        if (!ss)
            return;

        bool mousein = false;
        bool pressed = false;
        if (_mosuerectangle)
        {
            mousein = _mosuerectangle->mousein();
            pressed = _mosuerectangle->pressed();
        }

        core::property_table properties;
        Button::propertyTable(properties);

        std::shared_ptr<component::Style> style = ss->select("button");
        if(style)
        {
            std::map<std::string, std::string> items = style->items;
            if (mousein && style->pseudos.find("hover") != style->pseudos.end())
            {
                for (auto & item : style->pseudos["hover"])
                    items[item.first] = item.second;
            }
            if (pressed && style->pseudos.find("active") != style->pseudos.end())
            {
                for (auto & item : style->pseudos["active"])
                    items[item.first] = item.second;
            }

            for (const auto & item : items)
            {
                auto iter = properties.find(item.first);
                if (iter == properties.end())
                    continue;

                auto accessor = iter->second.first;
                auto serializer = iter->second.second;
                if (accessor && serializer)
                    serializer->set(item.second, *this, *accessor);
            }
            update();
        }
    }
}
