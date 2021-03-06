#pragma once
#include "ui/Control.h"
#include "ui/RadioGroup.h"

namespace ui::base
{
    class Radio : public Control
    {
    protected:
        Radio();
        ~Radio();

    public:
        static void propertyTableCallback(core::property_table & properties);
        void propertyTable(core::property_table & properties) override;

        void onEnteringScene(std::shared_ptr<Scene> & scene) override;
        void onLeavingScene() override;

    public:
        void setGroup(const std::u8string & group);
        void setCheckState(check_state state);

    protected:
        std::u8string _group;
        std::shared_ptr<RadioGroup> _radio_group;

        check_state _state = check_state::unchecked;

    public:
        core::event<void(check_state state)> stateChanged;
    };
}
