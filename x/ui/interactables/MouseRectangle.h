#pragma once
#include "ui/component/Interactable.h"

namespace ui::interactables
{
    class MouseRectangle: public component::MouseArea
    {
    public:
        MouseRectangle()  {}
        ~MouseRectangle() = default;

        core::error onHitTest(const core::pt32f & pos) const override;
    };
}
