#include "stdafx.h"
#include "ScrollBar.h"

namespace ui::controls
{
    ScrollBar::ScrollBar()
    {
        _border = core::vec4<core::dimensionf>{ 1_px };
        _padding = {1_em, 0.5_em};
        _accept_wheel_v = true;
        _capture_buttons = mouse_button::left;
    }

    ScrollBar::~ScrollBar()
    {
        
    }

    void ScrollBar::propertyTableCallback(core::property_table & properties)
    {
        Control::propertyTableCallback(properties);
        properties["bar-color"] = make_accessor(&ScrollBar::setBarColor, &ScrollBar::barColor, core::parseColor, nullptr);
        properties["bar-border"] = make_accessor(&ScrollBar::setBarBorder, &ScrollBar::barBorder, core::parseDimension, nullptr);
        properties["bar-border-color"] = make_accessor(&ScrollBar::setBarBorderColor, &ScrollBar::barBorderColor, core::parseColor, nullptr);
        properties["bar-border-style"] = make_accessor(&ScrollBar::setBarBorderStyle, &ScrollBar::barBorderStyle, drawing::parseStrokeStyle, nullptr);
    }

    void ScrollBar::propertyTable(core::property_table & properties)
    {
        ScrollBar::propertyTableCallback(properties);
    }

    std::string ScrollBar::styleName() const
    {
        if (_bar_active)
            return "scrollbar:bar-active";
        else if (_bar_hover)
            return "scrollbar:bar-hover";
        else
            return "scrollbar";
    }

    void ScrollBar::draw(drawing::Graphics & graphics, const core::rectf & clip) const
    {
        core::rectf bar_rect = barRect();
        graphics.drawRectangle(bar_rect, drawing::PathStyle().fill(_bar_color));
    }

    void ScrollBar::setValue(float32_t val)
    {
        val = std::clamp(val, _min, _max);
        if(!core::equal(_val, val))
        {
            auto old_val = _val;
            _val = val;
            refresh();
            valueChagned(old_val, val);
        }
    }

    void ScrollBar::setValues(float32_t val, float32_t min, float32_t max)
    {
        if (min > max)
            max = min;
        val = std::clamp(val, min, max);
        if (!core::equal(_val, val) || !core::equal(_min, min) || !core::equal(_max, max))
        {
            if (!core::equal(_val, val))
            {
                auto old_val = _val;
                _val = val;
                valueChagned(old_val, val);
            }
            _min = min;
            _max = max;
            refresh();
        }
    }

    float32_t ScrollBar::barSize() const
    {
        auto box = controlBox();
        if (rangeValue() <= 0)
            return 0;
        else if(_direction == core::align::top)
            return box.cy * barRate();
        else
            return box.cx * barRate();
    }

    float32_t ScrollBar::barSpace() const
    {
        auto box = controlBox();
        if (rangeValue() <= 0)
            return 0;
        else if (_direction == core::align::top)
            return box.cy * (1.0f - barRate());
        else
            return box.cx * (1.0f - barRate());
    }

    core::rectf ScrollBar::barRect() const
    {
        auto box = controlBox();
        if (rangeValue() <= 0)
            return { box.x, box.y, 0.0f, 0.0f };
        if (_direction == core::align::top)
            return { box.x, box.y + barPos(), box.cx, barSize() };
        else
            return { box.x + barPos(), box.y, barSize(), box.cy };
    }

    void ScrollBar::onMouseEnter(const input_state & state)
    {
        _updateMouse(state);
        Control::onMouseEnter(state);
    }

    void ScrollBar::onMouseMove(const input_state & state)
    {
        _updateMouse(state);
        Control::onMouseMove(state);
    }

    void ScrollBar::onMouseLeave(const input_state & state)
    {
        _bar_drag_start_vallue = 0.0f;
        _bar_drag_mouse_pos = { std::nanf("0"), std::nanf("0") };
        _updateBarState(false, false);
        Control::onMouseLeave(state);
    }

    void ScrollBar::onMouseDown(const input_state & state, mouse_button button)
    {
        if(_bar_hover)
        {
            _bar_drag_start_vallue = _val;
            _bar_drag_mouse_pos = state.pos();
            _updateBarState(true, true);
        }
        Control::onMouseDown(state, button);
    }

    void ScrollBar::onMouseUp(const input_state & state, mouse_button button)
    {
        if (_bar_active)
        {
            _bar_drag_start_vallue = 0.0f;
            _bar_drag_mouse_pos = { std::nanf("0"), std::nanf("0") };
            _updateBarState(true, false);
            _updateMouse(state);
        }
        Control::onMouseUp(state, button);
    }

    void ScrollBar::onMouseWheel(const input_state & state)
    {
        float32_t val = _val - lineValue() * state.wheelLines();
        setValue(std::clamp(val, _min, _max));
        Control::onMouseWheel(state);
    }

    void ScrollBar::onSizeChanged(const core::sizef & from, const core::sizef & to)
    {
        Control::onSizeChanged(from, to);
        restyle();
    }

    void ScrollBar::_updateMouse(const input_state & state)
    {
        // ÕýÔÚÍÏ¶¯
        if(_bar_active)
        {
            if (std::isnan(_bar_drag_mouse_pos.x))
                return;

            float32_t diff = _direction == core::align::top ? state.pos().y - _bar_drag_mouse_pos.y : state.pos().x - _bar_drag_mouse_pos.x;
            float32_t val = _bar_drag_start_vallue + rangeValue() * (diff / barSpace());
            setValue(std::clamp(val, _min, _max));
        }
        else
        {
            core::rectf bar_rect = barRect();
            _updateBarState(bar_rect.contains(state.pos()), false);
        }
    }

    void ScrollBar::_updateBarState(bool hover, bool active)
    {
        if (hover != _bar_hover || active != _bar_active)
        {
            _bar_hover = hover;
            _bar_active = active;
            restyle();
        }
    }
}
