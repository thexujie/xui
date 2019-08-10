#include "stdafx.h"
#include "Radio.h"

namespace ui::controls
{
    Radio::Radio()
    {
    }

	Radio::Radio(const std::u8string & text): _text(text)
	{
	}

    Radio::~Radio()
    {
        
    }

    void Radio::propertyTableCallback(core::property_table & properties)
    {
        Control::propertyTableCallback(properties);
        properties[u8"text"] = core::make_accessor(&Radio::setText, &Radio::text);
        properties[u8"hole-color"] = core::make_accessor(&Radio::_setHoleColor, &Radio::_hole_color);
        properties[u8"hole-border-size"] = core::make_accessor(&Radio::_setHoleBorderSize, &Radio::_hole_border_size);
        properties[u8"hole-border-color"] = core::make_accessor(&Radio::_setHoleBorderColor, &Radio::_hole_border_color);
    }

    void Radio::propertyTable(core::property_table & properties)
    {
        Radio::propertyTableCallback(properties);
    }

    void Radio::setText(const std::u8string & text)
    {
        _text.setText(text);
        refresh();
    }

    std::u8string Radio::styleName() const
    {
        if (_actived)
            return u8"radio:active";
        else if (_hovered)
            return u8"radio:hover";
        else
            return u8"radio";
    }

    void Radio::update()
    {
        _text.update(font(), color());
        core::sizef size = _text.bounds();
        size.cx += drawing::fontmetrics(font()).height + calc(_content_spacing);
        setContentSize(size);
    }

    void Radio::paint(drawing::Graphics & graphics, const core::rectf & clip) const
    {
        auto box = paddingBox();
        auto fm = drawing::fontmetrics(font());

        core::rectf rc_hole(box.leftTop(), { fm.height, fm.height });
        rc_hole.expand(-calc(_hole_border_size) * 0.5f);
        if (_hole_color.visible())
            graphics.drawEllipse(rc_hole, drawing::PathStyle().fill(_hole_color));
        if (_hole_border_color.visible())
            graphics.drawEllipse(rc_hole, drawing::PathStyle().stoke(_hole_border_color, calc(_hole_border_size)));

        if(_state == check_state::checked)
        {
            core::rectf rc_dot = rc_hole.expanded(-fm.height * 0.2f);
            if (_dot_color.visible())
                graphics.drawEllipse(rc_dot, drawing::PathStyle().fill(_dot_color));
            if (_dot_border_color.visible())
                graphics.drawEllipse(rc_dot, drawing::PathStyle().stoke(_dot_border_color, calc(_dot_border_size)));
        }

        graphics.drawText(_text, contentBox().leftTop().offset(fm.height + calc(_content_spacing), 0), drawing::StringFormat().color(color()));
    }

    void Radio::onActiveOut(const input_state & state)
    {
        Control::onActiveOut(state);
        restyle();
        if(_state != check_state::checked)
            setCheckState(check_state::checked);
    }

    void Radio::_setHoleColor(core::color color)
    {
        if(_hole_color != color)
        {
            _hole_color = color;
            repaint();
        }
    }

    void Radio::_setHoleBorderSize(core::dimenf border_size)
    {
        if (_hole_border_size != border_size)
        {
            _hole_border_size = border_size;
            repaint();
        }
    }

    void Radio::_setHoleBorderColor(core::color color)
    {
        if (_hole_border_color != color)
        {
            _hole_border_color = color;
            repaint();
        }
    }
}