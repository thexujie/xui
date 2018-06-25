#include "stdafx.h"
#include "font.h"
#include "win32/GraphicsService.h"

namespace graphics::text
{
    fontmetrics font::metrics() const
    {
        return win32::GraphicsService().FontMetrics(*this);
    }
}
