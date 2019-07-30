#include "stdafx.h"

//#include <ShellScalingAPI.h>
//#pragma comment(lib, "Shcore.lib")

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
//#pragma comment(lib, "E:/github/google/skia/out/x64d/skia.dll.lib")
#pragma comment(lib, "../../externals/skia/bin/x64/skia.dll.lib")
#include "main.h"

#include "hello.h"
#include "RHID3D12/RHID3D12.h"


LRESULT CALLBACK DefaultWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//if(message == WM_CREATE)
	//{
	//	CREATESTRUCT * cs = reinterpret_cast<CREATESTRUCT *>(lParam);
	//	Window * window = static_cast<Window *>(cs->lpCreateParams);
	//	if(window)
	//		window->attatch(hWnd);
	//}

	if (message == WM_PAINT)
	{
		PAINTSTRUCT pt;
		HDC hdc;
		hdc = BeginPaint(hWnd, &pt);
		EndPaint(hWnd, &pt);
	}

	else if (message == WM_CLOSE)
	{
		win32::endLoop(0);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

int main()
{
#if defined _DEBUG
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    core::App app;

	HINSTANCE hInstance = (HINSTANCE)win32::instance();

	WNDCLASSEXW wcex = { sizeof(WNDCLASSEXW) };

	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.style = 0;
	wcex.lpfnWndProc = DefaultWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = /*::LoadCursor(NULL, IDC_ARROW)*/NULL;
	//wcex.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = win32::WINDOW_CLASS_NAME;
	wcex.hIconSm = NULL;

	RegisterClassExW(&wcex);

	g_hwnd = CreateWindowExW(
		0, win32::WINDOW_CLASS_NAME, L"Title", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080,
		NULL, NULL, hInstance, NULL);

	::ShowWindow(g_hwnd, SW_NORMAL);
	::UpdateWindow(g_hwnd);


	RHI::RHID3D12::RHID3D12 RHI;
	RHI.Load();
	auto adapters = RHI.AdapterDescs();
	auto device = RHI.CreateDevice(adapters[0].uri);
	auto queue = device->CreateCommandQueue(RHI::CommandType::Direct, RHI::CommandQueueFlag::None);

	RHI::RenderTargetParams params =
	{
		.hwnd = g_hwnd,
	};
	auto rt = device->CreateRenderTargetForHWND(params);
	auto cmdlist = device->CreateCommandList(RHI::CommandType::Direct);

	RHI::BufferParams bufferParams = {};
	bufferParams.heap.type = RHI::HeapType::Default;
	bufferParams.size.cx = 1024;
	bufferParams.dimension = RHI::BufferDimension::Raw;
	bufferParams.states = RHI::ResourceState::VertexBuffer;
	auto vb = device->CreateBuffer(bufferParams);

	RECT rcClient;
	GetClientRect(g_hwnd, &rcClient);

	core::recti sccisorrect = { 0, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top };
	RHI::ViewPort viewport = { 0, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top };


	core::counter_fps<float, 3> fps;
	while(true)
	{
		rt->Begin();
		cmdlist->Reset();
		cmdlist->SetViewPort(viewport);
		cmdlist->SetScissorRect(sccisorrect);
		cmdlist->TransitionBarrier(rt.get(), RHI::ResourceState::RenderTarget);
		cmdlist->SetRenderTarget(rt.get());
		cmdlist->ClearRenderTarget(core::colors::Red);
		cmdlist->TransitionBarrier(rt.get(), RHI::ResourceState::Present);
		cmdlist->Close();
		rt->Excute(cmdlist.get());
		rt->End();
		rt->Present(0);
		fps.acc(1);

		core::logger_fps tt(__FILE__, __LINE__, 10);
		if (tt.ok())
			printf("\rfps = %.6f", fps.fps());
	}

    //LoadPipeLine((HWND)form->handle(), form->rect().round<int32_t>().size);
    //LoadAssets();

    //window->message += [](uint32_t msg, uintx_t wparam, intx_t lparam)
    //{
    //    if (msg == WM_PAINT)
    //        OnRender();
    //};

    //form->closed += []() {core::app().quit(0);};
    //form->show();

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
