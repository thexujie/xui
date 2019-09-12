#include "stdafx.h"
#include "TextLine.h"
#include <SkTextBlob.h>

namespace ui::controls
{
    static size_t textbox_u8_rfind_next(const std::u8string & str, size_t offset = core::npos)
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
        _cursor = cursor::ibeam;
		_ime_mode = ime_mode::on;
    }

    TextLine::~TextLine()
    {
        
    }

    void TextLine::propertyTableCallback(core::property_table & properties)
    {
        Control::propertyTableCallback(properties);
        properties[u8"text"] = core::make_accessor(&TextLine::setText, &TextLine::text);
		properties[u8"select-color"] = make_accessor(&TextLine::_setSelectColor, &TextLine::_color_select);
    }

    void TextLine::propertyTable(core::property_table & properties)
    {
        TextLine::propertyTableCallback(properties);
    }

    void TextLine::setText(const std::u8string & text)
    {
		_text = text;
		refresh();
    }

    std::u8string TextLine::styleName() const
    {
        if (_focused)
            return u8"textline:focused";
        else if (_hovered)
            return u8"textline:hover";
        else
            return u8"textline";
    }

	void TextLine::onEnterScene()
	{
		_text_object = scene()->createTextComponent();
		Control::onEnterScene();
	}

	void TextLine::onLeaveScene()
	{
		_text_object.reset();
		Control::onLeaveScene();
	}

	
    void TextLine::update()
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

		_text_object->update(_text, font(), color());

        if (!_text.empty())
        {
            auto & cluster = _text_object->findCluster(_cursor_far ? _cursor_pos - 1 : _cursor_pos);
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

                auto size = _text_object->bounds();
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
        repaint();
        setContentSize(_text_object->bounds());
    }

    void TextLine::paint(drawing::Graphics & graphics, const core::rectf & clip) const
    {
        auto cbox = contentBox();
        graphics.save();
        graphics.setClipRect(cbox);

        if(_cursor_pos_selected != core::npos && _color_select.visible())
        {
            size_t off = std::min(_cursor_pos, _cursor_pos_selected);
            size_t end = std::max(_cursor_pos, _cursor_pos_selected);
            core::rectf rect;
            do
            {
               std::tie(off, rect) = _text_object->textRect(off, end - off);
                if(!rect.empty())
                    graphics.drawRectangle({ (cbox.leftTop() + rect.leftTop()).offset(_scroll_pos, 0.0f), rect.size }, drawing::PathFormat()._fill(_color_select));
            }
            while (off != core::npos);
        }


        if (!_text.empty())
            graphics.drawText(*_text_object, contentBox().leftTop().offseted(_scroll_pos, 0), drawing::StringFormat()._color(color()));
        graphics.restore();

        if (_cursor_shown)
        {
            if(_text.empty())
            {
                auto fmetrics = scene()->graphicsService().font_metrics(font());
                graphics.drawRectangle({ cbox.leftTop(), { fmetrics.height * 0.5f, fmetrics.height } }, drawing::PathFormat()._fill(0x80ff0000));
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
                auto & cluster = _text_object->findCluster(cursor_tindex);
                if (cluster)
                {
                    if(cluster.bidi == drawing::bidirection::rtl)
                        cursor_left = !cursor_left;
                    core::rectf rect = cluster.rect;
                    rect.offset(cbox.leftTop()).offset(_scroll_pos, 0);
                    graphics.drawRectangle(rect, drawing::PathFormat()._fill(0x400000ff));
                    if (cursor_left)
                        graphics.drawLine(rect.leftTop(), rect.leftBottom(), drawing::PathFormat()._stoke(core::colors::Red, calc(1_px)));
                    else
                        graphics.drawLine(rect.rightTop(), rect.rightBottom(), drawing::PathFormat()._stoke(core::colors::Red, calc(1_px)));
                }
            }
        }
    }

    void TextLine::onSizeChanged(const core::sizef & from, const core::sizef & to)
    {
        update();
        updateIme();
        Control::onSizeChanged(from, to);
    }

    void TextLine::onHover(const input_state & state)
    {
        while (state.button(mouse_button::left))
        {
            auto cbox = contentBox();
            float32_t pos = state.pos().x - cbox.x - _scroll_pos;
            auto & cluster = _text_object->findCluster(pos);
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
                updateIme();
                repaint();
            }
            break;
        }
        Control::onHover(state);
    }

    void TextLine::onActiveIn(const input_state & state)
    {
        Control::onActiveIn(state);

        auto cbox = contentBox();
        float32_t pos = state.pos().x - cbox.x - _scroll_pos;
        auto & cluster = _text_object->findCluster(pos);
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
        repaint();
        updateIme();
    }

    void TextLine::onActiveOut(const input_state & state)
    {
        Control::onActiveOut(state);
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

    void TextLine::onFocusIn(const input_state & state)
    {
        if(!_cursor_anim)
        {
            auto accessor = make_accessor(&TextLine::_setCursorShown, &TextLine::_cursorShown);
            auto interpolator = std::make_shared<core::property_interpolator_keyframes<bool>>();
            interpolator->set_values({ { 0.0f, true }, { 0.5f, false } });
            _cursor_anim = std::make_shared<core::property_animation>(control_ref(), accessor, interpolator);
            _cursor_anim->setDuration(1s);
            _cursor_anim->setLoop(9999);
            appendAnimation(CONTROL_ANIMATION_GROUP_CONTROL, _cursor_anim);
        }
        _cursor_anim->start();
        restyle();

        if(auto ic = imeContext())
            ic->setImeMode(_ime_mode);
        updateIme();
    }

    void TextLine::onFocusOut(const input_state & state)
    {
        _setCursorShown(false);
        _cursor_anim ? _cursor_anim->stop() : nullptr;
        restyle();
    }

    void TextLine::onChar(char32_t ch)
    {
        if (ch <= 0xffff && std::iswcntrl((wint_t)ch))
            return;
        
        char8_t chars[6] = { 0 };
        size_t len = core::utf32_to_utf8(ch, chars, 6);
        insert(chars, len);
    }

	void TextLine::onAction(action_t action)
    {
	    switch(action)
	    {
		case system_action::cut:
			break;
		case system_action::copy:
			break;
		case system_action::paste:
			break;
	    }
    }

	void TextLine::onPopupMenu(const input_state & state, IMenuPresenter & presenter)
    {
		auto item_cut = std::make_shared<ui::MenuItem>(u8"icon.png", u8"ºÙ«–", ui::shortcut({ ui::keybind{ ui::keycode::ctrl, ui::keycode::X } }), system_action::cut, (_cursor_pos_selected != core::npos) ? item_flag::none : item_flag::disabled);
		auto item_copy = std::make_shared<ui::MenuItem>(u8"icon.png", u8"∏¥÷∆", ui::shortcut({ ui::keybind{ ui::keycode::ctrl, ui::keycode::C } }), system_action::copy, (_cursor_pos_selected != core::npos) ? item_flag::none : item_flag::disabled);
		auto item_paste = std::make_shared<ui::MenuItem>(u8"icon.png", u8"’≥Ã˘", ui::shortcut({ ui::keybind{ ui::keycode::ctrl, ui::keycode::V} }), system_action::paste);
		item_cut->active += [](auto action) { std::cout << "ºÙ«–"; };
		item_copy->active += [](auto action) { std::cout << "∏¥÷∆"; };
		item_paste->active += [](auto action) { std::cout << "’≥Ã˘"; };
		presenter.appendItems({ item_cut, item_copy, item_paste } );
    }

    void TextLine::caretLeft()
    {
        if (_cursor_pos < 1)
            return;

        auto & cluster = _text_object->findCluster(_cursor_pos - 1);
        if(!cluster)
            return;

        _cursor_far = false;
        _cursor_pos = cluster.trange.index;
        _cursor_anim ? _cursor_anim->reset() : nullptr;

        updateIme();
        repaint();
    }

    void TextLine::caretRight()
    {
        if (_cursor_pos >= _text.length())
            return;

        auto & cluster = _text_object->findCluster(_cursor_pos);
        if (!cluster)
            return;

        _cursor_far = true;
        _cursor_pos = cluster.trange.end();
        _cursor_anim ? _cursor_anim->reset() : nullptr;

        updateIme();
        repaint();
    }

    void TextLine::backSpace()
    {
        if (!_cursor_pos)
            return;

        auto & cluster = _text_object->findCluster(_cursor_pos - 1);
        if (!cluster)
            return;

        //////÷ª…æ≥˝◊Ó∫Û“ª∏ˆ glyph
        ////auto & glyph = _clusterizer->glyphAt(cluster.grange.end() - 1);
        ////if (!glyph)
        ////    return;

        ////_text.erase(glyph.trange.index, glyph.trange.length);
        _text.erase(cluster.trange.index, cluster.trange.length);
        _cursor_far = cluster.trange.index > 0;
        _cursor_pos = cluster.trange.index;
        _cursor_anim ? _cursor_anim->reset() : nullptr;

        update();
        updateIme();
    }

    void TextLine::del()
    {
        if (_cursor_pos >= _text.length())
            return;

        auto & cluster = _text_object->findCluster(_cursor_pos);
        if (!cluster)
            return;

        // …æ≥˝’˚∏ˆ cluster
        _text.erase(cluster.trange.index, cluster.trange.length);
        _cursor_far = cluster.trange.index > 0;
        _cursor_pos = cluster.trange.index;
        _cursor_anim ? _cursor_anim->reset() : nullptr;

        update();
        updateIme();
    }

    void TextLine::insert(const char8_t * text, size_t count)
    {
        _text.insert(_cursor_pos, text, count);

        //_cursor_far = _cursor_pos + count + 1 < _text.length();
        _cursor_far = true;
        _cursor_pos += count;
        _cursor_anim ? _cursor_anim->reset() : nullptr;

        update();
        updateIme();
    }

    void TextLine::updateIme()
    {
        if(_ime_mode != ime_mode::disabled)
        {
            auto cbox = contentBox();
            if (auto ic = imeContext())
            {
                if (_text.empty())
                    ic->setCompositionPos(cbox.leftTop().offseted(_scroll_pos, 0));
                else
                {
                    auto & cluster = _text_object->findCluster(_cursor_far ? _cursor_pos - 1 : _cursor_pos);
                    assert(cluster);
                    ic->setCompositionPos(cbox.leftTop().offseted(_cursor_far ? cluster.rect.right() : cluster.rect.left(), 0).offset(_scroll_pos, 0));
                }
                ic->setCompositionFont(font());
            }
        }
    }

    void TextLine::_docaret()
    {
        if (!_text.empty())
        {
            auto & cluster = _text_object->findCluster(_cursor_far ? _cursor_pos - 1 : _cursor_pos);
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

                auto size = _text_object->bounds();
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

        repaint();
    }

    void TextLine::_setCursorShown(bool vis)
    {
        if(_cursor_shown != vis)
        {
            _cursor_shown = vis;
            repaint();
        }
    }
}
