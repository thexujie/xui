#include "stdafx.h"
#include "MouseRectangle.h"

namespace controls::interactables
{
    core::error MouseRectangle::onHitTest(const core::pt32f & pos) const
    {
        if (_rect.contains(pos))
            return core::error_ok;
        return core::error_failed;
    }
}
