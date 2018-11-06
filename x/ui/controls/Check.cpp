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
        properties["text"] = core::make_accessor(&Check::setText, &Check::text, core::parseString, nullptr);
        properties["hole-color"] = core::make_accessor(&Check::_setHoleColor, &Check::_hole_color, core::parseColor, nullptr);
        properties["hole-border-size"] = core::make_accessor(&Check::_setHoleBorderSize, &Check::_hole_border_size, core::parseDimension, nullptr);
        properties["hole-border-color"] = core::make_accessor(&Check::_setHoleBorderColor, &Check::_hole_border_color, core::parseColor, nullptr);
    }

    void Check::propertyTable(core::property_table & properties)
    {
        Check::propertyTableCallback(properties);
    }

    void Check::setText(const std::string & text)
    {
        _textBlob = nullptr;
        _text = text;
    }

    core::sizef Check::contentSize() const
    {
        _confirmBlob();
        core::sizef size = _textBlob ? _textBlob->size() : core::sizef();
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

    void Check::draw(drawing::Graphics & graphics, const core::rectf & clip) const
    {
        auto box = paddingBox();
        auto fm = drawing::fontmetrics(font());
        _drawBackground(graphics);

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
            core::pt32f points[3];
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

        if (_textBlob)
            graphics.drawTextBlob(*_textBlob, contentBox().leftTop().offset(fm.height + calc(_content_spacing), 0), drawing::StringFormat().color(color()));
        _drawBorder(graphics);
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

    void Check::_confirmBlob() const
    {
        if (!_textBlob)
        {
            drawing::StringFormat format(font());
            format.color(color());
            _textBlob = std::make_shared<drawing::TextBlob>(_text, format);
        }
    }

    void Check::_setHoleColor(core::color color)
    {
        if(_hole_color != color)
        {
            _hole_color = color;
            refresh();
        }
    }

    void Check::_setHoleBorderSize(core::dimensionf border_size)
    {
        if (_hole_border_size != border_size)
        {
            _hole_border_size = border_size;
            refresh();
        }
    }

    void Check::_setHoleBorderColor(core::color color)
    {
        if (_hole_border_color != color)
        {
            _hole_border_color = color;
            refresh();
        }
    }
}
