#include "stdafx.h"
#include "font.h"
#include "graphics/GraphicsService.h"

namespace graphics::text
{
    fontmetrics font::metrics() const
    {
        return graphics::GraphicsService().FontMetrics(*this);
    }
}
