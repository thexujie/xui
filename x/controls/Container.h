#pragma once
#include "component/Scene.h"
#include "Control.h"

namespace controls
{
    class Container : public Control
    {
    public:
        Container();
        virtual ~Container();

        void addControl(std::shared_ptr<Control> control);
        void removeControl(std::shared_ptr<Control> control);

        virtual void enteringScene(std::shared_ptr<component::Scene> & scene);
        virtual void enterScene(std::shared_ptr<component::Scene> & scene);
        virtual void leavingScene(std::shared_ptr<component::Scene> & scene);
        virtual void leaveScene(std::shared_ptr<component::Scene> & scene);

        virtual core::si32f contentSize() const;
        virtual void layout();
        virtual void update();

    protected:
        std::list<std::shared_ptr<Control>> _controls;
        core::align _layout_direction = core::align::left;
        bool _top_level = false;
    };
}
