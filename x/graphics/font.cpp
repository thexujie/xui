#include "stdafx.h"
#include "font.h"
#include "win32/windows.h"

namespace graphics
{
    font::font() : weight(font_weight::normal), width(font_width::normal), slant(font_slant::upright)
    {
        NONCLIENTMETRICSW metrics = { sizeof(NONCLIENTMETRICSW) };
        SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, metrics.cbSize, &metrics, 0);

        family = core::wstr_u8str(metrics.lfMessageFont.lfFaceName);
        if(metrics.lfMessageFont.lfHeight < 0)
            size = -metrics.lfMessageFont.lfHeight;
        else
        {
            HDC hdc = GetDC(HWND_DESKTOP);
            int dpiy = GetDeviceCaps(hdc, LOGPIXELSY);
            size = metrics.lfMessageFont.lfHeight * 96.0f / dpiy;
            ReleaseDC(HWND_DESKTOP, hdc);
        }
    }

    font::font(const char * family_, float_t size_, font_weight weight_, font_width width_, font_slant slant_)
        : weight(weight_), width(width_), slant(slant_)
    {
        if(!family_ || !family_[0] || size_ <= 0)
        {
            NONCLIENTMETRICSW metrics = { sizeof(NONCLIENTMETRICSW) };
            SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, metrics.cbSize, &metrics, 0);

            if(!family_ || !family_[0])
                family = core::wstr_u8str(metrics.lfMessageFont.lfFaceName);
            else
                family = family_;
            if(size_ <= 0)
                size = metrics.lfMessageFont.lfHeight;
            else
                size = size_;
        }
        else
        {
            family = family_;
            size = size_;
        }
    }

    extern  void __fontmetrics(const graphics::font & font, graphics::fontmetrics & metrics);
    fontmetrics::fontmetrics(const font & font)
    {
        __fontmetrics(font, *this);
    }
}
