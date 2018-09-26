#pragma once
#include "ui/Control.h"
#include "ui/RadioGroup.h"

namespace ui::base
{
    class Check : public Control
    {
    protected:
        Check();
        ~Check();

    public:
        static void propertyTableCallback(core::property_table & properties);
        void propertyTable(core::property_table & properties) override;

        void enteringScene(std::shared_ptr<Scene> & scene) override;
        void leavingScene() override;

    public:
        void setCheckState(check_state state);

    protected:
        check_state _state = check_state::unchecked;

    public:
        core::event<void(check_state state)> stateChanged;
    };
}
