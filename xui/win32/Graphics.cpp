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
        if(!*_hdc || !_objCache)
            return;

        SetPen(_objCache->GetPen(AffineColor(color), width));

        MoveToEx(*_hdc, start.x, start.y, NULL);
        LineTo(*_hdc, end.x, end.y);
    }

    void Graphics::DrawRect(core::math::rc32_t rect, core::color32 color, float32_t width)
    {
        if(!*_hdc || !_objCache)
            return;

        SetPen(_objCache->GetPen(AffineColor(color), width));
        SetBrush(_objCache->GetBrush(AffineColor(colors::Transparent)));

        ::Rectangle(*_hdc, rect.x, rect.y, rect.right(), rect.bottom());
    }

    void Graphics::FillRect(core::math::rc32_t rect, color32 color)
    {
        if(!*_hdc || !_objCache)
            return;

        RECT rc = { rect.x, rect.y, rect.cx, rect.cy };

        SetPen(_objCache->GetPen(AffineColor(colors::Transparent), 1));
        SetBrush(_objCache->GetBrush(AffineColor(color)));

        ::Rectangle(*_hdc, rect.x, rect.y, rect.right() + 1, rect.bottom() + 1);
    }

    void Graphics::DrawString(std::string str, core::math::pt32_t point, graphics::font font, core::color32 color)
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

    void Graphics::DrawImage(graphics::IImage & image, core::math::pt32_t point)
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

    void Graphics::DrawImage(graphics::IImage & image, core::math::pt32_t point, core::math::rc32_t region)
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

    void Graphics::DrawImage(graphics::IImage & image, core::math::rc32_t rect)
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

    void Graphics::DrawImage(graphics::IImage & image, core::math::rc32_t rect, core::math::rc32_t region)
    {
        auto & win32image = dynamic_cast<win32::Image &>(image);
        HGDIOBJ hBitmapOld = ::SelectObject(*_hdcStaging, win32image.bitmap());
        if (win32image.cmode() == graphics::image::cmode_a8r8g8b8)
        {
            BLENDFUNCTION bfun = { AC_SRC_OVER, 0, 0xff, AC_SRC_ALPHA };
            ::GdiAlphaBlend(*_hdc, rect.x, rect.y, rect.cx, rect.cy, *_hdcStaging, region.x, region.y, region.cx, region.cy, bfun);
        }
        else if (rect.size != image.size())
        {
            ::StretchBlt(*_hdc, rect.x, rect.y, rect.cx, rect.cy, *_hdcStaging.get(), region.x, region.y, region.cx, region.cy, SRCCOPY);
        }
        else
        {
            ::BitBlt(*_hdc, rect.x, rect.y, rect.cx, rect.cy, *_hdcStaging.get(), region.x, region.y, SRCCOPY);
        }
        ::SelectObject(*_hdcStaging, hBitmapOld);
    }

    graphics::fontmetrics Graphics::GetFontMetrics(graphics::font font)
    {
        SetFont(_objCache->GetFont(font));
        TEXTMETRICW tm;
        ::GetTextMetricsW(*_hdc, &tm);

        graphics::fontmetrics metrics;
        metrics.size = font.size;
        metrics.height = tm.tmHeight;
        metrics.ascent = tm.tmAscent;
        metrics.descent= tm.tmDescent;
        metrics.linespace = tm.tmHeight;
        metrics.weight = tm.tmWeight;
        return metrics;
    }

    core::math::si32_t Graphics::MeasureString(std::string str, graphics::font font)
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
