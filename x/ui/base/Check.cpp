#include "stdafx.h"
#include "Check.h"
#include "ui/Scene.h"

namespace ui::base
{
    Check::Check()
    {
    }

    Check::~Check()
    {
        
    }

    void Check::propertyTableCallback(core::property_table & properties)
    {
        Control::propertyTableCallback(properties);
    }

    void Check::propertyTable(core::property_table & properties)
    {
        Check::propertyTableCallback(properties);
    }

    void Check::enteringScene(std::shared_ptr<Scene> & scene)
    {
        Control::enteringScene(scene);
    }

    void Check::leavingScene()
    {
        Control::leavingScene();
    }

    void Check::setCheckState(check_state state)
    {
        if (_state != state)
        {
            _state = state;
            refresh();
            stateChanged(_state);
        }
    }
}
