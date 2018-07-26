#pragma once
#include "component/Scene.h"
#include "Container.h"

namespace controls
{
    class Element : public Container
    {
    public:
        Element();
        virtual ~Element();

        void layout();
        void layout(LayoutState & state, const core::si32f & size);
    };
}
