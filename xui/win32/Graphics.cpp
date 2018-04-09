#include "stdafx.h"
#include "Graphics.h"
#include "win32/windows.h"
#include "Image.h"

namespace win32
{
    using namespace core;

    Graphics::Graphics(std::shared_ptr<win32::Bitmap> bitmap) : _bitmap(bitmap), _hdc(bitmap->hdc()), _objCache(std::make_shared<GDIObjectCache>(bitmap->hdc()))
    {
        //::SetStretchBltMode(*_hdc, COLORONCOLOR);
        ::SetStretchBltMode(*_hdc, STRETCH_HALFTONE);
        ::SetBkMode(*_hdc, TRANSPARENT);
        _hdcStaging = std::make_shared<HDC>();
        *_hdcStaging = ::CreateCompatibleDC(*bitmap->hdc());

        graphics::pixmap_buffer buffer = bitmap->buffer();
        _rbuf.attach(reinterpret_cast<agg::int8u *>(buffer.data), buffer.size.cx, buffer.size.cy, buffer.flip_y ? -buffer.pitch : buffer.pitch);
        _raster.clip_box(0, 0, buffer.size.cx, buffer.size.cy);
    }

    void Graphics::PushOrign(core::math::pt32_t point)
    {
        core::math::pt32_t temp = point;
        if (!_origns.empty())
            temp += _origns.top();
        _origns.push(temp);
    }

    core::math::pt32_t Graphics::GetOrign() const
    {
        if (_origns.empty())
            return {};
        else
            return _origns.top();
    }

    void Graphics::PopOrign()
    {
        _origns.pop();
    }

    void Graphics::PushClip(core::math::rc32_t rect)
    {
        if (!_origns.empty())
        {
            core::math::pt32_t orign = _origns.top();
            rect.x += orign.x;
            rect.y += orign.y;
        }
        _clips.push(rect);

        if (_clipRegion)
        {
            ::SelectClipRgn(*_hdc, NULL);
            ::DeleteObject(_clipRegion);
            _clipRegion = NULL;
        }

        winrect_t winrect = rect;
        _clipRegion = ::CreateRectRgnIndirect(&winrect);
        ::SelectClipRgn(*_hdc, _clipRegion);
    }

    core::math::rc32_t Graphics::GetClip() const
    {
        if (_clips.empty())
            return {};
        else
            return _clips.top();
    }

    void Graphics::PopClip()
    {
        _clips.pop();
        if (_clips.empty())
            ::SelectClipRgn(*_hdc, NULL);
        else
        {
            if (_clipRegion)
            {
                ::SelectClipRgn(*_hdc, NULL);
                ::DeleteObject(_clipRegion);
                _clipRegion = NULL;
            }

            winrect_t rect(_clips.top());
            _clipRegion = ::CreateRectRgnIndirect(&rect);
            ::SelectClipRgn(*_hdc, _clipRegion);
        }
    }

    void Graphics::Clear(core::color32 color)
    {
        if(!*_hdc)
            return;

        std::shared_ptr<win32::Bitmap> bitmap = std::dynamic_pointer_cast<win32::Bitmap>(_bitmap);
        if(bitmap)
            bitmap->Clear(color);
        else
        {
            int32_t cx = GetDeviceCaps(*_hdc, VERTRES);
            int32_t cy = GetDeviceCaps(*_hdc, HORZRES);
            FillRect({ 0, 0, cx, cy }, AffineColor(color));
        }
    }

    void Graphics::DrawLine(core::math::pt32_t start, core::math::pt32_t end, core::color32 color, float32_t width)
    {
        agg::pixfmt_bgra32 pixf(_rbuf);
        agg::renderer_base<agg::pixfmt_bgra32> renderer(pixf);

        agg::path_storage ps;
        agg::conv_stroke<agg::path_storage> pg(ps);
        pg.width(width);

        ps.move_to(start.x, start.y);
        ps.line_to(end.x, end.y);
        _raster.reset();
        _raster.add_path(pg);
        agg::render_scanlines_aa_solid(_raster, _sl, renderer, agg::tools::rgba(color));
    }

    void Graphics::DrawEllipse(core::math::rc32_t ellipse, core::color32 color, float32_t width)
    {
        agg::pixfmt_bgra32 pixf(_rbuf);
        agg::renderer_base<agg::pixfmt_bgra32> renderer(pixf);

        agg::ellipse path(ellipse.centerX(), ellipse.centerY(), ellipse.cx / 2, ellipse.cy / 2);
        agg::conv_stroke<agg::ellipse> pg(path);
        pg.width(width);

        _raster.reset();
        _raster.add_path(pg);
        agg::render_scanlines_aa_solid(_raster, _sl, renderer, agg::tools::rgba(color));
    }

    void Graphics::FillEllipse(core::math::rc32_t ellipse, core::color32 color)
    {
        agg::pixfmt_bgra32 pixf(_rbuf);
        agg::renderer_base<agg::pixfmt_bgra32> renderer(pixf);

        agg::ellipse path(ellipse.centerX(), ellipse.centerY(), ellipse.cx / 2, ellipse.cy / 2);
        _raster.reset();
        _raster.add_path(path);
        agg::render_scanlines_aa_solid(_raster, _sl, renderer, agg::tools::rgba(color));
    }

    void Graphics::DrawRect(core::math::rc32_t rect, core::color32 color, float32_t width)
    {
        agg::pixfmt_bgra32 pixf(_rbuf);
        agg::renderer_base<agg::pixfmt_bgra32> renderer(pixf);

        agg::path_storage ps;
        agg::conv_stroke<agg::path_storage> pg(ps);
        pg.width(width);

        ps.move_to(rect.x, rect.y);
        ps.line_to(rect.right(), rect.y);
        ps.line_to(rect.right(), rect.bottom());
        ps.line_to(rect.x, rect.bottom());
        ps.close_polygon();
        _raster.reset();
        _raster.add_path(pg);
        agg::render_scanlines_aa_solid(_raster, _sl, renderer, agg::tools::rgba(color));
    }

    void Graphics::FillRect(core::math::rc32_t rect, color32 color)
    {
        agg::pixfmt_bgra32 pixf(_rbuf);
        agg::renderer_base<agg::pixfmt_bgra32> renderer(pixf);

        _raster.move_to_d(rect.x, rect.y);
        _raster.line_to_d(rect.right(), rect.y);
        _raster.line_to_d(rect.right(), rect.bottom());
        _raster.line_to_d(rect.x, rect.bottom());
        _raster.close_polygon();
        agg::render_scanlines_aa_solid(_raster, _sl, renderer, agg::tools::rgba(color));
    }

    void Graphics::DrawRoundRect(core::math::rc32_t rect, core::color32 color, float32_t width, float32_t radius)
    {
        agg::pixfmt_bgra32 pixf(_rbuf);
        agg::renderer_base<agg::pixfmt_bgra32> renderer(pixf);

        agg::rounded_rect path(rect.x, rect.y, rect.right(), rect.bottom(), radius);
        agg::conv_stroke<agg::rounded_rect> pg(path);
        pg.width(width);

        _raster.reset();
        _raster.add_path(pg);
        agg::render_scanlines_aa_solid(_raster, _sl, renderer, agg::tools::rgba(color));
    }

    void Graphics::FillRoundRect(core::math::rc32_t rect, core::color32 color, float32_t radius)
    {
        agg::pixfmt_bgra32 pixf(_rbuf);
        agg::renderer_base<agg::pixfmt_bgra32> renderer(pixf);

        agg::rounded_rect path(rect.x, rect.y, rect.right(), rect.bottom(), radius);

        _raster.reset();
        _raster.add_path(path);
        agg::render_scanlines_aa_solid(_raster, _sl, renderer, agg::tools::rgba(color));
    }

    void Graphics::DrawString(std::string str, core::math::pt32_t point, graphics::text::font font, core::color32 color)
    {
        if(!*_hdc || !_objCache)
            return;

        std::wstring strw = core::string::u8_ucs2(str);
        SetFont(_objCache->GetFont(font));

        ::SetTextColor(*_hdc, AffineColor(color).argb & 0x00FFFFFF);
        SetTextAlign(*_hdc, 0);

        TEXTMETRICW tm;
        ::GetTextMetricsW(*_hdc, &tm);

        ::TextOutW(*_hdc, point.x, point.y + tm.tmExternalLeading, strw.c_str(), (int32_t)strw.length());
    }

    void Graphics::DrawImage(graphics::IGraphicsImage & image, core::math::pt32_t point)
    {
        auto & win32image = dynamic_cast<win32::Image &>(image);

        HGDIOBJ hBitmapOld = ::SelectObject(*_hdcStaging, win32image.bitmap());
        if(win32image.cmode() == graphics::image::cmode_a8r8g8b8)
        {
            BLENDFUNCTION bfun = { AC_SRC_OVER, 0, 0xff, AC_SRC_ALPHA };
            ::GdiAlphaBlend(*_hdc, point.x, point.y, win32image.size().cx, win32image.size().cy, *_hdcStaging, 0, 0, win32image.size().cx, win32image.size().cy, bfun);
        }
        else
        {
            ::BitBlt(*_hdc, point.x, point.y, win32image.size().cx, win32image.size().cy, *_hdcStaging, 0, 0, SRCCOPY);
        }
        ::SelectObject(*_hdcStaging, hBitmapOld);
    }

    void Graphics::DrawImage(graphics::IGraphicsImage & image, core::math::pt32_t point, core::math::rc32_t region)
    {
        if(region.right() > image.size().cx)
            region.setRight(image.size().cx);
        if(region.bottom() > image.size().cy)
            region.setBottom(image.size().cy);

        auto & win32image = dynamic_cast<win32::Image &>(image);

        HGDIOBJ hBitmapOld = ::SelectObject(*_hdcStaging, win32image.bitmap());
        if(win32image.cmode() == graphics::image::cmode_a8r8g8b8)
        {
            BLENDFUNCTION bfun = { AC_SRC_OVER, 0, 0xff, AC_SRC_ALPHA };
            ::GdiAlphaBlend(*_hdc, point.x, point.y, region.cx, region.cy, *_hdcStaging, region.x, region.y, region.cx, region.cy, bfun);
        }
        else
        {
            ::BitBlt(*_hdc, point.x, point.y, region.cx, region.cy, *_hdcStaging, region.x, region.y, SRCCOPY);
        }
        ::SelectObject(*_hdcStaging, hBitmapOld);
    }

    void Graphics::DrawImage(graphics::IGraphicsImage & image, core::math::rc32_t rect)
    {
        auto & win32image = dynamic_cast<win32::Image &>(image);
        HGDIOBJ hBitmapOld = ::SelectObject(*_hdcStaging, win32image.bitmap());
        if(win32image.cmode() == graphics::image::cmode_a8r8g8b8)
        {
            BLENDFUNCTION bfun = { AC_SRC_OVER, 0, 0xff, AC_SRC_ALPHA };
            ::GdiAlphaBlend(*_hdc, rect.x, rect.y, rect.cx, rect.cy, *_hdcStaging, 0, 0, image.size().cx, image.size().cy, bfun);
        }
        else if (rect.size != image.size())
        {
            ::StretchBlt(*_hdc, rect.x, rect.y, rect.cx, rect.cy, *_hdcStaging.get(), 0, 0, image.size().cx, image.size().cy, SRCCOPY);
        }
        else
        {
            ::BitBlt(*_hdc, rect.x, rect.y, rect.cx, rect.cy, *_hdcStaging.get(), 0, 0, SRCCOPY);
        }
        ::SelectObject(*_hdcStaging, hBitmapOld);
    }

    void Graphics::DrawImage(graphics::IGraphicsImage & image, core::math::rc32_t rect, core::math::rc32_t region)
    {
        auto & win32image = dynamic_cast<win32::Image &>(image);
        HGDIOBJ hBitmapOld = ::SelectObject(*_hdcStaging, win32image.bitmap());
        if (win32image.cmode() == graphics::image::cmode_a8r8g8b8)
        {
            BLENDFUNCTION bfun = { AC_SRC_OVER, 0, 0xff, AC_SRC_ALPHA };
            ::GdiAlphaBlend(*_hdc, rect.x, rect.y, rect.cx, rect.cy, *_hdcStaging, region.x, region.y, region.cx, region.cy, bfun);
        }
        else if (rect.size != region.size)
        {
            ::StretchBlt(*_hdc, rect.x, rect.y, rect.cx, rect.cy, *_hdcStaging.get(), region.x, region.y, region.cx, region.cy, SRCCOPY);
        }
        else
        {
            ::BitBlt(*_hdc, rect.x, rect.y, rect.cx, rect.cy, *_hdcStaging.get(), region.x, region.y, SRCCOPY);
        }
        ::SelectObject(*_hdcStaging, hBitmapOld);
    }

    void Graphics::DrawString(graphics::IGraphicsString & str, core::math::pt32_t point)
    {
        
    }

    void Graphics::FillPath(graphics::raster::path & path, core::color32 color)
    {
        agg::pixfmt_bgra32 pixf(_rbuf);
        agg::renderer_base<agg::pixfmt_bgra32> renderer(pixf);

        graphics::raster::command cmd = graphics::raster::none;
        int32_t status = 0;
        float64_t x = 0;
        float64_t y = 0;

        _raster.reset();
        while (path.vertex(status, x, y, cmd))
        {
            switch(cmd)
            {
            case graphics::raster::none:
                break;
            case graphics::raster::move:
                _raster.move_to_d(x, y);
                break;
            case graphics::raster::line:
                _raster.line_to_d(x, y);
                break;
            case graphics::raster::close:
                _raster.close_polygon();
                break;
            }
        }
        agg::render_scanlines_aa_solid(_raster, _sl, renderer, agg::tools::rgba(color));
    }

    graphics::text::fontmetrics Graphics::GetFontMetrics(graphics::text::font font)
    {
        SetFont(_objCache->GetFont(font));
        TEXTMETRICW tm;
        ::GetTextMetricsW(*_hdc, &tm);

        graphics::text::fontmetrics metrics;
        metrics.size = font.size;
        metrics.height = tm.tmHeight;
        metrics.ascent = tm.tmAscent;
        metrics.descent= tm.tmDescent;
        metrics.linespace = tm.tmHeight;
        metrics.weight = tm.tmWeight;
        return metrics;
    }

    core::math::si32_t Graphics::MeasureString(std::string str, graphics::text::font font)
    {
        if(!*_hdc || !_objCache)
            return {};

        std::wstring strw = core::string::u8_ucs2(str);
        int32_t fit = 0;
        SIZE size = {};

        SetFont(_objCache->GetFont(font));
        ::GetTextExtentExPointW(*_hdc, strw.c_str(), (int32_t)strw.length(), std::numeric_limits<uint32_t>::max() / 2, &fit, NULL, &size);
        return { size.cx, size.cy };
    }

    color32 Graphics::AffineColor(color32 color)
    {
        color32 color2 = (color & 0xFF000000) |
            ((color & 0x0000FF) << 16) |
            ((color & 0x00FF00) << 0) |
            ((color & 0xFF0000) >> 16);
        return color2;
    }

    void Graphics::SetPen(std::shared_ptr<HPEN> pen)
    {
        if (_hdc)
            ::SelectObject(*_hdc, *pen);
    }

    void Graphics::SetBrush(std::shared_ptr<HBRUSH> brush)
    {
        if (_hdc)
            ::SelectObject(*_hdc, *brush);
    }

    void Graphics::SetFont(std::shared_ptr<HFONT> font)
    {
        if (_hdc)
            ::SelectObject(*_hdc, *font);
    }
}
