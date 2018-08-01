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

    void Container::addControl(std::shared_ptr<Control> control)
    {
        control->setParent(shared_from_this());
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

            auto m = control->margin();
            margin = std::max(margin, m.bleft);

            if(_layout_direction == core::align::left || _layout_direction == core::align::right)
            {
                size.cx += margin;
                auto psize = control->prefferSize();
                margin = m.bright;
                size.cx = size.cx + psize.cx;
                size.cy = std::max(size.cy, psize.cy + m.bheight());
            }
        }
        return size;
    }

    void Container::layout()
    {
        float32_t margin = 0;
        core::si32f psize = prefferSize();

        core::rc32f rc;
        if(!_top_level)
            rc.pos = paddingBox().leftTop();
        rc.cy = psize.cy;
        for (auto & control : _controls)
        {
            auto m = control->margin();
            auto ps = control->prefferSize();
            auto lo = control->layoutOrigin();

            if (lo != layout_origin::layout && lo != layout_origin::sticky)
            {
                switch (lo)
                {
                case layout_origin::parent:
                    control->layout(core::rc32f(core::pt32f(), realSize()), ps);
                    break;
                case layout_origin::scene:
                    control->layout(scene()->rect(), ps);
                    break;
                case layout_origin::view:
                    control->layout(scene()->viewRect(), ps);
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
                control->layout(rc, ps);
                margin = m.bright;
                rc.x += rc.cx;
            }
        }
    }

    void Container::update()
    {
        Control::update();
        for (auto & control : _controls)
            control->update();
    }
}
