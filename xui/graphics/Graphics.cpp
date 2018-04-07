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

    void Graphics::Clear(core::color32 color)
    {
        if (!_graphics)
            return;

        _state = _graphics->Clear(color);
    }

    void Graphics::DrawLine(pt32_t start, pt32_t end, color32 color, float32_t width)
    {
        if (!_graphics)
            return;

        _state = _graphics->DrawLine(start, end, color, width);
    }

    void Graphics::DrawRect(rc32_t rect, color32 color, float32_t width)
    {
        if (!_graphics)
            return;

        _state = _graphics->DrawRect(rect, color, width);
    }

    void Graphics::FillRect(rc32_t rect, color32 color)
    {
        if (!_graphics)
            return;

        _state = _graphics->FillRect(rect, color);
    }

    void Graphics::DrawString(std::string str, core::color32 color, font font, pt32_t point, int32_t flags)
    {
        if (!_graphics)
            return;

        _state = _graphics->DrawString(str, color, font, point, flags);
    }

    void Graphics::DrawString(std::string str, core::color32 color, font font, rc32_t rect, int32_t flags)
    {
        if (!_graphics)
            return;

        _state = _graphics->DrawString(str, color, font, rect, flags);
    }

    void Graphics::DrawImage(graphics::Image & image, pt32_t point, int32_t flags)
    {
        if (!_graphics)
            return;

        _state = _graphics->DrawImage(*(image.image()), point, flags);
    }

    void Graphics::DrawImage(graphics::Image & image, rc32_t rect, int32_t flags)
    {
        if (!_graphics)
            return;

        _state = _graphics->DrawImage(*(image.image()), rect, flags);
    }

}
