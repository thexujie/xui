#include "stdafx.h"
#include "Container.h"
#include "renderables/Image.h"
#include "renderables/Rectangle.h"

namespace controls
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
        control->setParent(share_ref<Container>());
        _controls.push_back(control);
    }

    void Container::removeControl(std::shared_ptr<Control> control)
    {
        _controls.remove(control);
        control->setParent(nullptr);
    }

    void Container::enteringScene(std::shared_ptr<component::Scene> & scene)
    {
        Control::enteringScene(scene);
        for (auto & control : _controls)
            control->enteringScene(scene);
    }

    void Container::enterScene(std::shared_ptr<component::Scene> & scene)
    {
        Control::enterScene(scene);
        for (auto & control : _controls)
            control->enterScene(scene);
    }

    void Container::leavingScene(std::shared_ptr<component::Scene> & scene)
    {
        for (auto & control : _controls)
            control->leavingScene(scene);
        Control::leavingScene(scene);
    }

    void Container::leaveScene(std::shared_ptr<component::Scene> & scene)
    {
        for (auto & control : _controls)
            control->leaveScene(scene);
        Control::leaveScene(scene);
    }

    core::si32f Container::contentSize() const
    {
        core::si32f size;
        float32_t margin = 0;
        for (auto & control : _controls)
        {
            auto lo = control->layoutOrigin();
            if (lo != layout_origin::layout && lo != layout_origin::sticky)
                continue;

            auto m = control->realMargin();

            if(_layout_direction == core::align::left || _layout_direction == core::align::right)
            {
                margin = std::max(margin, _layout_direction == core::align::left ? m.bleft : m.bright);
                size.cx += margin;
                auto psize = control->prefferSize();
                margin = _layout_direction == core::align::left ? m.bright : m.bleft;
                size.cx = size.cx + psize.cx;
                size.cy = std::max(size.cy, psize.cy + m.bheight());
            }
            else if (_layout_direction == core::align::top || _layout_direction == core::align::bottom)
            {
                margin = std::max(margin, _layout_direction == core::align::top ? m.btop : m.bbottom);
                size.cy += margin;
                auto psize = control->prefferSize();
                margin = _layout_direction == core::align::top ? m.bbottom : m.btop;
                size.cy += psize.cy;
                size.cx = std::max(size.cx, psize.cx + m.bwidth());
            }
            else {}
        }
        return size;
    }

    void Container::layout()
    {
        float32_t margin = 0;
        core::si32f psize = prefferSize();

        core::rc32f box = paddingBox();
        core::rc32f rc;
        rc.cy = psize.cy;
        for (auto & control : _controls)
        {
            auto m = control->realMargin();
            auto ps = control->prefferSize();
            auto lo = control->layoutOrigin();
            auto & s = control->size();

            if (lo != layout_origin::layout && lo != layout_origin::sticky)
            {
                switch (lo)
                {
                case layout_origin::parent:
                    control->arrange(core::rc32f(core::pt32f(), realSize()), ps);
                    break;
                case layout_origin::scene:
                    control->arrange(scene()->rect(), ps);
                    break;
                case layout_origin::view:
                    control->arrange(scene()->viewRect(), ps);
                    break;
                default:
                    break;
                }
                continue;
            }

            if(_layout_direction == core::align::left)
            {
                margin = std::max(margin, m.bleft);
                rc.x += margin;
                rc.y = m.btop;
                rc.size = ps;
                if (s.cy.unit == core::unit::per)
                    ps.cy = control->calc_y(s.cy);
                control->arrange(rc, ps);
                margin = m.bright;
                rc.x += rc.cx;
            }
            else if (_layout_direction == core::align::top)
            {
                margin = std::max(margin, m.btop);
                rc.y += margin;
                rc.x = box.x + m.bleft;
                rc.size = ps;
                control->arrange(rc, ps);
                margin = m.bbottom;
                rc.y += rc.cy;
            }
            else{}
        }
        Control::layout();
    }

    void Container::update()
    {
        Control::update();
        for (auto & control : _controls)
            control->update();
    }

    void Container::onSizeChanged(const core::si32f & from, const core::si32f & to)
    {
        switch(_layout_direction)
        {
        case core::align::left:
            if (!core::equal(from.cy, to.cy))
                layout();
            break;
        case core::align::top:
            if (!core::equal(from.cx, to.cx))
                layout();
            break;
        default:
            break;
        }
        Control::onSizeChanged(from, to);
    }
}
