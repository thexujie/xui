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
        auto cbox = contentBox();
        std::lock_guard l(*this);
        _renderBackground(graphics);
        graphics.save();
        graphics.setClipRect(cbox);
        if (_textblob)
            graphics.drawTextBlob(*_textblob, contentBox().leftTop().offseted(_scroll_pos, 0));
        graphics.restore();

        if (_cursor_shown)
        {
            if(_text.empty())
            {
                drawing::fontmetrics fmetrics(font());
                graphics.drawRectangle({ cbox.leftTop(), { fmetrics.height * 0.5f, fmetrics.height } }, drawing::PathStyle().fill(0x80ff0000));
            }
            else
            {
                size_t gindex = _cursor_gindex + 1;
                while(gindex > 0)
                {
                    auto & glyph = _shaper->glyphAt(gindex - 1);
                    if (glyph.header)
                    {
                        core::rc32f rect = glyph.rect;
                        rect.offset(cbox.leftTop()).offset(_scroll_pos, 0);
                        graphics.drawRectangle(rect, drawing::PathStyle().fill(0x400000ff));
                        if (_cursor_far)
                            graphics.drawLine(rect.rightTop(), rect.rightBottom(), drawing::PathStyle().stoke(core::colors::Red, drawing::stroke_style::solid).width(calc_x(1_px)));
                        else
                            graphics.drawLine(rect.leftTop(), rect.leftBottom(), drawing::PathStyle().stoke(core::colors::Red, drawing::stroke_style::solid).width(calc_x(1_px)));
                        break;
                    }
                    --gindex;
                }
            }
        }
        _renderBorder(graphics);
    }

    void TextBox::onSizeChanged(const core::si32f & from, const core::si32f & to)
    {
        reshaper(shaper_flag::caret);
        Control::onSizeChanged(from, to);
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
        auto cbox = contentBox();
        float32_t pos = state.pos().x - cbox.x - _scroll_pos;
        auto & glyph = _shaper->findGlyph(pos, 0);
        if(glyph)
        {
            _cursor_gindex = glyph.gindex;
            _cursor_far = pos >= glyph.rect.centerX();
            _cursor_tindex = _cursor_far ? glyph.trange.end() : glyph.trange.index;
            _cursor_anim ? _cursor_anim->reset() : nullptr;
            reshaper(shaper_flag::caret);
        }
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
        case keycode::del:
            del();
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
        _cursor_anim ? _cursor_anim->stop() : nullptr;
        restyle();
    }

    void TextBox::onChar(char32_t ch)
    {
        if (ch <= 0xffff && std::iswcntrl((wint_t)ch))
            return;
        
        char chars[6] = { 0 };
        size_t len = core::utf32_to_utf8(ch, chars, 6);
        insert(chars, len);
    }

    void TextBox::reshaper(shaper_flags flags)
    {
#ifdef  _DEBUG
        size_t utf8_pos = 0;
        size_t nutf8 = 0;
        char32_t cp_utf8 = 0;
        do
        {
            nutf8 = core::utf8_to_utf32(_text.c_str() + utf8_pos, _text.length() - utf8_pos, cp_utf8);
            utf8_pos += nutf8;
        }
        while (nutf8 > 0);
        assert(utf8_pos <= _text.length());
#endif

        _delay_shaper_flags |= flags;
        if(!_delay_shaper)
        {
            _delay_shaper = true;
            invoke([this]() { _doshaper(); _docaret(); });
        }
    }

    void TextBox::caretLeft()
    {
        if (_delay_shaper_flags)
            _doshaper();

        size_t gindex = _cursor_far ? _cursor_gindex + 1: _cursor_gindex;
        while (gindex > 0)
        {
            --gindex;
            auto & glyph = _shaper->glyphAt(gindex);
            if (glyph.header)
            {
                _cursor_gindex = gindex;
                _cursor_tindex = glyph.trange.index;
                _cursor_far = false;
                _cursor_anim ? _cursor_anim->reset() : nullptr;
                _docaret();
                break;
            }
        }
    }

    void TextBox::caretRight()
    {
        if (_delay_shaper_flags)
            _doshaper();

        size_t gindex = _cursor_far ? _cursor_gindex + 1 : _cursor_gindex;
        while (gindex < _shaper->glyphCount())
        {
            auto & glyph = _shaper->glyphAt(gindex);
            if (glyph.tailer)
            {
                _cursor_gindex = gindex;
                _cursor_tindex = glyph.trange.end();
                _cursor_far = true;
                _cursor_anim ? _cursor_anim->reset() : nullptr;
                _docaret();
                break;
            }
            ++gindex;
        }
    }

    void TextBox::backSpace()
    {
        if (!_cursor_gindex && !_cursor_far)
            return;

        if (_delay_shaper_flags)
            _doshaper();

        size_t gindex = _cursor_far ? _cursor_gindex : _cursor_gindex - 1;
        auto & glyph = _shaper->glyphAt(gindex);
        _text.erase(glyph.trange.index, glyph.trange.length);
        if(_shaper->glyphCount() > 1 && gindex + 1 >= _shaper->glyphCount())
        {
            _cursor_gindex = gindex - 1;
            _cursor_far = true;
        }
        else
        {
            _cursor_gindex = gindex;
            _cursor_far = false;
        }
        _cursor_tindex = glyph.trange.index;
        _cursor_anim ? _cursor_anim->reset() : nullptr;
        reshaper(shaper_flag::shaper);
    }

    void TextBox::del()
    {
        if (_cursor_gindex >= _shaper->glyphCount() - 1 && _cursor_far)
            return;

        if (_delay_shaper_flags)
            _doshaper();

        size_t gindex = _cursor_far ? _cursor_gindex + 1: _cursor_gindex;
        auto & glyph = _shaper->glyphAt(gindex);
        if (gindex == 0)
        {
            _cursor_gindex = 0;
            _cursor_tindex = 0;
            _cursor_far = false;
        }
        else
        {
            _cursor_gindex = gindex - 1;
            _cursor_far = true;
        }
        _cursor_tindex = glyph.trange.index;
        _cursor_anim ? _cursor_anim->reset() : nullptr;

        reshaper(shaper_flag::shaper);
    }

    void TextBox::insert(const char * text, size_t count)
    {
        _text.insert(_cursor_tindex, text, count);
        _cursor_gindex = core::npos;
        _cursor_tindex += count;
        _cursor_far = true;
        _cursor_anim ? _cursor_anim->reset() : nullptr;
        reshaper(shaper_flag::shaper);
    }

    void TextBox::_updateIme()
    {
        if (!_imecontext)
            return;

        if(_ime_mode != ime_mode::disabled)
        {
            auto cbox = contentBox();
            auto & glyph = _shaper->glyphAt(_cursor_gindex);
            _imecontext->setCompositionPos(cbox.leftTop() + core::vec2f(glyph.rect.right() + _scroll_pos, 0));
            _imecontext->setCompositionFont(font());
        }
    }

    void TextBox::_confirmBlob() const
    {

    }

    void TextBox::_doshaper()
    {
        std::lock_guard l(*this);
        _delay_shaper = false;

        if(_delay_shaper_flags.any(shaper_flag::shaper))
        {
            if (!_textblob)
                _textblob = std::make_shared<drawing::TextBlob>();

            _shaper->itermize(_text, _font, _color);
            _shaper->wrap(std::numeric_limits<float32_t>::max(), drawing::wrap_mode::word);

            auto native = _shaper->build(0);
            _textblob->setNative(native, _shaper->lineSize(0));
        }

        refresh();
        _delay_shaper_flags.clear();
    }

    void TextBox::_docaret()
    {
        {
            std::lock_guard l(*this);

            if (_cursor_gindex == core::npos)
            {
                auto & glyph_curr = _shaper->findGlyph((_cursor_far && _cursor_tindex > 0) ? _cursor_tindex - 1 : _cursor_tindex);
                _cursor_gindex = glyph_curr.gindex;
            }

            auto cbox = contentBox();
            auto & glyph = _shaper->glyphAt(_cursor_gindex);
            if (glyph)
            {
                float32_t scroll_pos = _scroll_pos;
                if (_scroll_pos + glyph.rect.x < 0)
                    scroll_pos = -glyph.rect.x;
                else if (_scroll_pos + glyph.rect.right() >= cbox.width)
                    scroll_pos = cbox.cx - glyph.rect.right();
                else {}

                auto size = _shaper->lineSize(0);
                if (_scroll_pos + size.cx < cbox.width)
                {
                    float32_t scroll_max = cbox.width - size.cx;
                    if (scroll_max > 0)
                        scroll_max = 0;
                    scroll_pos = std::clamp(scroll_pos, scroll_max, 0.0f);
                }
                _scroll_pos = scroll_pos;
            }
        }

        refresh();
        _updateIme();
    }

    const drawing::glyph & TextBox::_lastGlyph(cursor pos) const
    {
        if (pos.far)
            return _shaper->glyphAt(pos.index);
        else if (pos.index == 0)
            return drawing::Shaper::empty_glyph;
        else
            return _shaper->glyphAt(pos.index - 1);
    }

    const drawing::glyph & TextBox::_nextGlyph(cursor pos) const
    {
        auto & glyph = _shaper->glyphAt(pos.index);
        if (!pos.far)
            return glyph;
        else if(glyph.trange.end() >= _text.length())
            return drawing::Shaper::empty_glyph;
        else
            return _shaper->glyphAt(glyph.trange.end());
    }

    cursor TextBox::_makeCursor(size_t index, bool far)
    {
        if (index == 0 || _text.empty())
            return { 0, false };
        if (index == _text.length())
            return { _text.length() - 1, true };

        size_t index2 = far ? index - 1 : index;
        auto & glyph = _shaper->findGlyph(index2);
        if(!glyph)
            return { 0, false };

        return { glyph.trange.index, far };
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
