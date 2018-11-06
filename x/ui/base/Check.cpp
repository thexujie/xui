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

    void Check::onEnteringScene(std::shared_ptr<Scene> & scene)
    {
        Control::onEnteringScene(scene);
    }

    void Check::onLeavingScene()
    {
        Control::onLeavingScene();
    }

    void Check::setCheckState(check_state state)
    {
        if (_state != state)
        {
            _state = state;
            repaint();
            stateChanged(_state);
            switch(state)
            {
            case check_state::unchecked:
                unchecked();
                break;
            case check_state::checked:
                checked();
                break;
            default:
                break;
            }
        }
    }
}
