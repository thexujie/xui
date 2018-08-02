#pragma once
#include "controls/component/Interactable.h"

namespace controls::interactables
{
    class MouseRectangle: public component::MouseArea
    {
    public:
        MouseRectangle(std::shared_ptr<component::View> view) : MouseArea(view) {}
        ~MouseRectangle() = default;

        virtual void onMouseEnter(component::mosue_state & state) {}
        virtual void onMouseMove(component::mosue_state & state) {}
        virtual void onMouseLeave(component::mosue_state & state){}

        virtual void onMouseDown(component::mosue_state & state) {}
        virtual void onMouseUp(component::mosue_state & state) {}
        virtual void onMouseClick(component::mosue_state & state) {}
        virtual void onMouseDBClick(component::mosue_state & state) {}
    };
}
