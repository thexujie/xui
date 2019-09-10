#pragma once

#include "win32/win32.h"
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>

namespace win32::DXGI
{
	const char8_t * DXGIFormatName(DXGI_FORMAT dxgiformat);
}
