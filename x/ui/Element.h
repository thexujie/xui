#pragma once
#include "component/Scene.h"
#include "Container.h"

namespace ui
{
    class Element : public Container
    {
    public:
        Element();
        virtual ~Element();

        void layout() override;
        void layout(const core::si32f & size);
    };
}
