#pragma once
#include "ui/component/Interactable.h"

namespace ui::interactables
{
    class MouseRectangle: public component::MouseArea
    {
    public:
        MouseRectangle(std::shared_ptr<ui::Control> control) : component::MouseArea(control) {}
        ~MouseRectangle() = default;

        core::error onHitTest(const core::pt32f & pos) const override;
    };
}
