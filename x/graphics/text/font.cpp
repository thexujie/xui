#include "stdafx.h"
#include "font.h"
#include "graphics/GraphicsService.h"
#include "win32/windows.h"

namespace graphics::text
{
    font::font() : weight(Weight_Normal), width(Width_Normal), slant(Slant_Upright)
    {
        NONCLIENTMETRICSW metrics = { sizeof(NONCLIENTMETRICSW) };
        SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, metrics.cbSize, &metrics, 0);

        family = core::string::u16_u8(metrics.lfMessageFont.lfFaceName);
        size = metrics.lfMessageFont.lfHeight;
    }

    font::font(const char * _family, float_t _size, int32_t _weight, int32_t _width, int32_t _slant, int32_t flags_)
        : weight(Weight_Normal), width(Width_Normal), slant(Slant_Upright)
    {
        if(!_family || !_family[0] || _size <= 0)
        {
            NONCLIENTMETRICSW metrics = { sizeof(NONCLIENTMETRICSW) };
            SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, metrics.cbSize, &metrics, 0);

            if(!_family || !_family[0])
                family = core::string::u16_u8(metrics.lfMessageFont.lfFaceName);
            else
                family = _family;
            if(_size <= 0)
                size = metrics.lfMessageFont.lfHeight;
            else
                size = _size;
        }
        else
        {
            family = _family;
            size = _size;
        }
        weight = font_weight(_weight);
        width = font_width(_width);
        slant = font_slant(_slant);
        flags = flags_;
    }

    fontmetrics font::metrics() const
    {
        return graphics::GraphicsService().FontMetrics(*this);
    }

    void font::makeDefault(font & font)
    {
        font = win32::defaultFont();
    }
}
