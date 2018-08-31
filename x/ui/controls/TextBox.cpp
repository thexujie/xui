#include "stdafx.h"
#include "TextBox.h"
#include "ui/renderables/Text.h"

namespace ui::controls
{
    const std::string TEXTBOX_ANIMATION_GROUP_CURSOR = "_textbox.cursor";
    TextBox::TextBox()
    {
        _accept_input = true;
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
    }


    std::string TextBox::styleName() const
    {
        if (_pressed)
            return "textbox:active";
        else if (_mousein)
            return "textbox:hover";
        else
            return "textbox";
    }

    void TextBox::onMouseEnter(const mosue_state & state)
    {
        Control::onMouseEnter(state);
        restyle();
    }

    void TextBox::onMouseMove(const mosue_state & state)
    {
        Control::onMouseMove(state);
    }

    void TextBox::onMouseLeave(const mosue_state & state)
    {
        Control::onMouseLeave(state);
        restyle();
    }

    
    void TextBox::onMouseDown(const mosue_state & state)
    {
        Control::onMouseDown(state);
        restyle();
    }

    void TextBox::onMouseUp(const mosue_state & state)
    {
        Control::onMouseUp(state);
        restyle();
    }

    void TextBox::onFocus(std::shared_ptr<ImeContext> imecontext)
    {
        _imecontext = imecontext;
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
        restyle();
        _updateIme();
    }

    void TextBox::onBlur()
    {
        _imecontext = nullptr;
        //std::lock_guard lock(*this);
        if (_cursor_obj)
            _cursor_obj->setVisible(false);
        if (_cursor_anim)
            _cursor_anim->stop();
        restyle();
    }

    void TextBox::onChar(char32_t ch)
    {
        if (std::iscntrl(ch))
            return;
        
        char chars[4] = { 0 };
        size_t len = core::unicode_to_utf8(ch, chars);
        _text.append(chars, len);
        _textblob.reset();
        refresh();
    }

    void TextBox::_updateIme()
    {
        if (!_imecontext)
            return;

        _imecontext->setImeMode(_ime_mode);
        if(_ime_mode != ime_mode::disabled)
        {
            _imecontext->setCompositionPos(contentBox().leftTop());
            _imecontext->setCompositionFont(font());
        }
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
