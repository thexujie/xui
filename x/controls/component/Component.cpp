#include "stdafx.h"
#include "Component.h"

namespace controls::component
{
    void Component::setPos(const core::pt32f & pt)
    {
        if (pt == _pos)
            return;
        _pos = pt;
    }

    core::pt32f Component::pos() const
    {
        return _pos;
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
