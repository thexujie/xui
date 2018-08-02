#include "stdafx.h"
#include "Component.h"
#include "View.h"

namespace controls::component
{
    Component::Component(ComponentType type) :_type(type)
    {
    }

    Component::Component(ComponentType type, const core::rc32f & rect) : _type(type), _rect(rect)
    {
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

    void Component::invalid()
    {
        if (auto v = view()) 
            v->invalid(_rect);
    }
}
