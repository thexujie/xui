#include "stdafx.h"
#include "font.h"
#include "win32/win32.h"

static drawing::font default_font;
static std::once_flag flag;

void generate_default_font()
{
    NONCLIENTMETRICSW metrics = { sizeof(NONCLIENTMETRICSW) };
    SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, metrics.cbSize, &metrics, 0);
    default_font.family = core::wstr_u8str(metrics.lfMessageFont.lfFaceName);
    if (metrics.lfMessageFont.lfHeight < 0)
        default_font.size = (float32_t)-metrics.lfMessageFont.lfHeight;
    else
    {
        HDC hdc = GetDC(HWND_DESKTOP);
        int dpiy = GetDeviceCaps(hdc, LOGPIXELSY);
        default_font.size = metrics.lfMessageFont.lfHeight * 96.0f / dpiy;
        ReleaseDC(HWND_DESKTOP, hdc);
    }
}

namespace drawing
{
    font::font()
    {
        std::call_once(flag, generate_default_font);
        *this = default_font;
    }

    font::font(const char * family_, float_t size_, font_style style_)
        : style(style_)
    {
        if (!family_ || !family_[0] || size_ <= 0)
        {
            std::call_once(flag, generate_default_font);
            family = (family_ && family_[0]) ? family_ : default_font.family;
            size = size_ > 0 ? size_ : default_font.size;
        }
        else
        {
            family = family_;
            size = size_;
        }
    }

    extern void __fontmetrics(const drawing::font & font, drawing::fontmetrics & metrics);

    fontmetrics::fontmetrics(const font & font)
    {
        __fontmetrics(font, *this);
    }
}
