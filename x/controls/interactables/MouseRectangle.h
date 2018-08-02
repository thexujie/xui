#pragma once
#include "controls/component/Interactable.h"

namespace controls::interactables
{
    class MouseRectangle: public component::MouseArea
    {
    public:
        MouseRectangle()  {}
        ~MouseRectangle() = default;

        core::error onHitTest(const core::pt32f & pos) const;

        virtual void onMouseEnter(component::mosue_state & state) {}
        virtual void onMouseMove(component::mosue_state & state) {}
        virtual void onMouseLeave(component::mosue_state & state){}

        virtual void onMouseDown(component::mosue_state & state) {}
        virtual void onMouseUp(component::mosue_state & state) {}
        virtual void onMouseClick(component::mosue_state & state) {}
        virtual void onMouseDBClick(component::mosue_state & state) {}
    };
}
