#include "stdafx.h"
#include "font.h"
#include "win32/windows.h"

namespace drawing
{
    font::font()
    {
        NONCLIENTMETRICSW metrics = { sizeof(NONCLIENTMETRICSW) };
        SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, metrics.cbSize, &metrics, 0);

        family = core::wstr_u8str(metrics.lfMessageFont.lfFaceName);
        if(metrics.lfMessageFont.lfHeight < 0)
            size = (float32_t)-metrics.lfMessageFont.lfHeight;
        else
        {
            HDC hdc = GetDC(HWND_DESKTOP);
            int dpiy = GetDeviceCaps(hdc, LOGPIXELSY);
            size = metrics.lfMessageFont.lfHeight * 96.0f / dpiy;
            ReleaseDC(HWND_DESKTOP, hdc);
        }
    }

    font::font(const char * family_, float_t size_, font_style style_)
        : style(style_)
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
                size = (float32_t)metrics.lfMessageFont.lfHeight;
            else
                size = size_;
        }
        else
        {
            family = family_;
            size = size_;
        }
    }

    extern  void __fontmetrics(const drawing::font & font, drawing::fontmetrics & metrics);
    fontmetrics::fontmetrics(const font & font)
    {
        __fontmetrics(font, *this);
    }
}
