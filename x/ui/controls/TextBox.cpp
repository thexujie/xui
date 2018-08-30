#include "stdafx.h"
#include "TextBox.h"
#include "ui/renderables/Text.h"

namespace ui::controls
{
    const std::string TEXTBOX_ANIMATION_GROUP_CURSOR = "_textbox.cursor";
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
        _textblob = nullptr;
        _text = text;
    }

    core::si32f TextBox::contentSize() const
    {
        _confirmBlob();
        return _textblob ? _textblob->size() : core::si32f();
    }

    void TextBox::updateContent()
    {
        _confirmBlob();
        if (_textblob)
        {
            if(!_text_obj)
            {
                _text_obj = std::make_shared<renderables::Text>(control_ref());
                insert(LOCAL_DEPTH_CONTENT, _text_obj);
            }
            _text_obj->setTextBlob(_textblob);
            _text_obj->setRect(contentBox());
        }

        if (!_cursor_obj)
        {
            _cursor_obj = std::make_shared<renderables::Line>(control_ref());
            _cursor_obj->setVisible(false);
            insert(LOCAL_DEPTH_CONTENT + 1, _cursor_obj);
        }

        graphics::fontmetrics fm(font());
        auto cbox = contentBox();
        _cursor_obj->setRect({ cbox.x, cbox.y, 1, fm.height});
        _cursor_obj->setPoints({ cbox.x, cbox.y }, { cbox.x, cbox.y + fm.height });
        _cursor_obj->setPathStyle(graphics::PathStyle().stoke(core::colors::Red , graphics::stroke_style::solid).width(1));

        if(!_input_obj)
        {
            _input_obj = std::make_shared<component::Interactable>(control_ref());
            _input_obj->setAcceptInput(true);
            insert(_input_obj);

            _input_obj->mouseEnter += std::weak_bind(&TextBox::onMouseEnter, share_ref<TextBox>(), std::placeholders::_1);
            _input_obj->mouseMove += std::weak_bind(&TextBox::onMouseMove, share_ref<TextBox>(), std::placeholders::_1);
            _input_obj->mouseLeave += std::weak_bind(&TextBox::onMouseLeave, share_ref<TextBox>(), std::placeholders::_1);
            _input_obj->mouseDown += std::weak_bind(&TextBox::onMouseDown, share_ref<TextBox>(), std::placeholders::_1);
            _input_obj->mouseUp += std::weak_bind(&TextBox::onMouseUp, share_ref<TextBox>(), std::placeholders::_1);
            _input_obj->focus += std::weak_bind(&TextBox::onFocus, share_ref<TextBox>());
            _input_obj->blur += std::weak_bind(&TextBox::onBlur, share_ref<TextBox>());
        }
        _input_obj->setRect(box());
    }


    std::string TextBox::styleName() const
    {
        bool mousein = false;
        bool pressed = false;
        if (_input_obj)
        {
            mousein = _input_obj->mousein();
            pressed = _input_obj->pressed();
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

    void TextBox::onFocus()
    {
        if(!_cursor_anim)
        {
            auto accessor = make_accessor(&renderables::Line::setVisible, &renderables::Line::visible, core::parseBool, nullptr);
            auto interpolator = std::make_shared<core::property_interpolator_keyframes<bool>>();
            interpolator->set_values({ { 0.0f, true }, { 0.5f, false } });
            {
                std::lock_guard lock(*this);
                if (!_cursor_obj)
                {
                    _cursor_obj = std::make_shared<renderables::Line>(control_ref());
                    _cursor_obj->setVisible(false);
                    insert(LOCAL_DEPTH_CONTENT + 1, _cursor_obj);
                }
            }
            _cursor_anim = std::make_shared<core::property_animation>(_cursor_obj, accessor, interpolator);
            _cursor_anim->setDuration(1s);
            _cursor_anim->setLoop(9999);
            appendAnimation(TEXTBOX_ANIMATION_GROUP_CURSOR, _cursor_anim);
        }
        _cursor_anim->start();
        updateStyle();
    }

    void TextBox::onBlur()
    {
        std::lock_guard lock(*this);
        if (_cursor_obj)
            _cursor_obj->setVisible(false);
        if (_cursor_anim)
            _cursor_anim->stop();
        updateStyle();
    }

    void TextBox::_confirmBlob() const
    {
        if (!_textblob)
        {
            graphics::StringFormat format(font());
            format.color(color());
            const_cast<std::shared_ptr<graphics::TextBlob> &>(_textblob) = std::make_shared<graphics::TextBlob>(_text, format);
        }
    }
}
