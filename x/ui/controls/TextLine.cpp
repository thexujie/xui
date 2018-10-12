#include "stdafx.h"
#include "TextLine.h"
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

    TextLine::TextLine()
    {
        _accept_input = true;
        _capture_buttons = mouse_button::left;
        _clusterizer = std::make_shared<drawing::TextClusterizer>();
        _cursor = cursor::ibeam;
    }

    TextLine::~TextLine()
    {
        
    }

    void TextLine::propertyTableCallback(core::property_table & properties)
    {
        Control::propertyTableCallback(properties);
        properties["text"] = core::make_accessor(&TextLine::setText, &TextLine::text, core::parseString, nullptr);
    }

    void TextLine::propertyTable(core::property_table & properties)
    {
        TextLine::propertyTableCallback(properties);
    }

    void TextLine::setText(const std::string & text)
    {
        _text = text;
        reshaper(shaper_flag::shaper);
    }

    core::si32f TextLine::contentSize() const
    {
        return { 0.0f, drawing::fontmetrics(font()).height };
    }

    std::string TextLine::styleName() const
    {
        if (_pressed)
            return "textline:active";
        else if (_mousein)
            return "textline:hover";
        else
            return "textline";
    }

    void TextLine::draw(drawing::Graphics & graphics, const drawing::Region & region) const
    {
        auto cbox = contentBox();
        std::lock_guard l(*this);
        _drawBackground(graphics);
        graphics.save();
        graphics.setClipRect(cbox);

        if(_cursor_pos_selected != core::npos)
        {
            size_t off = std::min(_cursor_pos, _cursor_pos_selected);
            size_t end = std::max(_cursor_pos, _cursor_pos_selected);
            core::rc32f rect;
            do
            {
               std::tie(off, rect) = _clusterizer->textRect(off, end - off);
                if(!rect.empty())
                    graphics.drawRectangle({ (cbox.leftTop() + rect.leftTop()).offset(_scroll_pos, 0.0f), rect.size }, drawing::PathStyle().fill(0x800000ff));
            }
            while (off != core::npos);
        }

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
                size_t cursor_tindex = _cursor_pos;
                bool cursor_left = false;
                if(cursor_tindex == _text.length())
                {
                    cursor_tindex = _text.length() - 1;
                    cursor_left = false;
                }
                else if(!cursor_tindex)
                {
                    cursor_tindex = 0;
                    cursor_left = true;
                }
                else if(_cursor_far)
                {
                    cursor_tindex = cursor_tindex - 1;
                    cursor_left = false;
                }
                else
                {
                    cursor_tindex = _cursor_pos;
                    cursor_left = !_cursor_far;
                }
                auto & cluster = _clusterizer->findCluster(cursor_tindex);
                if (cluster)
                {
                    if(cluster.bidi == drawing::bidirection::rtl)
                        cursor_left = !cursor_left;
                    core::rc32f rect = cluster.rect;
                    rect.offset(cbox.leftTop()).offset(_scroll_pos, 0);
                    graphics.drawRectangle(rect, drawing::PathStyle().fill(0x400000ff));
                    if (cursor_left)
                        graphics.drawLine(rect.leftTop(), rect.leftBottom(), drawing::PathStyle().stoke(core::colors::Red, calc(1_px)));
                    else
                        graphics.drawLine(rect.rightTop(), rect.rightBottom(), drawing::PathStyle().stoke(core::colors::Red, calc(1_px)));
                }
            }
        }
        _drawBorder(graphics);
    }

    void TextLine::onSizeChanged(const core::si32f & from, const core::si32f & to)
    {
        reshaper(shaper_flag::caret);
        Control::onSizeChanged(from, to);
    }

    void TextLine::onMouseEnter(const input_state & state)
    {
        Control::onMouseEnter(state);
        restyle();
    }

    void TextLine::onMouseMove(const input_state & state)
    {
        while (state.button(mouse_button::left))
        {
            auto cbox = contentBox();
            float32_t pos = state.pos().x - cbox.x - _scroll_pos;
            auto & cluster = _clusterizer->findCluster(pos);
            if (!cluster)
                break;

            size_t cursor_pos = ((pos >= cluster.rect.centerX()) ^ (cluster.bidi == drawing::bidirection::rtl)) ? cluster.trange.end() : cluster.trange.index;
            if (cursor_pos == _cursor_pos)
                break;

            if (_cursor_pos_selected == core::npos)
                _cursor_pos_selected = _cursor_pos;

            bool cursor_far = cursor_pos > _cursor_pos;
            if ((cursor_far ^ (cluster.bidi == drawing::bidirection::rtl)) ^ (pos < cluster.rect.centerX()))
            {
                _cursor_far = cursor_far; 
                _cursor_pos = _cursor_far ? cluster.trange.end() : cluster.trange.index;
                _cursor_anim ? _cursor_anim->reset() : nullptr;
                reshaper(shaper_flag::caret);
            }
            break;
        }
        Control::onMouseMove(state);
    }

    void TextLine::onMouseLeave(const input_state & state)
    {
        Control::onMouseLeave(state);
        restyle();
    }

    void TextLine::onMouseDown(const input_state & state, ui::mouse_button button)
    {
        Control::onMouseDown(state, button);
        auto cbox = contentBox();
        float32_t pos = state.pos().x - cbox.x - _scroll_pos;
        auto & cluster = _clusterizer->findCluster(pos);
        if (!cluster)
            return;

        if(state.key(ui::keycode::shift))
        {
            if (_cursor_pos_selected == core::npos)
                _cursor_pos_selected = _cursor_pos;
        }
        else
            _cursor_pos_selected = core::npos;

        _cursor_far = (pos >= cluster.rect.centerX()) ^ (cluster.bidi == drawing::bidirection::rtl);
        _cursor_pos = _cursor_far ? cluster.trange.end() : cluster.trange.index;
        _cursor_anim ? _cursor_anim->reset() : nullptr;
        reshaper(shaper_flag::caret);
        restyle();
    }

    void TextLine::onMouseUp(const input_state & state, ui::mouse_button button)
    {
        Control::onMouseUp(state, button);
        restyle();
    }

    void TextLine::onKeyDown(const input_state & state, keycode key)
    {
        Control::onKeyDown(state, key);
        switch(key)
        {
        case keycode::left:
            if (state.key(ui::keycode::shift))
            {
                if (_cursor_pos_selected == core::npos)
                    _cursor_pos_selected = _cursor_pos;
            }
            else
                _cursor_pos_selected = core::npos;
            caretLeft();
            break;
        case keycode::right:
            if (state.key(ui::keycode::shift))
            {
                if (_cursor_pos_selected == core::npos)
                    _cursor_pos_selected = _cursor_pos;
            }
            else
                _cursor_pos_selected = core::npos;
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

    void TextLine::onFocus(std::shared_ptr<ImeContext> imecontext)
    {
        _imecontext = imecontext;
        if(!_cursor_anim)
        {
            auto accessor = make_accessor(&TextLine::_setCursorShown, &TextLine::_cursorShown, core::parseBool, nullptr);
            auto interpolator = std::make_shared<core::property_interpolator_keyframes<bool>>();
            interpolator->set_values({ { 0.0f, true }, { 0.5f, false } });
            _cursor_anim = std::make_shared<core::property_animation>(control_ref(), accessor, interpolator);
            _cursor_anim->setDuration(1s);
            _cursor_anim->setLoop(9999);
            appendAnimation(CONTROL_ANIMATION_GROUP_CONTROL, _cursor_anim);
        }
        _cursor_anim->start();
        restyle();

        if(_imecontext)
            _imecontext->setImeMode(_ime_mode);
        _updateIme();
    }

    void TextLine::onBlur()
    {
        _imecontext = nullptr;
        //std::lock_guard lock(*this);
        _setCursorShown(false);
        _cursor_anim ? _cursor_anim->stop() : nullptr;
        restyle();
    }

    void TextLine::onChar(char32_t ch)
    {
        if (ch <= 0xffff && std::iswcntrl((wint_t)ch))
            return;
        
        char chars[6] = { 0 };
        size_t len = core::utf32_to_utf8(ch, chars, 6);
        insert(chars, len);
    }

    void TextLine::reshaper(shaper_flags flags)
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
            invoke([this]() { _doshaper(); _updateIme(); });
        }
    }

    void TextLine::caretLeft()
    {
        if (_cursor_pos < 1)
            return;

        if (_delay_shaper_flags)
            _doshaper();

        auto & cluster = _clusterizer->findCluster(_cursor_pos - 1);
        if(!cluster)
            return;

        _cursor_far = false;
        _cursor_pos = cluster.trange.index;
        _cursor_anim ? _cursor_anim->reset() : nullptr;
        reshaper(shaper_flag::caret);

    }

    void TextLine::caretRight()
    {
        if (_cursor_pos >= _text.length())
            return;

        if (_delay_shaper_flags)
            _doshaper();

        auto & cluster = _clusterizer->findCluster(_cursor_pos);
        if (!cluster)
            return;

        _cursor_far = true;
        _cursor_pos = cluster.trange.end();
        _cursor_anim ? _cursor_anim->reset() : nullptr;
        reshaper(shaper_flag::caret);
    }

    void TextLine::backSpace()
    {
        if (!_cursor_pos)
            return;

        if (_delay_shaper_flags)
            _doshaper();

        auto & cluster = _clusterizer->findCluster(_cursor_pos - 1);
        if (!cluster)
            return;

        //////只删除最后一个 glyph
        ////auto & glyph = _clusterizer->glyphAt(cluster.grange.end() - 1);
        ////if (!glyph)
        ////    return;

        ////_text.erase(glyph.trange.index, glyph.trange.length);
        _text.erase(cluster.trange.index, cluster.trange.length);
        _cursor_far = cluster.trange.index > 0;
        _cursor_pos = cluster.trange.index;
        _cursor_anim ? _cursor_anim->reset() : nullptr;
        reshaper(shaper_flag::shaper);
    }

    void TextLine::del()
    {
        if (_cursor_pos >= _text.length())
            return;

        if (_delay_shaper_flags)
            _doshaper();

        auto & cluster = _clusterizer->findCluster(_cursor_pos);
        if (!cluster)
            return;

        // 删除整个 cluster
        _text.erase(cluster.trange.index, cluster.trange.length);
        _cursor_far = cluster.trange.index > 0;
        _cursor_pos = cluster.trange.index;
        _cursor_anim ? _cursor_anim->reset() : nullptr;

        reshaper(shaper_flag::shaper);
    }

    void TextLine::insert(const char * text, size_t count)
    {
        _text.insert(_cursor_pos, text, count);
        //_cursor_far = _cursor_pos + count + 1 < _text.length();
        _cursor_far = true;
        _cursor_pos += count;
        _cursor_anim ? _cursor_anim->reset() : nullptr;
        reshaper(shaper_flag::shaper);
    }

    void TextLine::_updateIme()
    {
        if (!_imecontext)
            return;

        if(_ime_mode != ime_mode::disabled)
        {
            auto cbox = contentBox();
            if(_text.empty())
                _imecontext->setCompositionPos(cbox.leftTop().offseted(_scroll_pos, 0));
            else
            {
                auto & cluster = _clusterizer->findCluster(_cursor_far ? _cursor_pos - 1 : _cursor_pos);
                assert(cluster);
                _imecontext->setCompositionPos(cbox.leftTop().offseted(_cursor_far ? cluster.rect.right() : cluster.rect.left(), 0).offset(_scroll_pos, 0));
            }
            _imecontext->setCompositionFont(font()); 
        }
    }

    void TextLine::_doshaper()
    {
        std::lock_guard l(*this);
        _delay_shaper = false;

        if(_delay_shaper_flags.any(shaper_flag::shaper))
        {
            if (!_textblob)
                _textblob = std::make_shared<drawing::TextBlob>();

            _clusterizer->itermize(_text, _font, _color);
            _clusterizer->layout();

            auto native = _clusterizer->build();
            _textblob->setNative(native, _clusterizer->bounds());
        }

        if (!_text.empty())
        {
            auto & cluster = _clusterizer->findCluster(_cursor_far ? _cursor_pos - 1 : _cursor_pos);
            assert(cluster);
            if (cluster)
            {
                auto cbox = contentBox();
                float32_t scroll_pos = _scroll_pos;
                if (_scroll_pos + cluster.rect.x < 0)
                    scroll_pos = -cluster.rect.x;
                else if (_scroll_pos + cluster.rect.right() >= cbox.width)
                    scroll_pos = cbox.cx - cluster.rect.right();
                else {}

                auto size = _clusterizer->bounds();
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
        _delay_shaper_flags.clear();
    }

    void TextLine::_docaret()
    {
        if (!_text.empty())
        {
            auto & cluster = _clusterizer->findCluster(_cursor_far ? _cursor_pos - 1 : _cursor_pos);
            assert(cluster);
            if (cluster)
            {
                auto cbox = contentBox();
                float32_t scroll_pos = _scroll_pos;
                if (_scroll_pos + cluster.rect.x < 0)
                    scroll_pos = -cluster.rect.x;
                else if (_scroll_pos + cluster.rect.right() >= cbox.width)
                    scroll_pos = cbox.cx - cluster.rect.right();
                else {}

                auto size = _clusterizer->bounds();
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
    }

    void TextLine::_setCursorShown(bool vis)
    {
        if(_cursor_shown != vis)
        {
            _cursor_shown = vis;
            refresh();
        }
    }
}
