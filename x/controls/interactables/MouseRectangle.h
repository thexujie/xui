#pragma once
#include "controls/component/Interactable.h"

namespace controls::interactables
{
    class MouseRectangle: public component::MouseArea
    {
    public:
        MouseRectangle()  {}
        ~MouseRectangle() = default;

        core::error onHitTest(const core::pt32f & pos) const override;
    };
}
