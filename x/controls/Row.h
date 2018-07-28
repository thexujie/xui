#pragma once
#include "component/Scene.h"
#include "Container.h"

namespace controls
{
    class Row : public Container
    {
    public:
        Row();
        virtual ~Row();

        core::si32f expectContentSize() const;
        void layout(const core::rc32f & rect);
    };
}
