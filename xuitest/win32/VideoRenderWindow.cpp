#include "stdafx.h"
#include "VideoRenderWindow.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace win32
{
    static const wchar_t WINDOW_PROP_THIS_PTR[] = L"C8C8BD2D-46A7-4DFB-BB5D-EE6A25E83368";

    static LRESULT CALLBACK VideoRenderWindowWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        VideoRenderWindow * player = static_cast<VideoRenderWindow *>(GetPropW(hWnd, WINDOW_PROP_THIS_PTR));
        if (player)
            return player->WndProc(reinterpret_cast<void *>(hWnd), message, wParam, lParam);
        else
            return CallWindowProcW(DefWindowProcW, hWnd, message, wParam, lParam);
    }

    VideoRenderWindow::VideoRenderWindow()
    {
        HINSTANCE hInstance = GetModuleHandle(NULL);

        WNDCLASSEXW wcex = { sizeof(WNDCLASSEXW) };
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = VideoRenderWindowWndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = NULL;
        wcex.hCursor = NULL;
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = L"D3D9SurfaceVideoPlayer";
        wcex.hIconSm = NULL;

        RegisterClassExW(&wcex);

        _hWnd = CreateWindowExW(0, L"D3D9SurfaceVideoPlayer", L"VideoPlayer", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
        ShowWindow((HWND)_hWnd, SW_SHOW);

        SetPropW((HWND)_hWnd, WINDOW_PROP_THIS_PTR, (void *)(VideoRenderWindow *)this);
    }


    VideoRenderWindow::~VideoRenderWindow()
    {

    }

    int64_t VideoRenderWindow::WndProc(void * pWnd, uint32_t message, uint64_t wParam, int64_t lParam)
    {
        switch (message)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_ERASEBKGND:
            return 0;
        default:
            break;
        }
        return CallWindowProcW(DefWindowProcW, (HWND)pWnd, message, wParam, lParam);
    }
}
