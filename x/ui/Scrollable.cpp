#include "stdafx.h"
#include "Scrollable.h"
#include "controls/ScrollBar.h"

namespace ui
{
    Scrollable::Scrollable()
    {
        _interactable = false;
    }

    Scrollable::~Scrollable()
    {
        
    }

    void Scrollable::setLayoutDirection(core::align layout)
    {
        _layout_direction = layout;
    }

    core::si32f Scrollable::contentSize() const
    {
        core::si32f size;
        float32_t margin = 0;
        for (auto & iter : _controls)
        {
            auto & control = iter.second;
            auto lo = control->layoutOrigin();
            if (lo != layout_origin::layout && lo != layout_origin::sticky)
                continue;

            auto m = control->realMargin();

            if(_layout_direction == core::align::left || _layout_direction == core::align::right)
            {
                margin = std::max(margin, _layout_direction == core::align::left ? m.bleft : m.bright);
                size.cx += margin;
                auto psize = control->expectSize();
                margin = _layout_direction == core::align::left ? m.bright : m.bleft;
                size.cx = size.cx + psize.cx;
                size.cy = std::max(size.cy, psize.cy + m.bheight());
            }
            else if (_layout_direction == core::align::top || _layout_direction == core::align::bottom)
            {
                margin = std::max(margin, _layout_direction == core::align::top ? m.btop : m.bbottom);
                size.cy += margin;
                auto psize = control->expectSize();
                margin = _layout_direction == core::align::top ? m.bbottom : m.btop;
                size.cy += psize.cy;
                size.cx = std::max(size.cx, psize.cx + m.bwidth());
            }
            else {}
        }
        return size;
    }

    void Scrollable::layout(layout_flags flags)
    {
        if (!_invalid_layout && !flags.any(layout_flag::force))
            return;

        float32_t margin = 0;
        core::si32f fitting_size = expectSize();
        core::rc32f box = paddingBox();
        float32_t layout_pos = 0;
        core::si32f layout_size;
        switch(_layout_direction)
        {
        case core::align::left:
            layout_pos = _rect.x;
            if (_scrollbar_h)
                layout_pos -= _scrollbar_h->value();
            break;
        case core::align::top:
            layout_pos = _rect.y;
            if (_scrollbar_v)
                layout_pos -= _scrollbar_v->value();
            break;
        default:
            break;
        }
        std::set<std::string> setes;
        for (auto & iter : _controls)
        {
            auto& control = iter.second;
            auto margins = control->realMargin();
            auto lo = control->layoutOrigin();
            auto control_size = control->prefferSize(calc_flag::none);

            if (lo == layout_origin::layout || lo == layout_origin::sticky)
            {
                switch (_layout_direction)
                {
                case core::align::left:
                    margin = std::max(margin, margins.bleft);
                    control->place({ layout_pos + layout_size.cx + margin, box.y + margins.btop, control_size.cx, fitting_size.cy - margins.bheight() }, control_size);
                    layout_size.cx += margin + control_size.cx;
                    margin = margins.bright;
                    break;
                case core::align::top:
                    margin = std::max(margin, margins.btop);
                    control->place({ box.x + margins.bleft, layout_pos + layout_size.cy + margin, fitting_size.cx - margins.bwidth(), control_size.cy }, control_size);
                    layout_size.cy += margin + control_size.cy;
                    margin = margins.bbottom;
                    break;
                default:
                    break;
                }
            }
            else
            {
                switch (lo)
                {
                case layout_origin::parent:
                    control->place(contentBox(), control_size);
                    break;
                case layout_origin::scene:
                    control->place(scene()->rect(), control_size);
                    break;
                default:
                    break;
                }
            }
        }

        if (_layout_direction == core::align::left)
        {
            layout_size.cx += margin;
        }
        else if (_layout_direction == core::align::top)
        {
            layout_size.cy += margin;
        }
        setLayoutedSize(layout_size);
        _invalid_layout = false;

        switch (_layout_direction)
        {
        case core::align::left:
            if (_scrollbar_h)
            {
                _scrollbar_h->setPageValue(contentBox().cx);
                _scrollbar_h->setValues(_scrollbar_h->value(), 0, _layouted_size.cx - contentBox().cx);
            }
            break;
        case core::align::top:
            if (_scrollbar_v)
            {
                _scrollbar_v->setPageValue(contentBox().cy);
                _scrollbar_v->setValues(_scrollbar_v->value(), 0, _layouted_size.cy - contentBox().cy);
            }
            break;
        default:
            break;
        }
    }

    void Scrollable::onPosChanged(const core::pt32f & from, const core::pt32f & to)
    {
        relayout();
        Container::onPosChanged(from, to);
    }

    void Scrollable::onSizeChanged(const core::si32f & from, const core::si32f & to)
    {
        relayout();
        Container::onSizeChanged(from, to);
    }

    void Scrollable::setScrollbarVisionV(scrollbar_vision scrollbar_vision)
    {
        if(_scrollbar_vision_v != scrollbar_vision)
        {
            _scrollbar_vision_v = scrollbar_vision;
            if (_scrollbar_vision_v == scrollbar_vision::always)
            {
                if(!_scrollbar_v)
                {
                    _scrollbar_v = std::make_shared<controls::ScrollBar>();
                    _scrollbar_v->setSize({ 0.5_em, 100_per });
                    _scrollbar_v->setAnchorBorders(core::align::right | core::align::topBottom);
                    _scrollbar_v->setLayoutOrigin(layout_origin::parent);
                    _scrollbar_v->setZValue(ZVALUE_SCROLLBAR);
                    _scrollbar_v->valueChagned += std::weak_bind(&Scrollable::onScrollValueChangedV, share_ref<Scrollable>(), std::placeholders::_1, std::placeholders::_2);
                }
                addControl(_scrollbar_v);
            }
        }
    }

    void Scrollable::setScrollbarVisionH(scrollbar_vision scrollbar_vision)
    {
        if (_scrollbar_vision_h != scrollbar_vision)
        {
            _scrollbar_vision_h = scrollbar_vision;
            if (_scrollbar_vision_h == scrollbar_vision::always)
            {
                if (!_scrollbar_h)
                {
                    _scrollbar_h = std::make_shared<controls::ScrollBar>();
                    _scrollbar_h->setSize({ 100_per, 0.5_em });
                    _scrollbar_h->setAnchorBorders(core::align::bottom | core::align::leftRight);
                    _scrollbar_h->setLayoutOrigin(layout_origin::parent);
                    _scrollbar_h->setZValue(ZVALUE_SCROLLBAR);
                    _scrollbar_h->setDirection(core::align::left);
                    _scrollbar_h->valueChagned += std::weak_bind(&Scrollable::onScrollValueChangedH, share_ref<Scrollable>(), std::placeholders::_1, std::placeholders::_2);
                }
                addControl(_scrollbar_h);
            }
        }
    }

    void Scrollable::relayout()
    {
        if (!_invalid_layout)
        {
            _invalid_layout = true;
            invoke([this]() {layout(nullptr); });
        }
    }

    void Scrollable::onScrollValueChangedV(float32_t from, float32_t to)
    {
        relayout();
    }

    void Scrollable::onScrollValueChangedH(float32_t from, float32_t to)
    {
        relayout();
    }
}

