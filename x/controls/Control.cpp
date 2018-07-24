#include "stdafx.h"
#include "Control.h"

namespace controls
{
    Control::Control()
    {
        
    }
    Control::~Control()
    {
        
    }

    void Control::setRect(const core::rc32f & rect)
    {
        if (_rect == rect)
            return;

        core::rc32f rect_old = _rect;
        _rect = rect;
        onRectChanged(rect_old, _rect);
    }

    void Control::setPos(const core::pt32f & pos)
    {
        if (_rect.position == pos)
            return;
        core::rc32f rect_old = _rect;
        _rect.position = pos;
        onRectChanged(rect_old, _rect);
    }

    void Control::setSize(const core::si32f & size)
    {
        if (_rect.size == size)
            return;
        core::rc32f rect_old = _rect;
        _rect.size = size;
        onRectChanged(rect_old, _rect);
    }

    std::shared_ptr<component::Scene> Control::scene() const
    {
        return _scene.lock();
    }

    void Control::enteringScene(std::shared_ptr<component::Scene> & scene)
    {

    }

    void Control::enterScene(std::shared_ptr<component::Scene> & scene)
    {
        _scene = scene;
    }

    void Control::leavingScene(std::shared_ptr<component::Scene> & scene)
    {

    }

    void Control::leaveScene(std::shared_ptr<component::Scene> & scene)
    {
        _scene.reset();
    }

    void Control::onRectChanged(const core::rc32f & from, const core::rc32f & to)
    {
        rectChanged(from, to);
    }
}
