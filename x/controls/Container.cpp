#include "stdafx.h"
#include "Container.h"

namespace controls
{
    Container::Container()
    {

    }

    Container:: ~Container()
    {

    }

    void Container::addControl(std::shared_ptr<Control> control)
    {
        if (!control || hasControl(control))
            return;

        auto scene = _scene.lock();
        if (scene)
            control->enteringScene(scene);
        _controls.push_back(control);
        if (scene)
            control->enterScene(scene);
    }

    void Container::removeControl(std::shared_ptr<Control> control)
    {
        if (!control)
            return;

        auto scene = _scene.lock();
        if (scene)
            control->leavingScene(scene);
        _controls.push_back(control);
        if (scene)
            control->leaveScene(scene);
    }

    bool Container::hasControl(std::shared_ptr<Control> control)
    {
        return std::find(_controls.begin(), _controls.end(), control) != _controls.end();
    }

    void Container::enteringScene(std::shared_ptr<component::Scene> & scene)
    {
        if (!scene)
            return;

        Control::enteringScene(scene);
        for (std::shared_ptr<Control> & control : _controls)
            control->enteringScene(scene);
    }

    void Container::enterScene(std::shared_ptr<component::Scene> & scene)
    {
        if (!scene)
            return;

        Control::enterScene(scene);
        for (std::shared_ptr<Control> & control : _controls)
            control->enterScene(scene);
    }

    void Container::leavingScene(std::shared_ptr<component::Scene> & scene)
    {
        if (!scene)
            return;

        for (std::shared_ptr<Control> & control : _controls)
            control->leavingScene(scene);
        Control::leavingScene(scene);
    }

    void Container::leaveScene(std::shared_ptr<component::Scene> & scene)
    {
        if (!scene)
            return;

        for (std::shared_ptr<Control> & control : _controls)
            control->leaveScene(scene);
        Control::leaveScene(scene);
    }
}
