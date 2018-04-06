#include "stdafx.h"
#include "GraphicsService.h"
#include "core/app.h"

namespace graphics
{
    IGraphicsService & GraphicsService()
    {
        return *(IGraphicsService *)nullptr;
    }
}
