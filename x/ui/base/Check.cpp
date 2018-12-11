#include "stdafx.h"
#include "Check.h"

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

    void Check::onEntering(std::shared_ptr<Form> & form)
    {
        Control::onEntering(form);
    }

    void Check::onLeaving()
    {
        Control::onLeaving();
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
