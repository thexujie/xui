#include "stdafx.h"
#include "Check.h"

namespace ui::controls
{
    Check::Check()
    {
    }

    Check::Check(std::string text) : _text(text)
    {
    }

    Check::~Check()
    {
        
    }

    void Check::propertyTableCallback(core::property_table & properties)
    {
        Control::propertyTableCallback(properties);
        properties["text"] = core::make_accessor(&Check::setText, &Check::text);
        properties["hole-color"] = core::make_accessor(&Check::_setHoleColor, &Check::_hole_color);
        properties["hole-border-size"] = core::make_accessor(&Check::_setHoleBorderSize, &Check::_hole_border_size);
        properties["hole-border-color"] = core::make_accessor(&Check::_setHoleBorderColor, &Check::_hole_border_color);
    }

    void Check::propertyTable(core::property_table & properties)
    {
        Check::propertyTableCallback(properties);
    }

    void Check::setText(const std::string & text)
    {
        _text.setText(text);
        _text.update(font(), color());
    }

    core::sizef Check::contentSize() const
    {
        core::sizef size = _text.bounds();
        size.cx += drawing::fontmetrics(font()).height + calc(_content_spacing);
        return size;
    }

    std::string Check::styleName() const
    {
        if (_pressed)
            return "radio:active";
        else if (_mousein)
            return "radio:hover";
        else
            return "radio";
    }

    void Check::paint(drawing::Graphics & graphics, const core::rectf & clip) const
    {
        auto box = paddingBox();
        auto fm = drawing::fontmetrics(font());

        core::rectf rc_hole(box.leftTop(), { fm.height, fm.height });
        rc_hole.expand(-calc(_hole_border_size) * 0.5f);
        if (_hole_color.visible())
            graphics.drawRectangle(rc_hole, drawing::PathStyle().fill(_hole_color));
        if (_hole_border_color.visible())
            graphics.drawRectangle(rc_hole, drawing::PathStyle().stoke(_hole_border_color, calc(_hole_border_size)));

        if(_state == check_state::checked)
        {
            auto lw = 2_px;
            rc_hole.expand(-calc(lw) * 1.5f);
            core::pointf points[3];
            points[0] = rc_hole.leftBorder(0.5f);
            points[1] = rc_hole.bottomBorder(0.4f);
            points[2] = rc_hole.rightBorder(0.1f);

            if (_dot_border_color.visible())
                graphics.drawPoints(points, 3, drawing::point_mode::polygon, drawing::PathStyle().stoke(_dot_border_color, calc(lw)).join(drawing::join_style::round));
        }
        else if (_state == check_state::unknown)
        {
            core::rectf rc_dot = rc_hole.expanded(-fm.height * 0.2f);
            if (_dot_color.visible())
                graphics.drawRectangle(rc_dot, drawing::PathStyle().fill(_dot_color));
            if (_dot_border_color.visible())
                graphics.drawRectangle(rc_dot, drawing::PathStyle().stoke(_dot_border_color, calc(_dot_border_size)));
        }
        else {}

        graphics.drawText(_text, contentBox().leftTop().offset(fm.height + calc(_content_spacing), 0), drawing::StringFormat().color(color()));
    }

    void Check::onMouseEnter(const input_state & state)
    {
        Control::onMouseEnter(state);
        restyle();
    }

    void Check::onMouseMove(const input_state & state)
    {
        Control::onMouseMove(state);
    }

    void Check::onMouseLeave(const input_state & state)
    {
        Control::onMouseLeave(state);
        restyle();
    }

    
    void Check::onMouseDown(const input_state & state, ui::mouse_button button)
    {
        Control::onMouseDown(state, button);
        restyle();
    }

    void Check::onMouseUp(const input_state & state, ui::mouse_button button)
    {
        Control::onMouseUp(state, button);
        restyle();
        setCheckState(_state != check_state::checked ? check_state::checked : check_state::unchecked);
    }

    void Check::_setHoleColor(core::color color)
    {
        if(_hole_color != color)
        {
            _hole_color = color;
            repaint();
        }
    }

    void Check::_setHoleBorderSize(core::dimenf border_size)
    {
        if (_hole_border_size != border_size)
        {
            _hole_border_size = border_size;
            repaint();
        }
    }

    void Check::_setHoleBorderColor(core::color color)
    {
        if (_hole_border_color != color)
        {
            _hole_border_color = color;
            repaint();
        }
    }
}
