#include "stdafx.h"

#include "core/app.h"
#include "core/math/vec2.h"
#include "graphics/GraphicsService.h"

class GraphicsService
{
public:
    std::shared_ptr<core::handle_t> CreatePixmap(core::math::si32_t)
    {
        
    }

    std::shared_ptr<graphics::IGraphics> CreateGraphics(std::shared_ptr<core::handle_t> pixmap)
    {
        
    }
};

class Win32App : public core::IApp
{
public:
    void * GetService(std::string name)
    {
        if (name != "GraphicsService")
            return nullptr;

    }
};