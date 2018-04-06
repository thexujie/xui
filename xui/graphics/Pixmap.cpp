#include "stdafx.h"
#include "Pixmap.h"

namespace graphics
{
    using namespace core;
    using namespace core::math;

    Pixmap::Pixmap(si32_t size)
    {
        _handle = GraphicsService().CreatePixmap(size);
    }

    Pixmap::Pixmap(std::shared_ptr<core::handle_t> handle) : _handle(handle)
    {
        
    }
}

