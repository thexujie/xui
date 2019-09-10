#pragma once

#include "core/core.h"
#include "drawing/drawing.h"

namespace platform
{
    drawing::font default_font();
	void font_metrics(const drawing::font & font, drawing::fontmetrics & metrics);
}
