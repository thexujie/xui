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

    void Container::update()
    {
        Control::update();
        for (auto & control : _controls)
            control->update();
    }
}
