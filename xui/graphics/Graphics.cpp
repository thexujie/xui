#include "stdafx.h"
#include "Graphics.h"
#include "raster/agg.h"

namespace graphics
{
    Graphics::Graphics(core::si32i size): _graphics(GraphicsService().CreateGraphics(GraphicsService().CreatePixmap(size))) { }

    Graphics::Graphics(std::shared_ptr<IGraphics> graphics) : _graphics(graphics) { }

    Graphics::Graphics(std::shared_ptr<Pixmap> pixmap): _graphics(GraphicsService().CreateGraphics(pixmap->handle())) { }

    void Graphics::PushOrign(core::pt32i point)
    {
        if (_graphics)
            _graphics->PushOrign(point);
    }

    core::pt32i Graphics::GetOrign() const
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

    void Graphics::PushClip(core::rc32i rect)
    {
        if (_graphics)
            _graphics->PushClip(rect);
    }

    core::rc32i Graphics::GetClip() const
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

    void Graphics::DrawLine(core::pt32i start, core::pt32i end, core::color32 color, float32_t width)
    {
        if (!_graphics)
            return;

        agg::path_storage ps;
        ps.move_to(start.x, start.y);
        ps.line_to(end.x, end.y);
        agg::conv_stroke<agg::path_storage> pg(ps);
        pg.width(width);

        _graphics->FillPath(pg, color);
    }

    void Graphics::DrawEllipse(core::rc32i ellipse, core::color32 color, float32_t width)
    {
        if (!_graphics)
            return;

        agg::ellipse path(ellipse.centerX(), ellipse.centerY(), ellipse.cx / 2, ellipse.cy / 2);
        agg::conv_stroke<agg::ellipse> stroke(path);
        _graphics->FillPath(stroke, color);
    }

    void Graphics::FillEllipse(core::rc32i ellipse, core::color32 color)
    {
        if (!_graphics)
            return;

        agg::ellipse path(ellipse.centerX(), ellipse.centerY(), ellipse.cx / 2, ellipse.cy / 2);
        _graphics->FillPath(path, color);
    }

    void Graphics::DrawRect(core::rc32i rect, core::color32 color, float32_t width)
    {
        if (!_graphics)
            return;

        agg::path_adaptor<5> path;
        path.push(rect.leftTop().to<float64_t>(), agg::path_cmd_move_to);
        path.push(rect.rightTop().to<float64_t>(), agg::path_cmd_line_to);
        path.push(rect.rightBottom().to<float64_t>(), agg::path_cmd_line_to);
        path.push(rect.leftBottom().to<float64_t>(), agg::path_cmd_line_to);
        path.close();
        agg::conv_stroke<decltype(path)> stroke(path);
        stroke.width(width);
        _graphics->FillPath(stroke, color);
    }

    void Graphics::FillRect(core::rc32i rect, core::color32 color)
    {
        if (!_graphics)
            return;

        agg::path_adaptor<5> path;
        path.push(rect.leftTop().to<float64_t>(), agg::path_cmd_move_to);
        path.push(rect.rightTop().to<float64_t>(), agg::path_cmd_line_to);
        path.push(rect.rightBottom().to<float64_t>(), agg::path_cmd_line_to);
        path.push(rect.leftBottom().to<float64_t>(), agg::path_cmd_line_to);
        path.close();
        _graphics->FillPath(path, color);
    }

    void Graphics::DrawRoundRect(core::rc32i rect, core::color32 color, float32_t width, float32_t radius)
    {
        if (!_graphics)
            return;

        agg::rounded_rect path(rect.x, rect.y, rect.right(), rect.bottom(), radius);
        agg::conv_stroke<decltype(path)> stroke(path);
        stroke.width(width);
        _graphics->FillPath(stroke, color);
    }

    void Graphics::FillRoundRect(core::rc32i rect, core::color32 color, float32_t radius)
    {
        if (!_graphics)
            return;

        agg::rounded_rect path(rect.x, rect.y, rect.right(), rect.bottom(), radius);
        _graphics->FillPath(path, color);
    }

    void Graphics::DrawString(const std::string & str, core::pt32i point, const text::font & font, core::color32 color, int32_t flags)
    {
        if (!_graphics)
            return;

        _graphics->DrawString(str, point, font, color, flags);
    }

    void Graphics::DrawString(const std::string & str, core::rc32i rect, const text::font & font, core::color32 color, int32_t flags)
    {
        if (!_graphics)
            return;

        _graphics->DrawString(str, rect, font, color, flags);
    }

    void Graphics::DrawString(IGraphicsString & str, core::pt32i point, const text::font & font, core::color32 color, int32_t flags)
    {
        if (!_graphics)
            return;

        _graphics->DrawString(str, point);
    }

    void Graphics::DrawString(IGraphicsString & str, core::pt32i point)
    {
        if (!_graphics)
            return;

        _graphics->DrawString(str, point);
    }

    void Graphics::DrawImage(const Image & image, core::pt32i point, int32_t flags)
    {
        if (!_graphics)
            return;

        auto size = image.size();
        if (flags & core::align::right)
            point.x -= size.cx;
        else if (flags & core::align::centerX)
            point.x -= size.cx / 2;
        else {}

        if (flags & core::align::bottom)
            point.y -= size.cy;
        else if (flags & core::align::centerY)
            point.y -= size.cy / 2;
        else {}

        _graphics->DrawImage(image, point);
    }

    void Graphics::DrawImage(const Image & image, core::rc32i rect, int32_t flags)
    {
        if (!_graphics)
            return;

        core::pt32i point;
        auto size = image.size();
        if (flags & core::align::right)
            point.x = rect.right() - size.cx;
        else if (flags & core::align::centerX)
            point.x = rect.centerX() - size.cx / 2;
        else
            point.x = rect.x;

        if (flags & core::align::bottom)
            point.y = rect.bottom() - size.cy;
        else if (flags & core::align::centerY)
            point.y = rect.centerY() - size.cy / 2;
        else
            point.y = rect.y;

        _graphics->PushClip(rect);
        _graphics->DrawImage(image, point);
        _graphics->PopClip();
    }

    void Graphics::DrawImage(const Image & image, core::pt32i point, core::rc32i region, int32_t flags)
    {
        if (!_graphics)
            return;

        if (region.right() > image.size().cx)
            region.setRight(image.size().cx);
        if (region.bottom() > image.size().cy)
            region.setBottom(image.size().cy);

        if (flags & core::align::right)
            point.x = point.x - region.cx;
        else if (flags & core::align::centerX)
            point.x = point.x - region.cx / 2;
        else {}

        if (flags & core::align::bottom)
            point.y = point.y - region.cy;
        else if (flags & core::align::centerY)
            point.y = point.y - region.cy / 2;
        else {}

        _graphics->DrawImage(image, point, region);
    }

    void Graphics::DrawImage(const Image & image, core::rc32i rect, core::rc32i region, int32_t flags)
    {
        if (!_graphics)
            return;

        if (region.right() > image.size().cx)
            region.setRight(image.size().cx);
        if (region.bottom() > image.size().cy)
            region.setBottom(image.size().cy);

        core::pt32i point;
        if (flags & core::align::right)
            point.x = rect.right() - region.cx;
        else if (flags & core::align::centerX)
            point.x = rect.centerX() - region.cx / 2;
        else
            point.x = rect.x;

        if (flags & core::align::bottom)
            point.y = rect.bottom() - region.cy;
        else if (flags & core::align::centerY)
            point.y = rect.centerY() - region.cy / 2;
        else
            point.y = rect.y;

        _graphics->PushClip(rect);
        _graphics->DrawImage(image, point, region);
        _graphics->PopClip();
    }


    void Graphics::DrawImage(const Image & image, core::rc32i rect)
    {
        if (!_graphics)
            return;

        _graphics->DrawImage(image, rect);
    }

    void Graphics::DrawImage(const Image & image, core::rc32i rect, core::rc32i region)
    {
        if (!_graphics)
            return;

        _graphics->DrawImage(image, rect, region);
    }

    void Graphics::FillPath(graphics::raster::path & path, core::color32 color)
    {
        if (!_graphics)
            return;

        _graphics->FillPath(path, color);
    }

    text::fontmetrics Graphics::GetFontMetrics(text::font font)
    {
        if (!_graphics)
            return {};

        return _graphics->GetFontMetrics(font);
    }

    core::si32i Graphics::MeasureString(std::string str, text::font font)
    {
        if (!_graphics)
            return {};

        return _graphics->MeasureString(str, font);
    }
}
