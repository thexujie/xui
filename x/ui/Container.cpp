#include "stdafx.h"
#include "Container.h"
#include "renderables/Image.h"
#include "renderables/Rectangle.h"
#include "controls/ScrollBar.h"

namespace ui
{
    Container::Container()
    {
        _scrollbar_v = std::make_shared<controls::ScrollBar>();
        _scrollbar_v->setSize({ 1_em, 100_per });
        _scrollbar_v->setAnchorBorders(core::align::right | core::align::topBottom);
        _scrollbar_v->setAnchor({ 1_em, 100_per });
        _scrollbar_v->setLayoutOrigin(layout_origin::parent);
        _scrollbar_v->setZValue(ZVALUE_SCROLLBAR);
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
        if (std::find_if(_controls.begin(), _controls.end(), [&control](const auto & pair) { return pair.second == control; }) != _controls.end())
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
        core::si32f layout_size = expectSize();
        core::rc32f box = paddingBox();
        core::rc32f layout_rect;
        layout_rect.cy = layout_size.cy;
        std::set<std::string> setes;
        for (auto & iter : _controls)
        {
            auto& control = iter.second;
            auto m = control->realMargin();
            auto lo = control->layoutOrigin();
            auto es = control->expectSize();
            auto ps = control->prefferSize(calc_flag::none);

            if (lo != layout_origin::layout && lo != layout_origin::sticky)
            {
                switch (lo)
                {
                case layout_origin::parent:
                    control->place(contentBox(), ps);
                    break;
                case layout_origin::scene:
                    control->place(scene()->rect(), ps);
                    break;
                default:
                    break;
                }
                continue;
            }

            if (flags.any(layout_flag::no_resize))
                continue;

            if(_layout_direction == core::align::left)
            {
                margin = std::max(margin, m.bleft);
                layout_rect.x += margin;
                layout_rect.y = m.btop;
                layout_rect.cx = ps.cx;
                layout_rect.cy = layout_size.cy;
                control->place(layout_rect, ps);
                margin = m.bright;
                layout_rect.x += layout_rect.cx;
            }
            else if (_layout_direction == core::align::top)
            {
                margin = std::max(margin, m.btop);
                layout_rect.y += margin;
                layout_rect.x = box.x + m.bleft;
                layout_rect.cx = layout_size.cx;
                layout_rect.cy = ps.cy;
                control->place(layout_rect, ps);
                margin = m.bbottom;
                layout_rect.y += layout_rect.cy;
            }
            else{}
        }
        Control::layout(flags);
    }

    void Container::update()
    {
        Control::update();
        for (auto & iter : _controls)
            iter.second->update();
    }

    void Container::onSizeChanged(const core::si32f & from, const core::si32f & to)
    {
        switch(_layout_direction)
        {
        case core::align::left:
            layout(core::equal(from.cy, to.cy) ? layout_flag::no_resize : layout_flag::none);
            break;
        case core::align::top:
            layout(core::equal(from.cx, to.cx) ? layout_flag::no_resize : layout_flag::none);
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
                addControl(_scrollbar_v);
        }
    }
}

