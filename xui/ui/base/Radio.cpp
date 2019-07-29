#include "stdafx.h"
#include "Radio.h"
#include "ui/Form.h"

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

    void Radio::onEntering(std::shared_ptr<Form> & form)
    {
        if (!_group.empty())
        {
            _radio_group = form->radioGroup(_group);
            _radio_group->addRadio(share_ref<Radio>());
        }
        Control::onEntering(form);
    }

    void Radio::onLeaving()
    {
        if (_radio_group)
        {
            _radio_group->removeRadio(share_ref<Radio>());
            _radio_group = nullptr;
        }
        Control::onLeaving();
    }

    void Radio::setGroup(const std::u8string & group)
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
                if (auto f = form())
                    _radio_group = f->radioGroup(group);
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
            repaint();
            stateChanged(state);
        }
    }
}
