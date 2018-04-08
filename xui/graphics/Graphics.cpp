#include "stdafx.h"
#include "Graphics.h"

namespace graphics
{
    Graphics::Graphics(si32_t size): _graphics(GraphicsService().CreateGraphics(GraphicsService().CreatePixmap(size))) { }

    Graphics::Graphics(std::shared_ptr<IGraphics> graphics) : _graphics(graphics) { }

    Graphics::Graphics(std::shared_ptr<Pixmap> pixmap): _graphics(GraphicsService().CreateGraphics(pixmap->handle())) { }

    void Graphics::PushOrign(core::math::pt32_t point)
    {
        if (_graphics)
            _graphics->PushOrign(point);
    }

    core::math::pt32_t Graphics::GetOrign() const
    {
        if (_graphics)
            return _graphics->GetOrign();
        return {};
    }

    void Graphics::PopOrign()
    {
        if (_graphics)
            _graphics->PopOrign();
    }

    void Graphics::PushClip(core::math::rc32_t rect)
    {
        if (_graphics)
            _graphics->PushClip(rect);
    }

    core::math::rc32_t Graphics::GetClip() const
    {
        if (_graphics)
            return _graphics->GetClip();
        return {};
    }

    void Graphics::PopClip()
    {
        if (_graphics)
            _graphics->PopClip();
    }

    void Graphics::Clear(core::color32 color)
    {
        if (!_graphics)
            return;

        _graphics->Clear(color);
    }

    void Graphics::DrawLine(pt32_t start, pt32_t end, color32 color, float32_t width)
    {
        if (!_graphics)
            return;

        _graphics->DrawLine(start, end, color, width);
    }

    void Graphics::DrawRect(rc32_t rect, color32 color, float32_t width)
    {
        if (!_graphics)
            return;

        _graphics->DrawRect(rect, color, width);
    }

    void Graphics::FillRect(rc32_t rect, color32 color)
    {
        if (!_graphics)
            return;

        _graphics->FillRect(rect, color);
    }

    void Graphics::DrawString(std::string str, pt32_t point, font font, core::color32 color, int32_t flags)
    {
        if (!_graphics)
            return;

        core::math::si32_t size = _graphics->MeasureString(str, font);
        if (flags & core::math::align::right)
            point.x -= size.cx;
        else if (flags & core::math::align::centerX)
            point.x -= size.cx / 2;
        else {}

        if (flags & core::math::align::bottom)
            point.y -= size.cy;
        else if (flags & core::math::align::centerY)
            point.y -= size.cy / 2;
        else {}

        _graphics->DrawString(str, point, font, color);
    }

    void Graphics::DrawString(std::string str, rc32_t rect, font font, core::color32 color, int32_t flags)
    {
        if (!_graphics)
            return;
        if (!_graphics)
            return;

        pt32_t point;
        core::math::si32_t size = _graphics->MeasureString(str, font);
        if (flags & core::math::align::right)
            point.x = rect.right() - size.cx;
        else if (flags & core::math::align::centerX)
            point.x = rect.centerX() - size.cx / 2;
        else
            point.x = rect.x;

        if (flags & core::math::align::bottom)
            point.y = rect.bottom() - size.cy;
        else if (flags & core::math::align::centerY)
            point.y = rect.centerY() - size.cy / 2;
        else
            point.y = rect.y;

        PushClip(rect);
        _graphics->DrawString(str, point, font, color);
        PopClip();
    }

    void Graphics::DrawImage(graphics::Image & image, pt32_t point, int32_t flags)
    {
        if (!_graphics)
            return;

        auto size = image.size();
        if (flags & core::math::align::right)
            point.x -= size.cx;
        else if (flags & core::math::align::centerX)
            point.x -= size.cx / 2;
        else {}

        if (flags & core::math::align::bottom)
            point.y -= size.cy;
        else if (flags & core::math::align::centerY)
            point.y -= size.cy / 2;
        else {}

        _graphics->DrawImage(*(image.image()), point);
    }

    void Graphics::DrawImage(graphics::Image & image, rc32_t rect, int32_t flags)
    {
        if (!_graphics)
            return;

        pt32_t point;
        auto size = image.size();
        if (flags & core::math::align::right)
            point.x = rect.right() - size.cx;
        else if (flags & core::math::align::centerX)
            point.x = rect.centerX() - size.cx / 2;
        else
            point.x = rect.x;

        if (flags & core::math::align::bottom)
            point.y = rect.bottom() - size.cy;
        else if (flags & core::math::align::centerY)
            point.y = rect.centerY() - size.cy / 2;
        else
            point.y = rect.y;

        _graphics->PushClip(rect);
        _graphics->DrawImage(*(image.image()), point);
        _graphics->PopClip();
    }

    void Graphics::DrawImage(graphics::Image & image, pt32_t point, core::math::rc32_t region, int32_t flags)
    {
        if (!_graphics)
            return;

        if (region.right() > image.size().cx)
            region.setRight(image.size().cx);
        if (region.bottom() > image.size().cy)
            region.setBottom(image.size().cy);

        if (flags & core::math::align::right)
            point.x = point.x - region.cx;
        else if (flags & core::math::align::centerX)
            point.x = point.x - region.cx / 2;
        else {}

        if (flags & core::math::align::bottom)
            point.y = point.y - region.cy;
        else if (flags & core::math::align::centerY)
            point.y = point.y - region.cy / 2;
        else {}

        _graphics->DrawImage(*(image.image()), point, region);
    }

    void Graphics::DrawImage(graphics::Image & image, rc32_t rect, core::math::rc32_t region, int32_t flags)
    {
        if (!_graphics)
            return;

        if (region.right() > image.size().cx)
            region.setRight(image.size().cx);
        if (region.bottom() > image.size().cy)
            region.setBottom(image.size().cy);

        pt32_t point;
        if (flags & core::math::align::right)
            point.x = rect.right() - region.cx;
        else if (flags & core::math::align::centerX)
            point.x = rect.centerX() - region.cx / 2;
        else
            point.x = rect.x;

        if (flags & core::math::align::bottom)
            point.y = rect.bottom() - region.cy;
        else if (flags & core::math::align::centerY)
            point.y = rect.centerY() - region.cy / 2;
        else
            point.y = rect.y;

        _graphics->PushClip(rect);
        _graphics->DrawImage(*(image.image()), point, region);
        _graphics->PopClip();
    }


    void Graphics::DrawImage(graphics::Image & image, rc32_t rect)
    {
        if (!_graphics)
            return;

        _graphics->DrawImage(*(image.image()), rect);
    }

    void Graphics::DrawImage(graphics::Image & image, core::math::rc32_t rect, core::math::rc32_t region)
    {
        if (!_graphics)
            return;

        _graphics->DrawImage(*(image.image()), rect, region);
    }

    graphics::fontmetrics Graphics::GetFontMetrics(graphics::font font)
    {
        if (!_graphics)
            return {};

        return _graphics->GetFontMetrics(font);
    }
    core::math::si32_t Graphics::MeasureString(std::string str, graphics::font font)
    {
        if (!_graphics)
            return {};

        return _graphics->MeasureString(str, font);
    }
}
