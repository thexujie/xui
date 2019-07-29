#include "stdafx.h"

//#include <ShellScalingAPI.h>
//#pragma comment(lib, "Shcore.lib")

//#pragma comment(lib, "E:/github/google/skia/out/x64d/skia.dll.lib")
#pragma comment(lib, "../../externals/skia/bin/x64/skia.dll.lib")

#include "hello.h"
#include "win32/Window.h"

int main()
{
#if defined _DEBUG
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    core::App app;

    auto form = std::make_shared<ui::Form>(core::vec2<core::dimenf>(100_em, 30_em));
    auto window = std::dynamic_pointer_cast<win32::Window>(form->window());
    form->centerScreen();

    LoadPipeLine((HWND)form->handle(), form->rect().round<int32_t>().size);
    LoadAssets();

    window->message += [](uint32_t msg, uintx_t wparam, intx_t lparam)
    {
        if (msg == WM_PAINT)
            OnRender();
    };

    form->closed += []() {core::app().quit(0);};
    form->show();

    return win32::runLoop();
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    //https://docs.microsoft.com/en-us/windows/desktop/api/shellscalingapi/nf-shellscalingapi-setprocessdpiawareness
    // 使用 manifest 而不是直接设置
    //HMODULE Shcore = GetModuleHandleW(L"Shcore.dll");
    //auto pfn_SetProcessDpiAwareness = reinterpret_cast<decltype(SetProcessDpiAwareness) *>(GetProcAddress(Shcore, "SetProcessDpiAwareness"));
    //if(pfn_SetProcessDpiAwareness)
    //	pfn_SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);

    main();
    return 0;
}
