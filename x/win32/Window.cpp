#include "stdafx.h"
#include "Window.h"
#include "win32/windows.h"

namespace win32
{
    const uint32_t WM_REFRESH = WM_USER + 1;

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
        if (_handle)
            detach();
    }


    core::error Window::attatch(std::shared_ptr<ui::Form> form)
    {
        _form = form;
        auto scene = form->scene();
        form->shownChanged += std::weak_bind(&Window::onShownChanged, share_ref<Window>(), std::placeholders::_1);
        scene->rendered += std::weak_bind(&Window::onSceneRendered, share_ref<Window>(), std::placeholders::_1);
        scene->captured += std::weak_bind(&Window::onSceneCaptured, share_ref<Window>(), std::placeholders::_1);
        if(_imecontext)
            scene->setImeContext(_imecontext);
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
        _handle = handle;

        _imecontext = std::make_shared<win32::ImeContext>(handle);
        if (auto f = form())
        {
            if(auto s =  f->scene())
                s->setImeContext(_imecontext);
        }

        return core::error_ok;
    }

    void Window::detach()
    {
        HWND hwnd = (HWND)_handle;
        if (!hwnd)
            return;

        WNDPROC pfnWndProcOld = (WNDPROC)GetPropW(hwnd, WINDOW_PROP_OLD_WNDPROC);
        if (!pfnWndProcOld)
            return;

        SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)pfnWndProcOld);

        RemovePropW(hwnd, WINDOW_PROP_THIS_PTR);
        RemovePropW(hwnd, WINDOW_PROP_DLG_RESULT);
        RemovePropW(hwnd, WINDOW_PROP_OLD_WNDPROC);
        _imecontext->release();
        _imecontext.reset();
    }

    void Window::move(const core::pt32f & pos)
    {
        //if (!can_safe_invoke())
        //{
        //    invoke([this, pos]() { move(pos); });
        //    return;
        //}

        HWND hwnd = (HWND)_handle;
        if (!hwnd)
            return;

        auto p = pos.to<int32_t>();
        if (p != _pos())
            _adjustWindow(p, _size());
    }

    void Window::resize(const core::si32f & size)
    {
        HWND hwnd = (HWND)_handle;
        if (!hwnd)
            return;

        auto s = size.to<int32_t>();
        if (s != _size())
            _adjustWindow(_pos(), s);
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
            ShowWindow(hwnd, SW_SHOW);
        }
        else
        {
            HWND hwnd = (HWND)_handle;
            if (hwnd)
                ::ShowWindow(hwnd, SW_HIDE);
        }
    }

    void Window::onPosChanged(const core::pt32f & from, const core::pt32f & to)
    {
        HWND hwnd = (HWND)_handle;
        if (!hwnd)
            return;

        auto pos = to.to<int32_t>();
        if (pos != _pos())
            _adjustWindow(pos, _size());
    }

    void Window::onSizeChanged(const core::si32f & from, const core::si32f & to)
    {
        //HWND hwnd = (HWND)_handle;
        //if (!hwnd)
        //    return;

        //auto size = to.to<int32_t>();
        //if (size != _size())
        //    _adjustWindow(_pos(), size);
    }

    void Window::onSceneInvalidated(const core::rc32i & rect)
    {
        HWND hwnd = (HWND)_handle;
        if (!hwnd)
            return;

        ::PostMessageW(hwnd, WM_REFRESH, core::uxfromih(rect.x, rect.y), core::ixfromih(rect.cx, rect.cy));
    }

    void Window::onSceneRendered(const drawing::Region & region)
    {
        _render(region);
    }

    void Window::onSceneCaptured(bool capture)
    {
        if (!can_safe_invoke())
        {
            invoke([this, capture]() { onSceneCaptured(capture); });
            return;
        }

        HWND hwnd = (HWND)_handle;
        if (!hwnd)
            return;

        if (capture)
        {
            std::cout << " captured window" << std::endl;
            ::SetCapture(hwnd);
        }
        else if(hwnd == GetCapture())
        {
            ::ReleaseCapture();
            std::cout << " release captured window" << std::endl;
        }
        
    }

    core::error Window::_createWindow()
    {
        auto f = form();
        if (!f)
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
            wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
            wcex.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);
            wcex.lpszMenuName = NULL;
            wcex.lpszClassName = WINDOW_CLASS_NAME;
            wcex.hIconSm = NULL;

            RegisterClassExW(&wcex);
        }
        else
        {
        }

        auto pos = f->windowPos().to<int32_t>();
        auto size = f->realSize().convert(ceilf).to<int32_t>();

        RECT rect = { pos.x, pos.y, pos.x + size.cx, pos.y + size.cy };
        ::AdjustWindowRect(&rect, _style, FALSE);

        HWND hwnd = CreateWindowExW(
            _styleEx, WINDOW_CLASS_NAME, NULL, _style,
            rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
            NULL, NULL, hInstance, NULL);

        attatch(hwnd);
        return core::error_ok;
    }

    core::error Window::_adjustWindow(const core::pt32i & pos, const core::si32i & size)
    {
        if (!_handle)
            return core::error_nullptr;

        HWND hwnd = (HWND)_handle;
        RECT rect = { pos.x, pos.y, pos.x + size.cx, pos.y + size.cy };
        ::AdjustWindowRect(&rect, _style, FALSE);
        ::MoveWindow(hwnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, FALSE);
        return core::error_ok;
    }

    core::pt32i Window::_pos() const
    {
        if (!_handle)
            return {};

        HWND hwnd = (HWND)_handle;
        WINDOWINFO winfo;
        ::GetWindowInfo(hwnd, &winfo);
        return core::vec2i(winfo.rcClient.left, winfo.rcClient.top);
    }

    core::si32i Window::_size() const
    {
        if (!_handle)
            return {};

        HWND hwnd = (HWND)_handle;
        WINDOWINFO winfo;
        ::GetWindowInfo(hwnd, &winfo);
        return core::vec2i(winfo.rcClient.right - winfo.rcClient.left, winfo.rcClient.bottom - winfo.rcClient.top);
    }

    core::vec4i Window::_border() const
    {
        if (!_handle)
            return {};

        HWND hwnd = (HWND)_handle;
        WINDOWINFO winfo;
        ::GetWindowInfo(hwnd, &winfo);

        return core::vec4i(winfo.rcClient.left - winfo.rcWindow.left,
            winfo.rcClient.top - winfo.rcWindow.top,
            winfo.rcWindow.right - winfo.rcClient.right,
            winfo.rcWindow.bottom - winfo.rcClient.bottom);
        
    }

    core::vec4i Window::_padding() const
    {
        if (!_handle)
            return {};

        HWND hwnd = (HWND)_handle;
        WINDOWINFO winfo;
        ::GetWindowInfo(hwnd, &winfo);

        return core::vec4i(winfo.rcClient.left - winfo.rcWindow.left,
            winfo.rcClient.top - winfo.rcWindow.top,
            winfo.rcWindow.right - winfo.rcClient.right,
            winfo.rcWindow.bottom - winfo.rcClient.bottom);
    }

    void Window::_render(const core::rc32i & rect)
    {
        auto f = form();
        if (!f)
            return;

        auto rc = rect.intersected(core::rc32i(core::pt32i(), _size()));

        auto scene = f->scene();
        std::shared_ptr<drawing::Bitmap> bitmap = scene->bitmap();

        HWND hwnd = (HWND)_handle;
        if (!hwnd)
            return;

        drawing::bitmap_buffer buffer = bitmap->buffer();
        BITMAPINFO bmi;
        memset(&bmi, 0, sizeof(bmi));
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = buffer.size.cx;
        bmi.bmiHeader.biHeight = -buffer.size.cy;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        bmi.bmiHeader.biSizeImage = 0;

        HDC hdc = GetDC(hwnd);
        SetDIBitsToDevice(hdc, 
            rc.x, rc.y, rc.cx, rc.cy, 
            rc.x, buffer.size.cy - rc.y - rc.cy, 0, buffer.size.cy, buffer.data, &bmi, DIB_RGB_COLORS);
        ReleaseDC(hwnd, hdc);
    }

    void Window::_render(const drawing::Region & region)
    {
        auto f = form();
        if (!f)
            return;


        auto scene = f->scene();
        std::shared_ptr<drawing::Bitmap> bitmap = scene->bitmap();

        HWND hwnd = (HWND)_handle;
        if (!hwnd)
            return;

        drawing::bitmap_buffer buffer = bitmap->buffer();
        BITMAPINFO bmi;
        memset(&bmi, 0, sizeof(bmi));
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = buffer.size.cx;
        bmi.bmiHeader.biHeight = -buffer.size.cy;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        bmi.bmiHeader.biSizeImage = 0;

        HDC hdc = GetDC(hwnd);
        drawing::RegionIterator ri(region);
        while (!ri.done())
        {
            auto rect = ri.rect();
            ri.next();

            auto rc = rect.intersected(core::rc32i(core::pt32i(), _size()));
            SetDIBitsToDevice(hdc,
                rc.x, rc.y, rc.cx, rc.cy,
                rc.x, buffer.size.cy - rc.y - rc.cy, 0, buffer.size.cy, buffer.data, &bmi, DIB_RGB_COLORS);
        }
        ReleaseDC(hwnd, hdc);
    }

#define CASE_MSG(M, F) case M: return F(uiParam, iParam)
    intx_t Window::handleMSG(uint32_t uiMessage, uintx_t uiParam, intx_t iParam)
    {
        auto f = form();
        if (!f)
            return OnDefault(uiMessage, uiParam, iParam);

        switch (uiMessage)
        {
            //CASE_MSG(WM_NCHITTEST, OnWmHitTest);
            //CASE_MSG(WM_NCCALCSIZE, OnWmNcCalcSize);
            //CASE_MSG(WM_SHOWWINDOW, OnWmShow);
            CASE_MSG(WM_ERASEBKGND, OnWmEraseBack);
            //CASE_MSG(WM_PAINT, OnWmPaint);
            //CASE_MSG(WM_NCPAINT, OnWmNcPaint);
            //CASE_MSG(WM_NCACTIVATE, OnWmNcActivate);

            CASE_MSG(WM_MOUSEMOVE, OnWmMouseMove);
            CASE_MSG(WM_MOUSELEAVE, OnWmMouseLeave);

            CASE_MSG(WM_LBUTTONDOWN, OnWmMouseDownL);
            CASE_MSG(WM_LBUTTONUP, OnWmMouseUpL);

            //CASE_MSG(WM_RBUTTONDOWN, OnWmMouseDownR);
            //CASE_MSG(WM_RBUTTONUP, OnWmMouseUpR);

            //CASE_MSG(WM_MBUTTONDOWN, OnWmMouseDownM);
            //CASE_MSG(WM_MBUTTONUP, OnWmMouseUpM);

            //CASE_MSG(WM_NCLBUTTONDOWN, OnWmNcMouseDownL);

            //CASE_MSG(WM_LBUTTONDBLCLK, OnWmMouseDBClick);
            CASE_MSG(WM_MOUSEWHEEL, OnWmMouseWheelV);

            CASE_MSG(WM_MOVE, OnWmMove);
            CASE_MSG(WM_SIZE, OnWmSize);
            //CASE_MSG(WM_SETTEXT, OnWmSetText);
            //CASE_MSG(WM_ACTIVATE, OnWmActive);
            //CASE_MSG(WM_SETFOCUS, OnWmSetFocus);
            //CASE_MSG(WM_KILLFOCUS, OnWmKillFocus);
            CASE_MSG(WM_CHAR, OnWmChar);
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

            CASE_MSG(WM_REFRESH, OnWmRefresh);

        case WM_CLOSE:
            f->onClose();
            return 0;
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
        return (intx_t)CallWindowProcW(pfnOldWndProc, hwnd, (UINT)uiMessage, (WPARAM)uiParam, (LPARAM)iParam);
    }

    intx_t Window::OnWmMove(uintx_t uiParam, intx_t iParam)
    {
        auto f = form();
        if (!f)
            throw core::exception(core::error_nullptr);

        f->setWindowPos(_pos().to<float32_t>());
        return 0;
    }

    intx_t Window::OnWmSize(uintx_t uiParam, intx_t iParam)
    {
        auto f = form();
        if (!f)
            throw core::exception(core::error_nullptr);

        f->setWindowSize(_size().to<float32_t>());
        return 0;
    }

     intx_t Window::OnWmRefresh(uintx_t uiParam, intx_t iParam)
    {
        return 0;
    }

    intx_t Window::OnWmMouseMove(uintx_t uiParam, intx_t iParam)
    {
        auto f = form();
        if (!f)
            throw core::exception(core::error_nullptr);
        auto s = f->scene();

        _mouse_state.setWheelLines(0);
        _mouse_state.setPos(core::pt32i(core::i32li16((int32_t)iParam), core::i32hi16((int32_t)iParam)).to<float32_t>());
        if (!_trackingMouse)
        {
            //OnWmMouseEnter(uiMessage, uiParam, iParam);
            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_LEAVE;
            tme.dwHoverTime = 1;
            tme.hwndTrack = (HWND)_handle;
            TrackMouseEvent(&tme);
            _trackingMouse = true;
            s->onMouseState(_mouse_state, ui::mouse_action::enter);
        }

        s->onMouseState(_mouse_state, ui::mouse_action::move);
        return 0;
    }

    intx_t Window::OnWmMouseLeave(uintx_t uiParam, intx_t iParam)
    {
        if (!_trackingMouse)
            return 0;
        _trackingMouse = false;

        auto f = form();
        if (!f)
            throw core::exception(core::error_nullptr);
        auto s = f->scene();

        _mouse_state.setWheelLines(0);
        _mouse_state.setButton(ui::mouse_button::mask, false);
        _mouse_state.setPos(core::pt32i(core::i32li16((int32_t)iParam), core::i32hi16((int32_t)iParam)).to<float32_t>());
        s->onMouseState(_mouse_state, ui::mouse_action::leave);
        return 0;
    }

    intx_t Window::OnWmMouseDownL(uintx_t uiParam, intx_t iParam)
    {
        auto f = form();
        if (!f)
            throw core::exception(core::error_nullptr);
        auto s = f->scene();

        _mouse_state.setWheelLines(0);
        _mouse_state.setButton(ui::mouse_button::left, true);
        _mouse_state.setPos(core::pt32i(core::i32li16((int32_t)iParam), core::i32hi16((int32_t)iParam)).to<float32_t>());
        s->onMouseState(_mouse_state, ui::mouse_action::press);
        return 0;
    }

    intx_t Window::OnWmMouseUpL(uintx_t uiParam, intx_t iParam)
    {
        auto f = form();
        if (!f)
            throw core::exception(core::error_nullptr);
        auto s = f->scene();

        _mouse_state.setWheelLines(0);
        _mouse_state.setButton(ui::mouse_button::left, false);
        _mouse_state.setPos(core::pt32i(core::i32li16((int32_t)iParam), core::i32hi16((int32_t)iParam)).to<float32_t>());
        s->onMouseState(_mouse_state, ui::mouse_action::releas);
        return 0;
    }

    intx_t Window::OnWmMouseWheelV(uintx_t uiParam, intx_t iParam)
    {
        HWND hwnd = (HWND)_handle;
        if (!hwnd)
            return 0;

        auto f = form();
        if (!f)
            throw core::exception(core::error_nullptr);
        auto s = f->scene();

        POINT point = { core::i32li16((int32_t)iParam), core::i32hi16((int32_t)iParam) };
        ::ScreenToClient(hwnd, &point);
        //core::pt32i point = core::pt32i(core::i32li16(iParam), core::i32hi16(iParam));
        //core::pt32i wheel = core::pt32i(core::u32li16(uiParam), core::u32hi16(uiParam));
        _mouse_state.setWheelLines(core::u32hi16((uint32_t)uiParam) / WHEEL_DELTA);
        _mouse_state.setPos(core::pt32i(point.x, point.y).to<float32_t>());
        s->onMouseState(_mouse_state, ui::mouse_action::wheel_v);
        return 0;
    }

    intx_t Window::OnWmChar(uintx_t uiParam, intx_t iParam)
    {
        if (auto s = scene())
            s->onChar(char32_t(uiParam));
        return 0;
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
                win32::endLoop(0);
                break;
            default:
                return DefWindowProcW(hWnd, uiMessage, uiParam, iParam);
            }
            return 0;
        }
    }
}