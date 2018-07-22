#include "stdafx.h"
#include "GraphicsService.h"
#include "core/app.h"
#include "skia/skia.h"

namespace graphics
{
    thread_local std::shared_ptr<IGraphicsService> __graphicsService = nullptr;

    IGraphicsService & GraphicsService()
    {
        if (!__graphicsService)
        {
            auto service = core::app().GetService("GraphicsService");
            __graphicsService = std::dynamic_pointer_cast<IGraphicsService>(service);
        }

        return *__graphicsService.get();
    }
}
