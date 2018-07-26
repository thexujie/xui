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

        void layout();
        void layout(LayoutState & state, const core::si32f & size);

    protected:
        std::list<std::shared_ptr<Control>> _controls;
    };
}
