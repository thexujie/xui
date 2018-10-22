#include "stdafx.h"
#include "Radio.h"
#include "ui/Scene.h"

namespace ui::base
{
    Radio::Radio()
    {
    }

    Radio::~Radio()
    {
        
    }

    void Radio::propertyTableCallback(core::property_table & properties)
    {
        Control::propertyTableCallback(properties);
    }

    void Radio::propertyTable(core::property_table & properties)
    {
        Radio::propertyTableCallback(properties);
    }

    void Radio::onEnteringScene(std::shared_ptr<Scene> & scene)
    {
        if (!_group.empty())
        {
            _radio_group = scene->radioGroup(_group);
            _radio_group->addRadio(share_ref<Radio>());
        }
        Control::onEnteringScene(scene);
    }

    void Radio::onLeavingScene()
    {
        if (_radio_group)
        {
            _radio_group->removeRadio(share_ref<Radio>());
            _radio_group = nullptr;
        }
        Control::onLeavingScene();
    }

    void Radio::setGroup(std::string group)
    {
        if (group != _group)
        {
            if (_radio_group)
            {
                _radio_group->removeRadio(share_ref<Radio>());
                _radio_group = nullptr;
            }
            _group = group;
            if (!group.empty())
            {
                if (auto s = scene())
                    _radio_group = s->radioGroup(group);
            }

        }
    }

    void Radio::setCheckState(check_state state)
    {
        if (_state != state)
        {
            _state = state;
            if (_radio_group && state == check_state::checked)
                _radio_group->check(share_ref<Radio>());
            refresh();
            stateChanged(state);
        }
    }
}
