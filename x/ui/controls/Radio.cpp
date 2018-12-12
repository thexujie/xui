#include "stdafx.h"
#include "Radio.h"

namespace ui::controls
{
    Radio::Radio()
    {
    }

    Radio::~Radio()
    {
        
    }

    void Radio::propertyTableCallback(core::property_table & properties)
    {
        Control::propertyTableCallback(properties);
        properties["text"] = core::make_accessor(&Radio::setText, &Radio::text);
        properties["hole-color"] = core::make_accessor(&Radio::_setHoleColor, &Radio::_hole_color);
        properties["hole-border-size"] = core::make_accessor(&Radio::_setHoleBorderSize, &Radio::_hole_border_size);
        properties["hole-border-color"] = core::make_accessor(&Radio::_setHoleBorderColor, &Radio::_hole_border_color);
    }

    void Radio::propertyTable(core::property_table & properties)
    {
        Radio::propertyTableCallback(properties);
    }

    void Radio::setText(const std::string & text)
    {
        _text.setText(text);
        _text.update(font(), color());
    }

    core::sizef Radio::contentSize() const
    {
        core::sizef size = _text.bounds();
        size.cx += drawing::fontmetrics(font()).height + calc(_content_spacing);
        return size;
    }

    std::string Radio::styleName() const
    {
        if (_pressed)
            return "radio:active";
        else if (_mousein)
            return "radio:hover";
        else
            return "radio";
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

    void Radio::onMouseEnter(const input_state & state)
    {
        Control::onMouseEnter(state);
        restyle();
    }

    void Radio::onMouseMove(const input_state & state)
    {
        Control::onMouseMove(state);
    }

    void Radio::onMouseLeave(const input_state & state)
    {
        Control::onMouseLeave(state);
        restyle();
    }

    
    void Radio::onMouseDown(const input_state & state, ui::mouse_button RadioButton)
    {
        Control::onMouseDown(state, RadioButton);
        restyle();
    }

    void Radio::onMouseUp(const input_state & state, ui::mouse_button RadioButton)
    {
        Control::onMouseUp(state, RadioButton);
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
