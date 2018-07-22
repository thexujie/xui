#include "stdafx.h"
#include "GraphicsService.h"
#include "win32/windows.h"

namespace win32
{
    graphics::text::fontmetrics GraphicsService::FontMetrics(const graphics::text::font & font)
    {
        TEXTMETRICW tm = {};
        HDC hdcScreen = GetDC(NULL);
        ::GetTextMetricsW(*_hdc, &tm);
        ::ReleaseDC(NULL, hdcScreen);

        graphics::text::fontmetrics metrics;
        metrics.size = font.size;
        metrics.height = tm.tmHeight;
        metrics.ascent = tm.tmAscent;
        metrics.descent = tm.tmDescent;
        metrics.linespace = tm.tmHeight;
        metrics.weight = tm.tmWeight;
        return metrics;
    }
}
