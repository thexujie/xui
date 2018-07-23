#pragma once
#include "Control.h"

namespace controls
{
    class Container : public Control
    {
    public:
        Container();
        ~Container();

        void addControl(std::shared_ptr<Control> control);
        void removeControl(std::shared_ptr<Control> control);
        bool hasControl(std::shared_ptr<Control> control);

    public:
        void enteringScene(std::shared_ptr<component::Scene> & scene) override;
        void enterScene(std::shared_ptr<component::Scene> & scene) override;
        void leavingScene(std::shared_ptr<component::Scene> & scene) override;
        void leaveScene(std::shared_ptr<component::Scene> & scene) override;

    protected:
        std::list<std::shared_ptr<Control>> _controls;
    };
}
