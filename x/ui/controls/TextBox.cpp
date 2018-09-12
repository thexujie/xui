#include "stdafx.h"
#include "TextBox.h"
#include <SkTextBlob.h>

namespace ui::controls
{
    static size_t textbox_u8_rfind_next(const std::string & str, size_t offset = core::npos)
    {
        if (offset == core::npos)
            offset = str.length() - 1;

        size_t nfound = 1;
        for (size_t index = offset + 1; index > 0; --index)
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

        return nfound <= offset ? offset - nfound + 1: core::npos;
    }

    const std::string TEXTBOX_ANIMATION_GROUP_CURSOR = "_textbox.cursor";
    TextBox::TextBox()
    {
        _accept_input = true;
        _shaper = std::make_shared<drawing::Shaper>();
        auto accessor = make_accessor(&TextBox::_setCursorShown, &TextBox::_cursorShown, core::parseBool, nullptr);
        auto interpolator = std::make_shared<core::property_interpolator_keyframes<bool>>();
        interpolator->set_values({ { 0.0f, true }, { 0.5f, false } });
        _cursor_anim = std::make_shared<core::property_animation>(control_ref(), accessor, interpolator);
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
        reshaper(shaper_flag::shaper);
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

        if (_cursor_shown)
        {
            auto & glyph = _shaper->findGlyph(_cursor_pos);
            core::rc32f rect = { glyph.pos.x, glyph.pos.y, glyph.size.cx, glyph.size.cy };
            auto cbox = contentBox();
            //graphics.drawLine({ cbox.x + rcCurr.right(), cbox.y + rcCurr.y }, { cbox.x + rcCurr.right(), cbox.y + rcCurr.y + rcCurr.height },
            //    drawing::PathStyle().stoke(core::colors::Red, drawing::stroke_style::solid).width(1));
            graphics.drawRectangle(rect.offseted(cbox.leftTop()), drawing::PathStyle().fill(0x80ff0000));
        }
        _renderBorder(graphics);
    }

    void TextBox::onMouseEnter(const input_state & state)
    {
        Control::onMouseEnter(state);
        restyle();
    }

    void TextBox::onMouseMove(const input_state & state)
    {
        Control::onMouseMove(state);
    }

    void TextBox::onMouseLeave(const input_state & state)
    {
        Control::onMouseLeave(state);
        restyle();
    }

    
    void TextBox::onMouseDown(const input_state & state, ui::mouse_button button)
    {
        Control::onMouseDown(state, button);
        restyle();
    }

    void TextBox::onMouseUp(const input_state & state, ui::mouse_button button)
    {
        Control::onMouseUp(state, button);
        restyle();
    }

    void TextBox::onKeyDown(const input_state & state, keycode key)
    {
        Control::onKeyDown(state, key);
        switch(key)
        {
        case keycode::left:
            caretLeft();
            break;
        case keycode::right:
            caretRight();
            break;
        case keycode::backspace:
            backSpace();
            break;
        default:
            break;
        }
    }

    void TextBox::onFocus(std::shared_ptr<ImeContext> imecontext)
    {
        _imecontext = imecontext;
        if(!_cursor_anim)
        {
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
            return;
        
        char chars[6] = { 0 };
        size_t len = core::utf32_to_utf8(ch, chars, 6);
        auto & glyph_curr = _shaper->findGlyph(_cursor_pos);
        if (!glyph_curr.trange.length)
            return;
        insert(glyph_curr.trange.end(), chars, len);
    }

    void TextBox::reshaper(shaper_flags flags)
    {
        _delay_shaper_flags |= flags;
        if(!_delay_shaper)
        {
            _delay_shaper = true;
            invoke([this]() { _doshaper(); });
        }
    }

    void TextBox::caretLeft()
    {
        if (!_cursor_pos)
            return;

        auto & glyph_curr = _shaper->findGlyph(_cursor_pos);
        if (!glyph_curr.trange.index)
            return;

        size_t index = glyph_curr.trange.index - 1;
        while(index >= 0)
        {
            auto & glyph = _shaper->findGlyph(index);
            if (!glyph.trange.length)
                return;

            if(glyph.charbreak)
            {
                _cursor_pos = glyph.trange.index;
                _cursor_shown = true;
                _cursor_anim->reset();
                reshaper(shaper_flag::caret);
                break;
            }

            if (!glyph.trange.index)
                break;
            index = glyph.trange.index - 1;
        }
    }

    void TextBox::caretRight()
    {
        if (_cursor_pos >= _text.length())
            return;

        auto & glyph_curr = _shaper->findGlyph(_cursor_pos);
        size_t index = glyph_curr.trange.end();
        do
        {
            auto & glyph = _shaper->findGlyph(index);
            if (!glyph.trange.length)
                break;

            if (glyph.charbreak)
            {
                _cursor_pos = glyph.trange.index;
                _cursor_shown = true;
                _cursor_anim->reset();
                reshaper(shaper_flag::caret);
                break;
            }

            if (glyph.trange.end() == _text.length())
                break;
            index = glyph.trange.end();
        }
        while (index <= _text.length());
    }


    void TextBox::backSpace()
    {
        auto & glyph = _shaper->findGlyph(_cursor_pos);
        if (glyph.trange.length)
        {
            _text.erase(glyph.trange.index, glyph.trange.length);
            if(_cursor_pos != core::npos && _cursor_pos >= glyph.trange.index)
                _cursor_pos -= glyph.trange.length;
            assert(_cursor_pos == core::npos || _cursor_pos < _text.length());
            reshaper(shaper_flag::shaper);
        }
    }

    void TextBox::insert(size_t offset, const char * text, size_t count)
    {
        if (offset == core::npos)
            _text.append(text, count);
        else
            _text.insert(offset, text, count);

        if (_cursor_pos != core::npos && _cursor_pos >= offset)
            _cursor_pos += count;

        _cursor_shown = true;
        _cursor_anim->reset();
        reshaper(shaper_flag::shaper);
    }

    void TextBox::_updateIme()
    {
        if (!_imecontext)
            return;

        if(_ime_mode != ime_mode::disabled)
        {
            auto & glyph = _shaper->findGlyph(_cursor_pos == core::npos ? _text.length() - 1 : _cursor_pos);
            core::rc32f rect = { glyph.pos.x, glyph.pos.y, glyph.size.cx, glyph.size.cy };
            _imecontext->setCompositionPos(contentBox().leftTop() + core::vec2f(rect.right(), 0));
            _imecontext->setCompositionFont(font());
        }
    }

    void TextBox::_confirmBlob() const
    {

    }

    void TextBox::_doshaper()
    {
        _delay_shaper = false;

        if(_delay_shaper_flags.any(shaper_flag::shaper))
        {
            if (!_textblob)
                _textblob = std::make_shared<drawing::TextBlob>();

            _shaper->itermize(_text, _font, _color);
            _shaper->wrap(std::numeric_limits<float32_t>::max(), drawing::wrap_mode::word);

            auto native = _shaper->build(0);
            {
                std::lock_guard l(*this);
                _textblob->setNative(native, _shaper->lineSize(0));
            }
        }

        if (_delay_shaper_flags.any(shaper_flag::shaper | shaper_flag::caret))
            refresh();

        _delay_shaper_flags.clear();
        _updateIme();
    }

    void TextBox::_setCursorShown(bool vis)
    {
        if(_cursor_shown != vis)
        {
            _cursor_shown = vis;
            refresh();
        }
    }
}
