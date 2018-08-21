#include "stdafx.h"
#include "ScrollBar.h"
#include "ui/renderables/Text.h"
#include "ui/interactables/MouseRectangle.h"

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

    void ScrollBar::updateContent(std::shared_ptr<component::View> & view)
    {
        core::rc32f bar_rect = barRect();

        if (!_mc_bar)
        {
            _mc_bar = std::make_shared<interactables::MouseRectangle>();
            _mc_bar->mouseEnter += std::weak_binder(std::mem_fn(&ScrollBar::onBarMouseEnter), shared_from_this());
            _mc_bar->mouseMove += std::weak_bind(&ScrollBar::onBarMouseMove, share_ref<ScrollBar>(), std::placeholders::_1);
            _mc_bar->mouseLeave += std::weak_bind(&ScrollBar::onBarMouseLeave, share_ref<ScrollBar>(), std::placeholders::_1);
            _mc_bar->mouseDown += std::weak_binder(std::mem_fn(&ScrollBar::onBarMouseDown), shared_from_this());
            _mc_bar->mouseUp += std::weak_binder(std::mem_fn(&ScrollBar::onBarMouseUp), shared_from_this());
            _mc_bar->setCaptureButtons(component::mouse_button::left);
            view->insert(_mc_bar);
        }
        _mc_bar->setRect(bar_rect);

        if(!_bar)
        {
            _bar = std::make_shared<renderables::Rectangle>();
            view->insert(_bar);
        }

        _bar->setRect(bar_rect);
        _bar->setRectangle(bar_rect);
        _bar->setPathStyle(graphics::PathStyle().fill(_bar_color));
    }


    std::string ScrollBar::styleName() const
    {
        bool mousein = false;
        bool pressed = false;
        if (_mc_bar)
        {
            mousein = _mc_bar->mousein();
            pressed = _mc_bar->pressed();
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
        if(!core::equal(_val, val))
        {
            _val = val;
            invalid();
        }
    }

    float32_t ScrollBar::barHeight() const
    {
        auto box = contentBox();
        return box.cy * pageValue() / rangeValue();
    }

    float32_t ScrollBar::barSpace() const
    {
        auto box = contentBox();
        return box.cy - box.cy * pageValue() / rangeValue();
    }

    core::rc32f ScrollBar::barRect() const
    {
        auto box = contentBox();
        float32_t bar_cy = box.cy * pageValue() / rangeValue();
        float32_t bar_y = (box.cy - bar_cy) * (_val / rangeValue());
        core::rc32f bar_rect = { box.x, bar_y, box.cx, bar_cy };
        return bar_rect;
    }

    void ScrollBar::onBarMouseEnter(const component::mosue_state & state)
    {
        updateStyle();
    }

    void ScrollBar::onBarMouseMove(const component::mosue_state & state)
    {
        if (_bar_drag_mouse_pos.empty())
            return;

        float32_t diff = state.pos().y - _bar_drag_mouse_pos.y;
        float32_t val = std::clamp(_bar_drag_start_vallue + rangeValue() * (diff / barSpace()), _min, _max);
        setValue(val);
    }

    void ScrollBar::onBarMouseLeave(const component::mosue_state & state)
    {
        _bar_drag_start_vallue = 0.0f;
        _bar_drag_mouse_pos = {};
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
        _bar_drag_mouse_pos = {};
        updateStyle();
    }

    void ScrollBar::onSizeChanged(const core::si32f & from, const core::si32f & to)
    {
        Control::onSizeChanged(from, to);
        updateStyle();
    }
}
