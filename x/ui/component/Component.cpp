#include "stdafx.h"
#include "Component.h"

namespace ui::component
{
    Component::Component(ComponentType type, std::shared_ptr<ui::Control> control) :_type(type), _control(control)
    {
    }

    Component::Component(ComponentType type, std::shared_ptr<ui::Control> control, const core::rc32f & rect) : _type(type), _control(control), _rect(rect)
    {
    }
}
