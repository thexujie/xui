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

        void setLayoutDirection(core::align layout);

        void addControl(std::shared_ptr<Control> control);
        void removeControl(std::shared_ptr<Control> control);
        std::list<std::shared_ptr<Control>> & children() { return _controls; }

        void enteringScene(std::shared_ptr<component::Scene> & scene) override;
        void enterScene(std::shared_ptr<component::Scene> & scene) override;
        void leavingScene(std::shared_ptr<component::Scene> & scene) override;
        void leaveScene(std::shared_ptr<component::Scene> & scene) override;

        core::si32f contentSize() const override;
        void layout() override;
        void update() override;

        void onSizeChanged(const core::si32f & from, const core::si32f & to) override;
    protected:
        std::list<std::shared_ptr<Control>> _controls;
        core::align _layout_direction = core::align::left;
        bool _top_level = false;
    };
}
