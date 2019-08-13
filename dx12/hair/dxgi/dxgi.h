#pragma once

#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <dxgi1_4.h>
#include "x.h"
#include "win32/win32.h"

namespace win32::DXGI
{
	const char8_t * DXGIFormatName(DXGI_FORMAT dxgiformat);
	core::format ToPixelFormat(DXGI_FORMAT dxgiformat);
	DXGI_FORMAT FromPixelFormat(core::format pixelformat);
}
