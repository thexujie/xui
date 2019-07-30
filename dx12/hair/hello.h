#pragma once

#include "win32/win32.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

void LoadPipeLine(HWND hwnd, core::sizei size);
void LoadAssets();
void OnRender();
