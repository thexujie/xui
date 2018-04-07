#include "stdafx.h"
#include "Graphics.h"
#include "win32/windows.h"
#include "Image.h"

namespace win32
{
    using namespace core;

    Graphics::Graphics(std::shared_ptr<win32::Bitmap> bitmap) : _bitmap(bitmap), _hdc(bitmap->hdc()), _objCache(std::make_shared<GDIObjectCache>(bitmap->hdc())){}

    core::error_e Graphics::Clear(core::color32 color)
    {
        if (!_hdc)
            return error_state;

        std::shared_ptr<win32::Bitmap> bitmap = std::dynamic_pointer_cast<win32::Bitmap>(_bitmap);
        if (bitmap)
            bitmap->Clear(color);
        else
        {
            int32_t cx = GetDeviceCaps((HDC)_hdc, VERTRES);
            int32_t cy = GetDeviceCaps((HDC)_hdc, HORZRES);
            FillRect({ 0, 0, cx, cy }, AffineColor(color));
        }
        return error_ok;
    }

    core::error_e Graphics::DrawLine(core::math::pt32_t start, core::math::pt32_t end, core::color32 color, float32_t width)
    {
        if (!_hdc || !_objCache)
            return error_state;

        ::SelectObject((HDC)_hdc, _objCache->GetPen(AffineColor(color), width));
        MoveToEx((HDC)_hdc, start.x, start.y, NULL);
        LineTo((HDC)_hdc, end.x, end.y);
        return error_ok;
    }

    core::error_e Graphics::DrawRect(core::math::rc32_t rect, core::color32 color, float32_t width)
    {
        if (!_hdc || !_objCache)
            return error_state;

        RECT rc = { rect.x, rect.y, rect.cx, rect.cy };
        ::SelectObject((HDC)_hdc, _objCache->GetBrush(colors::Transparent));
        ::SelectObject((HDC)_hdc, _objCache->GetPen(AffineColor(color), width));
        ::Rectangle((HDC)_hdc, rect.x, rect.y, rect.right(), rect.bottom());
        return error_ok;
    }

    core::error_e Graphics::FillRect(core::math::rc32_t rect, color32 color)
    {
        if (!_hdc || !_objCache)
            return error_state;

        RECT rc = { rect.x, rect.y, rect.cx, rect.cy };
        ::SelectObject((HDC)_hdc, _objCache->GetBrush(AffineColor(color)));
        ::SelectObject((HDC)_hdc, _objCache->GetPen(colors::Transparent, 1));
        ::Rectangle((HDC)_hdc, rect.x, rect.y, rect.right() + 1, rect.bottom() + 1);
        return error_ok;
    }

    core::error_e Graphics::DrawString(std::string str, core::color32 color, graphics::font font, core::math::pt32_t point, int32_t flags)
    {
        if (!_hdc || !_objCache)
            return error_state;

        std::wstring strw = core::string::u8_ucs2(str);
        ::SetTextColor((HDC)_hdc, AffineColor(color).argb & 0x00FFFFFF);
        ::SelectObject((HDC)_hdc, (HFONT)_objCache->GetFont(font));
        ::SetBkMode((HDC)_hdc, TRANSPARENT);

        TEXTMETRICW tm;
        ::GetTextMetricsW((HDC)_hdc, &tm);

        if(flags & core::math::align::mask)
        {
            uint32_t uiAlign = 0;
            if (flags & core::math::align::right)
                uiAlign |= TA_RIGHT;
            if (flags & core::math::align::centerX)
                uiAlign |= TA_CENTER;
            else
                uiAlign |= TA_LEFT;

            if (flags & core::math::align::bottom)
            {
                uiAlign |= TA_BOTTOM;
            }
            else if (flags & core::math::align::centerY)
            {
                uiAlign |= TA_TOP;
                point.y -= tm.tmHeight / 2;
            }
            else
                uiAlign |= TA_TOP;
            SetTextAlign((HDC)_hdc, uiAlign);
        }

        ::TextOutW((HDC)_hdc, point.x, point.y + tm.tmExternalLeading, strw.c_str(), strw.length());
        return error_ok;
    }

    core::error_e Graphics::DrawString(std::string str, core::color32 color, graphics::font font, core::math::rc32_t rect, int32_t flags)
    {
        if (!_hdc || !_objCache)
            return error_state;

        std::wstring strw = core::string::u8_ucs2(str);
        ::SetTextColor((HDC)_hdc, AffineColor(color).argb & 0x00FFFFFF);
        ::SelectObject((HDC)_hdc, (HFONT)_objCache->GetFont(font));
        ::SetBkMode((HDC)_hdc, TRANSPARENT);

        TEXTMETRICW tm;
        ::GetTextMetricsW((HDC)_hdc, &tm);

        core::math::pt32_t point;
        uint32_t uiAlign = 0;
        if (flags & core::math::align::right)
        {
            uiAlign |= TA_RIGHT;
            point.x = rect.right();
        }
        else if (flags & core::math::align::centerX)
        {
            uiAlign |= TA_CENTER;
            point.x = rect.centerX();
        }
        else
        {
            uiAlign |= TA_LEFT;
            point.x = rect.x;
        }

        if (flags & core::math::align::bottom)
        {
            uiAlign |= TA_BOTTOM;
            point.y = rect.bottom();
        }
        else if (flags & core::math::align::centerY)
        {
            uiAlign |= TA_TOP;
            point.y = rect.centerY() - tm.tmHeight / 2;
        }
        else
        {
            uiAlign |= TA_TOP;
            point.y = rect.y;
        }

        SetTextAlign((HDC)_hdc, uiAlign);

        winrect_t winrect = rect;
        ::ExtTextOutW((HDC)_hdc, point.x, point.y + tm.tmExternalLeading, /*ETO_CLIPPED*/0, &winrect, strw.c_str(), strw.length(), nullptr);
        return error_ok;
    }

    core::error_e Graphics::DrawImage(graphics::IImage & image, core::math::pt32_t point, int32_t flags)
    {
        auto & win32image = dynamic_cast<win32::Image &>(image);

        HGDIOBJ hBitmapOld = ::SelectObject((HDC)win32image.hdc(), win32image.bitmap());
        if (win32image.cmode() == graphics::image::cmode_a8r8g8b8)
        {
            BLENDFUNCTION bfun = {};
            bfun.BlendOp = AC_SRC_OVER;
            bfun.SourceConstantAlpha = 0xff;
            bfun.AlphaFormat = AC_SRC_ALPHA;
            ::AlphaBlend((HDC)_hdc, point.x, point.y, win32image.size().cx, win32image.size().cy, (HDC)win32image.hdc(), 0, 0, win32image.size().cx, win32image.size().cy, bfun);
        }
        else
        {
            ::BitBlt((HDC)_hdc, point.x, point.y, win32image.size().cx, win32image.size().cy, (HDC)win32image.hdc(), 0, 0, SRCCOPY);
        }
        ::SelectObject((HDC)win32image.hdc(), hBitmapOld);

        return error_ok;
    }

    core::error_e Graphics::DrawImage(graphics::IImage & image, core::math::rc32_t rect, int32_t flags)
    {
        auto & win32image = dynamic_cast<win32::Image &>(image);

        core::math::rc32_t point;
        if (flags & core::math::align::right)
        {
            point.x = rect.right() - image.size().cx;
        }
        else if (flags & core::math::align::centerX)
        {
            point.x = rect.centerX() - image.size().cx / 2;
        }
        else
        {
            point.x = rect.x;
        }

        if (flags & core::math::align::bottom)
        {
            point.y = rect.bottom() - image.size().cy;
        }
        else if (flags & core::math::align::centerY)
        {
            point.y = rect.centerY() - image.size().cy / 2;
        }
        else
        {
            point.y = rect.y;
        }

        PushClip(rect);

        HGDIOBJ hBitmapOld = ::SelectObject((HDC)win32image.hdc(), win32image.bitmap());
        if (win32image.cmode() == graphics::image::cmode_a8r8g8b8)
        {
            BLENDFUNCTION bfun = {};
            bfun.BlendOp = AC_SRC_OVER;
            bfun.SourceConstantAlpha = 0xff;
            bfun.AlphaFormat = AC_SRC_ALPHA;
            ::GdiAlphaBlend((HDC)_hdc, point.x, point.y, win32image.size().cx, win32image.size().cy, (HDC)win32image.hdc(), 0, 0, win32image.size().cx, win32image.size().cy, bfun);
        }
        //else if(rect.size != win32image.size())
        //{
        //    //BLENDFUNCTION bfun = {};
        //    //bfun.BlendOp = AC_SRC_OVER;
        //    //bfun.SourceConstantAlpha = 0xff;
        //    //bfun.AlphaFormat = AC_SRC_ALPHA;
        //    //::GdiAlphaBlend((HDC)_hdc, rect.x, rect.y, rect.cx, rect.cy, (HDC)win32image.hdc(), 0, 0, win32image.size().cx, win32image.size().cy, bfun);

        //    //::SetStretchBltMode((HDC)_hdc, COLORONCOLOR);
        //    //::SetStretchBltMode((HDC)_hdc, STRETCH_HALFTONE);
        //    //::StretchBlt((HDC)_hdc, rect.x, rect.y, rect.cx, rect.cy, (HDC)win32image.hdc(), 0, 0, win32image.size().cx, win32image.size().cy, SRCCOPY);
        //}
        else
        {
            ::BitBlt((HDC)_hdc, point.x, point.y, win32image.size().cx, win32image.size().cy, (HDC)win32image.hdc(), 0, 0, SRCCOPY);
        }
        ::SelectObject((HDC)win32image.hdc(), hBitmapOld);
        PopClip();
        return error_ok;
    }

    color32 Graphics::AffineColor(color32 color)
    {
        color32 color2 = (color & 0xFF000000) |
            ((color & 0x0000FF) << 16) |
            ((color & 0x00FF00) << 0) |
            ((color & 0xFF0000) >> 16);
        return color2;
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
            ::SelectClipRgn(_hdc, NULL);
            ::DeleteObject(_clipRegion);
            _clipRegion = NULL;
        }

        winrect_t winrect = rect;
        _clipRegion = ::CreateRectRgnIndirect(&winrect);
        ::SelectClipRgn(_hdc, _clipRegion);
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
    }

}
