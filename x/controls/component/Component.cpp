#include "stdafx.h"
#include "Component.h"
#include "View.h"

namespace controls::component
{
    Component::Component(ComponentType type, std::shared_ptr<View> view) :_type(type), _view(view)
    {
        view->insert(shared_from_this());
    }

    Component::Component(ComponentType type, std::shared_ptr<View> view, const core::rc32f & rect) : _type(type), _view(view), _rect(rect)
    {
        view->insert(shared_from_this());
    }

    void Component::enteringScene(Scene & scene)
    {

    }

    void Component::enterScene(Scene & scene)
    {

    }

    void Component::leavingScene(Scene & scene)
    {

    }

    void Component::leaveScene(Scene & scene)
    {

    }
}
