#include "stdafx.h"
#include "Graphics.h"
#include "win32/windows.h"
#include "core/io/filestream.h"
#include "uniscribe/script.h"

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

    void Graphics::PushOrign(core::math::pt32i point)
    {
        core::math::pt32i temp = point;
        if (!_origns.empty())
            temp += _origns.top();
        _origns.push(temp);
    }

    core::math::pt32i Graphics::GetOrign() const
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

    void Graphics::PushClip(core::math::rc32i rect)
    {
        if (!_origns.empty())
        {
            core::math::pt32i orign = _origns.top();
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

        if(rect.is_valid())
        {
            winrect_t winrect = rect;
            _clipRegion = ::CreateRectRgnIndirect(&winrect);
            ::SelectClipRgn(*_hdc, _clipRegion);
        }
        else
            ::SelectClipRgn(*_hdc, NULL);
    }

    core::math::rc32i Graphics::GetClip() const
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
            agg::pixfmt_bgra32 pixf(_rbuf);
            agg::renderer_base<agg::pixfmt_bgra32> renderer(pixf);
            renderer.clear(agg::tools::rgba(color));
        }
    }

    void Graphics::DrawString(std::string str, core::math::pt32i point, graphics::text::font font, core::color32 color)
    {
        if(!*_hdc || !_objCache)
            return;

        std::wstring strw = core::string::u8_u16(str);
        SetFont(_objCache->GetFont(font));

        ::SetTextColor(*_hdc, AffineColor(color).argb & 0x00FFFFFF);
        SetTextAlign(*_hdc, 0);

        TEXTMETRICW tm;
        ::GetTextMetricsW(*_hdc, &tm);

        ::TextOutW(*_hdc, point.x, point.y + tm.tmExternalLeading, strw.c_str(), (int32_t)strw.length());
    }

    void Graphics::DrawImage(const graphics::IGraphicsImage & image, core::math::pt32i point)
    {
        DrawImage(image, { point, image.size() });
    }

    void Graphics::DrawImage(const graphics::IGraphicsImage & image, core::math::pt32i point, core::math::rc32i region)
    {
        DrawImage(image, core::math::rc32i(point, region.size), region);
    }

    void Graphics::DrawImage(const graphics::IGraphicsImage & image, core::math::rc32i rect)
    {
        auto & data = image.data();
        agg::pixfmt_bgra32 pixf(_rbuf);
        agg::renderer_base<agg::pixfmt_bgra32> render(pixf);

        agg::path_storage ps;
        ps.move_to(rect.x, rect.y);
        ps.line_to(rect.right(), rect.y);
        ps.line_to(rect.right(), rect.bottom());
        ps.line_to(rect.x, rect.bottom());
        ps.close_polygon();
        _raster.reset();
        _raster.add_path(ps);

        agg::rendering_buffer img_buffer((agg::int8u *)data.data, data.format.width, data.format.height, data.pitch);

        agg::span_interpolator_linear<> interpolator(
            agg::trans_affine_translation(-rect.x, -rect.y) *
            agg::trans_affine_scaling(data.format.width / (double)rect.cx, data.format.height / (double)rect.cy));

        if(data.format.format == graphics::image::format_r8g8b8)
        {
            agg::pixfmt_bgr24 pixel_accessor(img_buffer);
            agg::span_image_filter_rgb_bilinear_clip<agg::pixfmt_bgr24, agg::span_interpolator_linear<>> sg(pixel_accessor, agg::rgba_pre(0, 0, 0, 0.5), interpolator);
            agg::span_allocator<agg::rgba8> sa;
            agg::render_scanlines_aa(_raster, _sl, render, sa, sg);
        }
        else if (data.format.format == graphics::image::format_a8r8g8b8)
        {
            agg::pixfmt_bgra32 pixel_accessor(img_buffer);
            agg::span_image_filter_rgba_bilinear_clip<agg::pixfmt_bgra32, agg::span_interpolator_linear<>> sg(pixel_accessor, agg::rgba_pre(0, 0, 0, 0.5), interpolator);
            agg::span_allocator<agg::rgba8> sa;
            agg::render_scanlines_aa(_raster, _sl, render, sa, sg);
        }
    }

    void Graphics::DrawImage(const graphics::IGraphicsImage & image, core::math::rc32i rect, core::math::rc32i region)
    {
        auto & data = image.data();
        agg::pixfmt_bgra32 pixf(_rbuf);
        agg::renderer_base<agg::pixfmt_bgra32> render(pixf);

        agg::path_storage ps;
        ps.move_to(rect.x, rect.y);
        ps.line_to(rect.right(), rect.y);
        ps.line_to(rect.right(), rect.bottom());
        ps.line_to(rect.x, rect.bottom());
        ps.close_polygon();
        _raster.reset();
        _raster.add_path(ps);

        agg::rendering_buffer img_buffer((agg::int8u *)data.data, data.format.width, data.format.height, data.pitch);

        agg::span_interpolator_linear<> interpolator(
            agg::trans_affine_translation(-rect.x, -rect.y) *
            agg::trans_affine_scaling(region.cx / (double)rect.cx, (double)region.cy / rect.cy)*
            agg::trans_affine_translation(region.x, region.y));

        if (data.format.format == graphics::image::format_r8g8b8)
        {
            agg::pixfmt_bgr24 pixel_accessor(img_buffer);
            agg::span_image_filter_rgb_bilinear_clip<agg::pixfmt_bgr24, agg::span_interpolator_linear<>> sg(pixel_accessor, agg::rgba_pre(0, 0, 0, 0.5), interpolator);
            agg::span_allocator<agg::rgba8> sa;
            agg::render_scanlines_aa(_raster, _sl, render, sa, sg);
        }
        else if (data.format.format == graphics::image::format_a8r8g8b8)
        {
            agg::pixfmt_bgra32 pixel_accessor(img_buffer);
            agg::span_image_filter_rgba_bilinear_clip<agg::pixfmt_bgra32, agg::span_interpolator_linear<>> sg(pixel_accessor, agg::rgba_pre(0, 0, 0, 0.5), interpolator);
            agg::span_allocator<agg::rgba8> sa;
            agg::render_scanlines_aa(_raster, _sl, render, sa, sg);
        }
    }

    void Graphics::DrawString(const std::string & str, core::math::pt32i point, const graphics::text::font & font, core::color32 color, int32_t flags)
    {
        if (!*_hdc || !_objCache)
            return;

        auto strW = core::string::u8_u16(str);
        if(_scpItemTemp.Text() != strW)
        {
            _scpItemTemp.SetText(strW);
            _scpItemTemp.Itemize();
            _scpItemTemp.SetFont(0, _scpItemTemp.NumClusters(), font);
            _scpItemTemp.SetColor(0, _scpItemTemp.NumClusters(), color);
            _scpItemTemp.Slice();
            _scpItemTemp.Shape();
            _scpItemTemp.Layout(0, -1, win32::uniscribe::wrapmode_none);
        }

        math::si32i size = _scpItemTemp.Size();
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

        _scpItemTemp.Draw(*_hdc, point.x, point.y, {});
    }

    void Graphics::DrawString(const std::string & str, core::math::rc32i rect, const graphics::text::font & font, core::color32 color, int32_t flags)
    {
        if (!*_hdc || !_objCache)
            return;

        auto strW = core::string::u8_u16(str);
        if (_scpItemTemp.Text() != strW)
        {
            _scpItemTemp.SetText(strW);
            _scpItemTemp.Itemize();
            _scpItemTemp.SetFont(0, _scpItemTemp.NumClusters(), font);
            _scpItemTemp.SetColor(0, _scpItemTemp.NumClusters(), color);
            _scpItemTemp.Slice();
            _scpItemTemp.Shape();
            _scpItemTemp.Layout(0, -1, win32::uniscribe::wrapmode_none);
        }

        math::si32i size = _scpItemTemp.Size();
        core::math::pt32i point = rect.leftTop();
        if (flags & core::math::align::right)
            point.x = rect.right() - size.cx;
        else if (flags & core::math::align::centerX)
            point.x = rect.x + (rect.cx - size.cx) / 2;
        else
            point.x = rect.x;

        if (flags & core::math::align::bottom)
            point.y = rect.bottom() - size.cy;
        else if (flags & core::math::align::centerY)
            point.y = rect.y + (rect.cy - size.cy) / 2;
        else
            point.y = rect.y;

        _scpItemTemp.Draw(*_hdc, point.x, point.y, {});
    }

    void Graphics::DrawString(graphics::IGraphicsString & str, core::math::pt32i point)
    {
        win32::uniscribe::ScriptItem & item = dynamic_cast<win32::uniscribe::ScriptItem &>(str);
        item.Draw(*_hdc, point.x, point.y, { });
    }

    void Graphics::FillPath(graphics::raster::path & path, core::color32 color)
    {
        agg::pixfmt_bgra32 pixf(_rbuf);
        agg::renderer_base<agg::pixfmt_bgra32> renderer(pixf);

        _raster.reset();
        _raster.add_path(path);
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

    core::math::si32i Graphics::MeasureString(std::string str, graphics::text::font font)
    {
        if(!*_hdc || !_objCache)
            return {};

        win32::uniscribe::ScriptItem item;
        item.SetText(core::string::u8_u16(str));
        item.Itemize();
        item.SetFont(0, item.NumClusters(), font);
        item.Slice();
        item.Shape();
        item.Layout(0, -1, win32::uniscribe::wrapmode_none);
        return item.Size();
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
