#include "stdafx.h"

#include <ShellScalingAPI.h>
#pragma comment(lib, "Shcore.lib")

//#pragma comment(lib, "E:/github/google/skia/out/x64d/skia.dll.lib")
#pragma comment(lib, "../externals/skia/bin/x64/skia.dll.lib")

void all_main();
void views_main();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	HMODULE Shcore = GetModuleHandleW(L"Shcore.dll");
	auto pfn_SetProcessDpiAwareness = reinterpret_cast<decltype(SetProcessDpiAwareness) *>(GetProcAddress(Shcore, "SetProcessDpiAwareness"));
	if(pfn_SetProcessDpiAwareness)
		pfn_SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);

	auto app = std::make_shared<win32::Win32App>();
    views_main();
	return win32::runLoop();
}

int main()
{
	HMODULE Shcore = GetModuleHandleW(L"Shcore.dll");
	auto pfn_SetProcessDpiAwareness = reinterpret_cast<decltype(SetProcessDpiAwareness) *>(GetProcAddress(Shcore, "SetProcessDpiAwareness"));
	if(pfn_SetProcessDpiAwareness)
		pfn_SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);

    auto app = std::make_shared<win32::Win32App>();
	views_main();
	return win32::runLoop();
}
