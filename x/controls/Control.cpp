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
        if (rect_old.position != _rect.position)
            posChanged(rect_old.position, _rect.position);
        if (rect_old.size != _rect.size)
            sizeChanged(rect_old.size, _rect.size);
        rectChanged(rect_old, _rect);
    }

    void Control::setPos(const core::pt32f & pos)
    {
        if (_rect.position == pos)
            return;
        core::pt32f pos_old = _rect.position; 
        _rect.position = pos;
        posChanged(pos_old, _rect.position);
    }

    void Control::setSize(const core::si32f & size)
    {
        if (_rect.size == size)
            return;
        core::si32f size_old = _rect.size;
        _rect.size = size;
        posChanged(size_old, _rect.size);
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
}
