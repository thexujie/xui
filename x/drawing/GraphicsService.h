#pragma once

#include "drawing.h"

namespace drawing
{
	class GraphicsService
	{
	public:
		GraphicsService() = default;
		virtual ~GraphicsService() = default;

		virtual drawing::fontmetrics font_metrics(const drawing::font & font) = 0;
	};
}
