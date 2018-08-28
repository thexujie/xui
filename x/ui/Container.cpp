#include "stdafx.h"
#include "Container.h"
#include "renderables/Image.h"
#include "renderables/Rectangle.h"
#include "controls/ScrollBar.h"

namespace ui
{
    Container::Container()
    {
    }

    Container::~Container()
    {
        
    }

    void Container::setLayoutDirection(core::align layout)
    {
        _layout_direction = layout;
    }

    void Container::addControl(std::shared_ptr<Control> control)
    {
        if (std::any_of(_controls.begin(), _controls.end(), [&control](const auto & pair) { return pair.second == control; }))
            return;

        auto s = scene();
        control->setParent(share_ref<Container>());
        if(s)
        {
            control->enteringScene(s);
            _controls.insert(std::make_pair(control->ZValue(), control));
            control->enterScene(s);
        }
        else
            _controls.insert(std::make_pair(control->ZValue(), control));
        relayout();
    }

    void Container::removeControl(std::shared_ptr<Control> control)
    {
        control->leavingScene();
        for (auto iter = _controls.begin(); iter != _controls.end(); )
        {
            if (iter->second == control)
            {
                control->setParent(nullptr);
                _controls.erase(iter);
                break;
            }
        }
        control->leaveScene();
    }

    void Container::enteringScene(std::shared_ptr<component::Scene> & scene)
    {
        Control::enteringScene(scene);
        for (auto & iter : _controls)
            iter.second->enteringScene(scene);
    }

    void Container::enterScene(std::shared_ptr<component::Scene> & scene)
    {
        Control::enterScene(scene);
        for (auto & iter : _controls)
            iter.second->enterScene(scene);
    }

    void Container::leavingScene()
    {
        for (auto & iter : _controls)
            iter.second->leavingScene();
        Control::leavingScene();
    }

    void Container::leaveScene()
    {
        for (auto & iter : _controls)
            iter.second->leaveScene();
        Control::leaveScene();
    }

    core::si32f Container::contentSize() const
    {
        core::si32f size;
        float32_t margin = 0;
        for (auto & iter : _controls)
        {
            auto& control = iter.second;
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

    void Container::layout(layout_flags flags)
    {
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
                    control->place({ layout_pos + layout_size.cx + margin, box.y + margins.btop, control_size.cx, fitting_size.cy }, control_size);
                    layout_size.cx += margin + control_size.cx;
                    margin = margins.bright;
                    break;
                case core::align::top:
                    margin = std::max(margin, margins.btop);
                    control->place({ box.x + margins.bleft, layout_pos + layout_size.cy + margin, fitting_size.cx, control_size.cy }, control_size);
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
    }

    void Container::arrange()
    {
        float32_t margin = 0;
        core::si32f layout_size = expectSize();
        core::rc32f box = paddingBox();
        core::rc32f layout_rect;
        layout_rect.x = _rect.x;
        if (_layout_direction == core::align::left && _scrollbar_h)
            layout_rect.x -= _scrollbar_h->value();
        std::set<std::string> setes;
        for (auto & iter : _controls)
        {
            auto& control = iter.second;
            auto m = control->realMargin();
            auto lo = control->layoutOrigin();

            if (lo != layout_origin::layout && lo != layout_origin::sticky)
                continue;

            if (_layout_direction == core::align::left)
            {
                margin = std::max(margin, m.bleft);
                layout_rect.x += margin;
                layout_rect.y = m.btop;
                layout_rect.cx = control->realSize().cx;
                layout_rect.cy = layout_size.cy;
                control->setShowPos(layout_rect.pos);
                margin = m.bright;
                layout_rect.x += layout_rect.cx;
            }
            else if (_layout_direction == core::align::top)
            {
                margin = std::max(margin, m.btop);
                layout_rect.y += margin;
                layout_rect.x = box.x + m.bleft;
                layout_rect.cx = layout_size.cx;
                layout_rect.cy = control->realSize().cy;
                control->setShowPos(layout_rect.pos);
                margin = m.bbottom;
                layout_rect.y += layout_rect.cy;
            }
            else {}
        }
    }

    void Container::update()
    {
        Control::update();
        for (auto & iter : _controls)
            iter.second->update();
    }

    void Container::onPosChanged(const core::pt32f & from, const core::pt32f & to)
    {
        layout(layout_flag::resize_cx | layout_flag::resize_cy);
        Control::onPosChanged(from, to);
    }

    void Container::onSizeChanged(const core::si32f & from, const core::si32f & to)
    {
        switch(_layout_direction)
        {
        case core::align::left:
            layout(core::equal(from.cy, to.cy) ? layout_flag::none : layout_flag::resize_cy);
            if (_scrollbar_h)
            {
                _scrollbar_h->setPageValue(contentBox().cx);
                _scrollbar_h->setValues(_scrollbar_h->value(), 0, _layouted_size.cx - contentBox().cx);
            }
            break;
        case core::align::top:
            layout(core::equal(from.cx, to.cx) ? layout_flag::none : layout_flag::resize_cx);
            if (_scrollbar_v)
            {
                _scrollbar_v->setPageValue(contentBox().cy);
                _scrollbar_v->setValues(_scrollbar_v->value(), 0, _layouted_size.cy - contentBox().cy);
            }
            break;
        default:
            break;
        }
        Control::onSizeChanged(from, to);
    }

    void Container::setScrollbarVisionV(scrollbar_vision scrollbar_vision)
    {
        if(_scrollbar_vision_v != scrollbar_vision)
        {
            _scrollbar_vision_v = scrollbar_vision;
            if (_scrollbar_vision_v == scrollbar_vision::always)
            {
                if(!_scrollbar_v)
                {
                    _scrollbar_v = std::make_shared<controls::ScrollBar>();
                    _scrollbar_v->setSize({ 1_em, 100_per });
                    _scrollbar_v->setAnchorBorders(core::align::right | core::align::topBottom);
                    _scrollbar_v->setLayoutOrigin(layout_origin::parent);
                    _scrollbar_v->setZValue(ZVALUE_SCROLLBAR);
                    _scrollbar_v->valueChagned += std::weak_bind(&Container::onScrollValueChangedV, share_ref<Container>(), std::placeholders::_1, std::placeholders::_2);
                }
                addControl(_scrollbar_v);
            }
        }
    }

    void Container::setScrollbarVisionH(scrollbar_vision scrollbar_vision)
    {
        if (_scrollbar_vision_h != scrollbar_vision)
        {
            _scrollbar_vision_h = scrollbar_vision;
            if (_scrollbar_vision_h == scrollbar_vision::always)
            {
                if (!_scrollbar_h)
                {
                    _scrollbar_h = std::make_shared<controls::ScrollBar>();
                    _scrollbar_h->setSize({ 100_per, 1_em });
                    _scrollbar_h->setAnchorBorders(core::align::bottom | core::align::leftRight);
                    _scrollbar_h->setLayoutOrigin(layout_origin::parent);
                    _scrollbar_h->setZValue(ZVALUE_SCROLLBAR);
                    _scrollbar_h->setDirection(core::align::left);
                    _scrollbar_h->valueChagned += std::weak_bind(&Container::onScrollValueChangedH, share_ref<Container>(), std::placeholders::_1, std::placeholders::_2);
                }
                addControl(_scrollbar_h);
            }
        }
    }

    void Container::relayout()
    {
        if (!_invalid_layout)
        {
            _invalid_layout = true;
            invoke([this]() {layout(nullptr); });
        }
    }

    void Container::onScrollValueChangedV(float32_t from, float32_t to)
    {
        layout(layout_flag::none);
    }

    void Container::onScrollValueChangedH(float32_t from, float32_t to)
    {
        layout(layout_flag::none);
    }

    void Container::onLayoutedSizeChaged(const core::si32f & from, const core::si32f & to)
    {
        layoutedSizeChaged(from, to);
    }

    void Container::setLayoutedSize(const core::si32f & layouted_size)
    {
        if(layouted_size != _layouted_size)
        {
            _layouted_size = layouted_size;
        }
    }
}

