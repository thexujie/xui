#include "stdafx.h"
#include "Graphics.h"

namespace graphics
{

    Graphics::Graphics(si32_t size):_graphics(GraphicsService().CreateGraphics(GraphicsService().CreatePixmap(size)))
    {
        
    }

    Graphics::Graphics(std::shared_ptr<IGraphics> graphics) : _graphics(graphics)
    {

    }

    Graphics::Graphics(std::shared_ptr<Pixmap> pixmap): _graphics(GraphicsService().CreateGraphics(pixmap->handle()))
    {

    }

    void Graphics::Clear()
    {
        if (!_graphics)
            return;

        _graphics->Clear();
    }
}
