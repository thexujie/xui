#include "stdafx.h"
#include "ScrollBar.h"
#include "ui/renderables/Text.h"

namespace ui::controls
{
    ScrollBar::ScrollBar()
    {
        _border = core::vec4<core::dimensionf>{ 1_px };
        _padding = {1_em, 0.5_em};
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
        properties["bar-border-style"] = make_accessor(&ScrollBar::setBarBorderStyle, &ScrollBar::barBorderStyle, graphics::parseStrokeStyle, nullptr);
    }

    void ScrollBar::propertyTable(core::property_table & properties)
    {
        ScrollBar::propertyTableCallback(properties);
    }

    void ScrollBar::updateContent()
    {
        core::rc32f bar_rect = barRect();

        if (!_input)
        {
            _input = std::make_shared<component::Interactable>(control_ref());
            _input->mouseWheel += std::weak_bind(&ScrollBar::onMouseWheel, share_ref<ScrollBar>(), std::placeholders::_1);
            _input->setAcceptWheelV(true);
            insert(_input);
        }
        _input->setRect(box());

        if (!_input_bar)
        {
            _input_bar = std::make_shared<component::Interactable>(control_ref());
            _input_bar->mouseEnter += std::weak_bind(&ScrollBar::onBarMouseEnter, share_ref<ScrollBar>(), std::placeholders::_1);
            _input_bar->mouseMove += std::weak_bind(&ScrollBar::onBarMouseMove, share_ref<ScrollBar>(), std::placeholders::_1);
            _input_bar->mouseLeave += std::weak_bind(&ScrollBar::onBarMouseLeave, share_ref<ScrollBar>(), std::placeholders::_1);
            _input_bar->mouseDown += std::weak_bind(&ScrollBar::onBarMouseDown, share_ref<ScrollBar>(), std::placeholders::_1);
            _input_bar->mouseUp += std::weak_bind(&ScrollBar::onBarMouseUp, share_ref<ScrollBar>(), std::placeholders::_1);
            _input_bar->setCaptureButtons(component::mouse_button::left);
            insert(_input_bar);
        }
        _input_bar->setRect(bar_rect);

        if(!_bar)
        {
            _bar = std::make_shared<renderables::Rectangle>(control_ref());
            insert(_bar);
        }

        _bar->setRect(bar_rect);
        _bar->setRectangle(bar_rect);
        _bar->setPathStyle(graphics::PathStyle().fill(_bar_color));
    }


    std::string ScrollBar::styleName() const
    {
        bool mousein = false;
        bool pressed = false;
        if (_input_bar)
        {
            mousein = _input_bar->mousein();
            pressed = _input_bar->pressed();
        }

        if (pressed)
            return "scrollbar:bar-active";
        else if (mousein)
            return "scrollbar:bar-hover";
        else
            return "scrollbar";
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

    core::rc32f ScrollBar::barRect() const
    {
        auto box = controlBox();
        if (rangeValue() <= 0)
            return { box.x, box.y, 0.0f, 0.0f };
        if (_direction == core::align::top)
            return { box.x, box.y + barPos(), box.cx, barSize() };
        else
            return { box.x + barPos(), box.y, barSize(), box.cy };
    }

    void ScrollBar::onBarMouseEnter(const component::mosue_state & state)
    {
        updateStyle();
    }

    void ScrollBar::onBarMouseMove(const component::mosue_state & state)
    {
        if (std::isnan(_bar_drag_mouse_pos.x))
            return;

        float32_t diff = _direction == core::align::top ? state.pos().y - _bar_drag_mouse_pos.y : state.pos().x - _bar_drag_mouse_pos.x;
        float32_t val = _bar_drag_start_vallue + rangeValue() * (diff / barSpace());
        setValue(std::clamp(val, _min, _max));
    }

    void ScrollBar::onBarMouseLeave(const component::mosue_state & state)
    {
        _bar_drag_start_vallue = 0.0f;
        _bar_drag_mouse_pos = {std::nanf("0")};
        updateStyle();
    }

    
    void ScrollBar::onBarMouseDown(const component::mosue_state & state)
    {
        _bar_drag_start_vallue = _val;
        _bar_drag_mouse_pos = state.pos();
        updateStyle();
    }

    void ScrollBar::onBarMouseUp(const component::mosue_state & state)
    {
        _bar_drag_start_vallue = 0.0f;
        _bar_drag_mouse_pos = { std::nanf("0") };
        updateStyle();
    }

    void ScrollBar::onMouseWheel(const component::mosue_state & state)
    {
        float32_t val = _val - lineValue() * state.wheelLines();
        setValue(std::clamp(val, _min, _max));
    }

    void ScrollBar::onSizeChanged(const core::si32f & from, const core::si32f & to)
    {
        Control::onSizeChanged(from, to);
        updateStyle();
    }
}
