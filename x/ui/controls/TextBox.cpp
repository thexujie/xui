#include "stdafx.h"
#include "TextBox.h"
#include <SkTextBlob.h>
#include "ui/renderables/Text.h"
#include "drawing/skia/script.h"

namespace ui::controls
{
    static size_t textbox_u8_rfind_next(const std::string & str, size_t offset = core::npos)
    {
        if (offset == core::npos)
            offset = str.length();

        if (offset == 0)
            return core::npos;

        size_t nfound = 1;
        for (size_t index = offset; index > 0; --index)
        {
            uint8_t ch = str[index - 1];

            size_t nbytes = 0;
            if ((ch & 0b11111100) == 0b11111100)
                nbytes = 6;
            else if ((ch & 0b11111000) == 0b11111000)
                nbytes = 5;
            else if ((ch & 0b11110000) == 0b11110000)
                nbytes = 4;
            else if ((ch & 0b11100000) == 0b11100000)
                nbytes = 3;
            else if ((ch & 0b11000000) == 0b11000000)
                nbytes = 2;
            else if ((ch & 0b10000000) == 0b10000000)
                nbytes = 1;
            else
                nbytes = 0;
            if (nbytes < nfound)
                break;

            nfound = nbytes;
        }

        return nfound <= str.length() ? str.length() - nfound : core::npos;
    }

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
        _text = text;
        reshaper();
    }

    core::si32f TextBox::contentSize() const
    {
        return _textblob ? _textblob->size() : core::si32f();
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

    void TextBox::render(drawing::Graphics & graphics, const drawing::Region & region) const
    {
        std::lock_guard l(*this);
        _renderBackground(graphics);
        if (_textblob)
            graphics.drawTextBlob(*_textblob, contentBox().leftTop());

        if (_shaper && _cursor_shown)
        {
            auto rcCurr = _shaper->charRect(_text.length() - 1);
            auto cbox = contentBox();
            graphics.drawLine({ cbox.x + rcCurr.right(), cbox.y + rcCurr.y }, { cbox.x + rcCurr.right(), cbox.y + rcCurr.y + rcCurr.height },
                drawing::PathStyle().stoke(core::colors::Red, drawing::stroke_style::solid).width(1));
        }
        _renderBorder(graphics);
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
            auto accessor = make_accessor(&TextBox::_setCursorShown, &TextBox::_cursorShown, core::parseBool, nullptr);
            auto interpolator = std::make_shared<core::property_interpolator_keyframes<bool>>();
            interpolator->set_values({ { 0.0f, true }, { 0.5f, false } });
            _cursor_anim = std::make_shared<core::property_animation>(control_ref(), accessor, interpolator);
            _cursor_anim->setDuration(1s);
            _cursor_anim->setLoop(9999);
            appendAnimation(TEXTBOX_ANIMATION_GROUP_CURSOR, _cursor_anim);
        }
        _cursor_anim->start();
        restyle();

        if(_imecontext)
            _imecontext->setImeMode(_ime_mode);
        _updateIme();
    }

    void TextBox::onBlur()
    {
        _imecontext = nullptr;
        //std::lock_guard lock(*this);
        _setCursorShown(false);
        if (_cursor_anim)
            _cursor_anim->stop();
        restyle();
    }

    void TextBox::onChar(char32_t ch)
    {
        if (ch <= 0xffff && std::iswcntrl((wint_t)ch))
        {
            if(ch == 8)
            {
                if(!_text.empty())
                {
                    auto pos = textbox_u8_rfind_next(_text);
                    if (pos != core::npos)
                    {
                        _text.resize(pos);
                        reshaper();
                    }
                }
            }
            return;
        }
        
        char chars[6] = { 0 };
        size_t len = core::utf32_to_utf8(ch, chars, 6);
        _text.append(chars, len);
        reshaper();
    }

    void TextBox::reshaper()
    {
        if(!_delay_shaper)
        {
            _delay_shaper = true;
            invoke([this]() { _doshaper(); });
        }
    }

    void TextBox::_updateIme()
    {
        if (!_imecontext)
            return;

        if(_ime_mode != ime_mode::disabled)
        {
            if(_shaper)
            {
                auto rcCurr = _shaper->charRect(_text.length() - 1);
                _imecontext->setCompositionPos(contentBox().leftTop() + core::vec2f(rcCurr.right(), 0));
            }
            _imecontext->setCompositionFont(font());
        }
    }

    void TextBox::_confirmBlob() const
    {

    }

    void TextBox::_doshaper()
    {
        _delay_shaper = false;

        if (!_textblob)
            _textblob = std::make_shared<drawing::TextBlob>();

        if (!_shaper)
            _shaper = std::make_shared<drawing::script::Shaper>();

        _shaper->reset(_text);
        _shaper->itermize();
        _shaper->wrap(std::numeric_limits<float32_t>::max(), drawing::wrap_mode::word);

        auto native = _shaper->build(0);
        {
            std::lock_guard l(*this);
            _textblob->setNative(native, _shaper->lineSize(0));
            invalid();
        }

        _updateIme();
    }

    void TextBox::_setCursorShown(bool vis)
    {
        if(_cursor_shown != vis)
        {
            _cursor_shown = vis;
            invalid();
        }
    }
}
