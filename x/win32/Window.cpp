#include "stdafx.h"
#include "Window.h"
#include "win32/windows.h"

namespace win32
{
    static LRESULT CALLBACK WindowWndProc(HWND hWnd, UINT uiMessage, WPARAM uiParam, LPARAM iParam);
    //static LRESULT CALLBACK WindowWndProc(HWND hWnd, UINT uiMessage, WPARAM uiParam, LPARAM iParam)
    //{
    //    if (uiMessage == WM_NCCREATE)
    //    {
    //        CREATESTRUCT * pcs = (CREATESTRUCT *)iParam;
    //        if (pcs->lpCreateParams)
    //        {
    //            ((Window *)pcs->lpCreateParams)->SetHandle(hWnd);
    //        }
    //    }

    //    return DefWindowProcW(hWnd, uiMessage, uiParam, iParam);
    //}

    Window::Window()
    {
        _style = WS_OVERLAPPEDWINDOW;
    }

    Window::~Window()
    {
        
    }

    core::error Window::attatch(std::shared_ptr<controls::Form> form)
    {
        _form = form;
        form->posChanged += std::weak_bind(&Window::onPosChagned, shared_from_this(), std::placeholders::_1, std::placeholders::_2);
        form->shownChanged += std::weak_bind(&Window::onShownChanged, shared_from_this(), std::placeholders::_1);
        return core::error_ok;
    }

    core::error Window::attatch(handle_t handle)
    {
        if (!handle)
            return core::error_args;

        HWND hwnd = (HWND)handle;

        if (GetPropW(hwnd, WINDOW_PROP_THIS_PTR))
            return core::error_state;

        WNDPROC pfnWndProcOld = (WNDPROC)GetWindowLongPtrW(hwnd, GWLP_WNDPROC);
        if (pfnWndProcOld == WindowWndProc)
            return core::error_state;

        SetPropW(hwnd, WINDOW_PROP_THIS_PTR, (HANDLE)(void *)this);
        SetPropW(hwnd, WINDOW_PROP_DLG_RESULT, (HANDLE)0);
        SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)WindowWndProc);
        SetPropW(hwnd, WINDOW_PROP_OLD_WNDPROC, (HANDLE)pfnWndProcOld);
        return core::error_ok;
    }

    handle_t Window::handle() const
    {
        if (!_handle)
        {
            auto err = const_cast<Window *>(this)->_createWindow();
        }
        return _handle;
    }

    void Window::onShownChanged(bool shown)
    {
        if(shown)
        {
            HWND hwnd = (HWND)handle();
            ::ShowWindow(hwnd, SW_SHOW);
        }
    }

    void Window::onPosChagned(const core::pt32f & from, const core::pt32f & to)
    {
        HWND hwnd = (HWND)handle();
        if (!hwnd)
            return;

        auto pos = _form->realPos().to<int32_t>();
        auto size = _form->realSize().convert(ceilf).to<int32_t>();
        ::MoveWindow(hwnd, pos.x, pos.y, size.cx, size.cy, FALSE);
    }

    core::error Window::_createWindow()
    {
        if (!_form)
        {
            core::logger::err() << __FUNCTIONW__ L" createWindow without form.";
            throw core::exception("createWindow without form");
        }

        if (_handle)
            return core::error_ok;

        HINSTANCE hInstance = (HINSTANCE)win32::instance();
        WNDCLASSEXW wcex = { sizeof(WNDCLASSEXW) };
        if (!GetClassInfoExW(hInstance, WINDOW_CLASS_NAME, &wcex))
        {
            wcex.cbSize = sizeof(WNDCLASSEXW);
            wcex.style = 0;
            wcex.lpfnWndProc = DefWindowProcW;
            wcex.cbClsExtra = 0;
            wcex.cbWndExtra = 0;
            wcex.hInstance = hInstance;
            wcex.hIcon = NULL;
            wcex.hCursor = /*::LoadCursor(NULL, IDC_ARROW)*/NULL;
            wcex.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);
            wcex.lpszMenuName = NULL;
            wcex.lpszClassName = WINDOW_CLASS_NAME;
            wcex.hIconSm = NULL;

            RegisterClassExW(&wcex);
        }
        else
        {
        }

        Window * pthis = const_cast<Window *>(this);
        auto pos = _form->realPos().to<int32_t>();
        auto size = _form->realSize().convert(ceilf).to<int32_t>();

        pthis->_handle = CreateWindowExW(
            _styleEx, WINDOW_CLASS_NAME, NULL, _style,
            pos.x, pos.y, size.cx, size.cy,
            NULL, NULL, hInstance, NULL);

        pthis->attatch(_handle);
        return core::error_ok;
    }

#define CASE_MSG(M, F) case M: return F(uiMessage, uiParam, iParam)
    intx_t Window::handleMSG(uint32_t uiMessage, uintx_t uiParam, intx_t iParam)
    {
        switch (uiMessage)
        {
            //CASE_MSG(WM_NCHITTEST, OnWmHitTest);
            //CASE_MSG(WM_NCCALCSIZE, OnWmNcCalcSize);
            //CASE_MSG(WM_SHOWWINDOW, OnWmShow);
            //CASE_MSG(WM_ERASEBKGND, OnWmEraseBack);
            //CASE_MSG(WM_PAINT, OnWmPaint);
            //CASE_MSG(WM_NCPAINT, OnWmNcPaint);
            //CASE_MSG(WM_NCACTIVATE, OnWmNcActivate);

            //CASE_MSG(WM_MOUSEMOVE, OnWmMouseMove);
            //CASE_MSG(WM_MOUSELEAVE, OnWmMouseLeave);

            //CASE_MSG(WM_LBUTTONDOWN, OnWmMouseDownL);
            //CASE_MSG(WM_LBUTTONUP, OnWmMouseUpL);

            //CASE_MSG(WM_RBUTTONDOWN, OnWmMouseDownR);
            //CASE_MSG(WM_RBUTTONUP, OnWmMouseUpR);

            //CASE_MSG(WM_MBUTTONDOWN, OnWmMouseDownM);
            //CASE_MSG(WM_MBUTTONUP, OnWmMouseUpM);

            //CASE_MSG(WM_NCLBUTTONDOWN, OnWmNcMouseDownL);

            //CASE_MSG(WM_LBUTTONDBLCLK, OnWmMouseDBClick);
            //CASE_MSG(WM_MOUSEWHEEL, OnWmMouseWheelV);

            //CASE_MSG(WM_MOVE, OnWmMove);
            //CASE_MSG(WM_SIZE, OnWmSize);
            //CASE_MSG(WM_SETTEXT, OnWmSetText);
            //CASE_MSG(WM_ACTIVATE, OnWmActive);
            //CASE_MSG(WM_SETFOCUS, OnWmSetFocus);
            //CASE_MSG(WM_KILLFOCUS, OnWmKillFocus);
            //CASE_MSG(WM_CHAR, OnWmChar);
            //CASE_MSG(WM_UNICHAR, OnWmChar);

            //CASE_MSG(WM_SYSKEYDOWN, OnWmKeyDown);
            //CASE_MSG(WM_KEYDOWN, OnWmKeyDown);
            //CASE_MSG(WM_SYSKEYUP, OnWmKeyUp);
            //CASE_MSG(WM_KEYUP, OnWmKeyUp);

            //CASE_MSG(WM_SETCURSOR, OnWmSetCursor);
            //CASE_MSG(WM_QUERYDRAGICON, OnWmQueryDrag);
            //CASE_MSG(WM_DROPFILES, OnWmDropFiles);
            //CASE_MSG(WM_NOTIFY, OnWmNotify);
            //CASE_MSG(WM_CAPTURECHANGED, OnWmCaptureChanged);
            //CASE_MSG(WM_MOUSEACTIVATE, OnWmMouseActive);
            //CASE_MSG(WM_NCMOUSEMOVE, OnWmNcMouseMove);

            //CASE_MSG(WM_CLOSE, OnWmClose);
            //CASE_MSG(WM_DESTROY, OnWmDestroy);

            //CASE_MSG(WM_GETMINMAXINFO, OnWmGetMinMaxInfo);
            //CASE_MSG(WM_SYSCOMMAND, OnWmSysCommand);

        default:
            return OnDefault(uiMessage, uiParam, iParam);
        }
    }

    intx_t Window::OnDefault(uint32_t uiMessage, uintx_t uiParam, intx_t iParam)
    {
        assert(_handle);
        HWND hwnd = (HWND)_handle;
        WNDPROC pfnOldWndProc = (WNDPROC)GetPropW(hwnd, WINDOW_PROP_OLD_WNDPROC);
        if (!pfnOldWndProc)
            pfnOldWndProc = DefWindowProcW;
        return (intx_t)CallWindowProc(pfnOldWndProc, hwnd, (UINT)uiMessage, (WPARAM)uiParam, (LPARAM)iParam);
    }

    Window * Window::fromHandle(handle_t handle)
    {
        return (Window *)(void *)GetPropW((HWND)handle, WINDOW_PROP_THIS_PTR);
    }


    static LRESULT CALLBACK WindowWndProc(HWND hWnd, UINT uiMessage, WPARAM uiParam, LPARAM iParam)
    {
        Window * pWindow = Window::fromHandle(hWnd);

        switch (uiMessage)
        {
        case WM_MOVE:
        case WM_MOUSEMOVE:
        case WM_NCMOUSEMOVE:
        case WM_SIZE:
        case WM_NCHITTEST:
        case WM_PAINT:
        case WM_SETCURSOR:
        case WM_WINDOWPOSCHANGED:
        case WM_WINDOWPOSCHANGING:
        case WM_GETMINMAXINFO:
        case WM_MOVING:
        case WM_IME_NOTIFY:
        case WM_ERASEBKGND:
        case WM_NCPAINT:
        case WM_NCMOUSELEAVE:
            break;
        default:
            //Printf("消息：[%02d:%02d:%02d:%03d][0x%04X][%s](%u, %d)\n", time.Hour, time.Minute, time.Second, time.MimiSecond, 
            //uiMessage, GetWmMessageText(uiMessage), uiParam, iParam);
            break;
        }

        if (pWindow)
            return pWindow->handleMSG(uiMessage, uiParam, iParam);
        else
        {
            //TimeT time;
            //Printf(_T("未处理的消息：[%02d:%02d:%02d:%03d][0x%04X][%s](%u, %d)\n"), time.Hour, time.Minute, time.Second, time.MimiSecond,
            //	   uiMessage, GetWmMessageText(uiMessage), uiParam, iParam);
            switch (uiMessage)
            {
            case WM_PAINT:
                if (true)
                {
                    PAINTSTRUCT ps;
                    HDC hdc = ::BeginPaint(hWnd, &ps);
                    EndPaint(hWnd, &ps);
                }
                break;
            case WM_DESTROY:
                win32::quit(0);
                break;
            default:
                return DefWindowProcW(hWnd, uiMessage, uiParam, iParam);
            }
            return 0;
        }
    }
}