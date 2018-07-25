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

    protected:
        std::list<std::shared_ptr<Control>> _controls;
    };
}
