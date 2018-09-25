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
        uint32_t style = 0;
        uint32_t styleEx = 0;
        auto fs = form->styles();
        if (fs.any(ui::form_style::popup))
        {
            _style = WS_POPUP | WS_BORDER | WS_DLGFRAME | WS_SYSMENU | WS_THICKFRAME;
            _styleEx = 0;
        }
        else /*if (fs.all(ui::form_style::normal))*/
        {
            _style = WS_OVERLAPPED | WS_BORDER | WS_DLGFRAME | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
            _styleEx = 0;
        }

        auto scene = form->scene();
        form->shownChanged += std::weak_bind(&Window::onShownChanged, share_ref<Window>(), std::placeholders::_1);
        form->stylesChanged += std::weak_bind(&Window::onStylesChanged, share_ref<Window>(), std::placeholders::_1);
        scene->rendered += std::weak_bind(&Window::onSceneRendered, share_ref<Window>(), std::placeholders::_1);
        scene->rendered2 += std::weak_bind(&Window::onSceneRendered2, share_ref<Window>(), std::placeholders::_1);
        scene->captured += std::weak_bind(&Window::onSceneCaptured, share_ref<Window>(), std::placeholders::_1);
        if (_ime_context)
            scene->setImeContext(_ime_context);
        if (_cursor_context)
            scene->setCursorContext(_cursor_context);
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

        _ime_context = std::make_shared<win32::ImeContext>(handle);
        _cursor_context = std::make_shared<win32::CursorContext>(handle);
        if (auto f = form())
        {
            if(auto s =  f->scene())
            {
                s->setImeContext(_ime_context);
                s->setCursorContext(_cursor_context);
            }
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
        _ime_context->release();
        _ime_context.reset();
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

    void Window::onStylesChanged(ui::form_styles styles)
    {
        HWND hwnd = (HWND)_handle;
        if (!hwnd)
            return;

        //if (fs.any(ui::form_style::popup))
        //{
        //    _style = WS_POPUP | WS_BORDER | WS_SYSMENU;
        //    _styleEx = 0;
        //}
        //else /*if (fs.all(ui::form_style::normal))*/
        //{
        //    _style = WS_OVERLAPPED | WS_BORDER | WS_DLGFRAME | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
        //    _styleEx = 0;
        //}

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

    void Window::onSceneRendered2(const core::rc32i & rect)
    {
        _render(rect);
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

            CASE_MSG(WM_SYSKEYDOWN, OnWmKeyDown);
            CASE_MSG(WM_KEYDOWN, OnWmKeyDown);
            CASE_MSG(WM_SYSKEYUP, OnWmKeyUp);
            CASE_MSG(WM_KEYUP, OnWmKeyUp);

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
            s->onMouse(_mouse_state, ui::mouse_button::none, ui::mouse_action::enter);
        }

        s->onMouse(_mouse_state, ui::mouse_button::none, ui::mouse_action::move);
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
        s->onMouse(_mouse_state, ui::mouse_button::none, ui::mouse_action::leave);
        if(_cursor_context)
            _cursor_context->reset();
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
        s->onMouse(_mouse_state, ui::mouse_button::left, ui::mouse_action::press);
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
        s->onMouse(_mouse_state, ui::mouse_button::left, ui::mouse_action::release);
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
        s->onMouse(_mouse_state, ui::mouse_button::none,  ui::mouse_action::wheel_v);
        return 0;
    }

    intx_t Window::OnWmChar(uintx_t uiParam, intx_t iParam)
    {
        if (auto s = scene())
            s->onChar(char32_t(uiParam));
        return 0;
    }

    intx_t Window::OnWmKeyDown(uintx_t uiParam, intx_t iParam)
    {
        auto f = form();
        if (!f)
            throw core::exception(core::error_nullptr);
        auto s = f->scene();

        ui::keycode key = virtualkey2keycode(uiParam);
        _mouse_state.setKey(key, true);
        s->onKey(_mouse_state, key, ui::key_action::press);
        return 0;
    }

    intx_t Window::OnWmKeyUp(uintx_t uiParam, intx_t iParam)
    {
        auto f = form();
        if (!f)
            throw core::exception(core::error_nullptr);
        auto s = f->scene();

        ui::keycode key = virtualkey2keycode(uiParam);
        _mouse_state.setKey(key, false);
        s->onKey(_mouse_state, key, ui::key_action::release);
        return 0;
    }

    intx_t Window::OnWmSetFocus(uintx_t uiParam, intx_t iParam)
    {
        return 0;
    }

    intx_t Window::OnWmKillFocus(uintx_t uiParam, intx_t iParam)
    {
        _mouse_state.setAllKeys(false);
        return 0;
    }

    Window * Window::fromHandle(handle_t handle)
    {
        return (Window *)(void *)GetPropW((HWND)handle, WINDOW_PROP_THIS_PTR);
    }


    static LRESULT CALLBACK WindowWndProc(HWND hWnd, UINT uiMessage, WPARAM uiParam, LPARAM iParam)
    {
        Window * pWindow = Window::fromHandle(hWnd);
        LRESULT res = 0;
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
        {
            res = pWindow->handleMSG(uiMessage, uiParam, iParam);
        }
        else
        {
            //TimeT time;
            //Printf(_T("未处理的消息：[%02d:%02d:%02d:%03d][0x%04X][%s](%u, %d)\n"), time.Hour, time.Minute, time.Second, time.MimiSecond,
            //	   uiMessage, GetWmMessageText(uiMessage), uiParam, iParam);
            switch (uiMessage)
            {
            case WM_PAINT:
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
                res = DefWindowProcW(hWnd, uiMessage, uiParam, iParam);
                break;
            }
        }
        core::object::get_invoke_helper().trigger();
        return res;
    }

    ui::keycode virtualkey2keycode(uint32_t vkey)
    {
        if (vkey >= 0xff)
            return ui::keycode::none;

        const ui::keycode notmatch = ui::keycode::none;
        const ui::keycode reserved = ui::keycode::none;
        const ui::keycode unassigned = ui::keycode::none;
        const ui::keycode none = ui::keycode::none;

        static const ui::keycode keys[0xff] =
        {
            //! 不可用的按键码。
            /*0x00*/none,
            /*0x01*/none, // vk_lbutton
            /*0x02*/none, // vk_rbutton
            /*0x03*/none, // vk_cancel
            /*0x04*/none, // vm_mbutton
            /*0x05*/none, // vk_xbutton1
            /*0x06*/none, // vk_xbutton2
            /*0x07*/none,
            /*0x08*/ui::keycode::backspace, // vk_back
            /*0x09*/ui::keycode::tab, // vk_tab
            /*0x0a*/none,
            /*0x0b*/none,
            /*0x0c*/ui::keycode::clear,
            /*0x0d*/ui::keycode::enter,
            /*0x0e*/none,
            /*0x0f*/none,

            /*0x10*/ui::keycode::shift,
            /*0x11*/ui::keycode::control,
            /*0x12*/ui::keycode::alt,
            /*0x13*/ui::keycode::pausebreak,
            /*0x14*/ui::keycode::caps,
            /*0x15*/ui::keycode::ime_kana,
            /*0x16*/ui::keycode::ime_hangul,
            /*0x17*/ui::keycode::ime_junja,
            /*0x18*/ui::keycode::ime_final,
            /*0x19*/ui::keycode::ime_kanji,
            /*0x1a*/none,
            /*0x1b*/ui::keycode::escape,
            /*0x1c*/ui::keycode::ime_convert,
            /*0x1d*/ui::keycode::ime_nonconvert,
            /*0x1e*/ui::keycode::ime_accept,
            /*0x1f*/ui::keycode::ime_modechange,

            /*0x20*/ui::keycode::space,
            /*0x21*/ui::keycode::pageup,
            /*0x22*/ui::keycode::pagedown,
            /*0x23*/ui::keycode::end,
            /*0x24*/ui::keycode::home,
            /*0x25*/ui::keycode::left,
            /*0x26*/ui::keycode::up,
            /*0x27*/ui::keycode::right,
            /*0x28*/ui::keycode::down,
            /*0x29*/ui::keycode::select,
            /*0x2a*/ui::keycode::print,
            /*0x2b*/ui::keycode::excute,
            /*0x2c*/ui::keycode::printscreen,
            /*0x2d*/ui::keycode::insert,
            /*0x2e*/ui::keycode::del,
            /*0x2f*/ui::keycode::help,

            /*0x30*/ui::keycode::num0,
            /*0x31*/ui::keycode::num1,
            /*0x32*/ui::keycode::num2,
            /*0x33*/ui::keycode::num3,
            /*0x34*/ui::keycode::num4,
            /*0x35*/ui::keycode::num5,
            /*0x36*/ui::keycode::num6,
            /*0x37*/ui::keycode::num7,
            /*0x38*/ui::keycode::num8,
            /*0x39*/ui::keycode::num9,
            /*0x3a*/none,
            /*0x3b*/none,
            /*0x3c*/none,
            /*0x3d*/none,
            /*0x3e*/none,
            /*0x3f*/none,

            /*0x40*/none,
            /*0x41*/ui::keycode::a,
            /*0x42*/ui::keycode::b,
            /*0x43*/ui::keycode::c,
            /*0x44*/ui::keycode::d,
            /*0x45*/ui::keycode::e,
            /*0x46*/ui::keycode::f,
            /*0x47*/ui::keycode::g,
            /*0x48*/ui::keycode::h,
            /*0x49*/ui::keycode::i,
            /*0x4a*/ui::keycode::j,
            /*0x4b*/ui::keycode::k,
            /*0x4c*/ui::keycode::l,
            /*0x4d*/ui::keycode::m,
            /*0x4e*/ui::keycode::n,
            /*0x4f*/ui::keycode::o,
            /*0x50*/ui::keycode::p,
            /*0x51*/ui::keycode::q,
            /*0x52*/ui::keycode::r,
            /*0x53*/ui::keycode::s,
            /*0x54*/ui::keycode::t,
            /*0x55*/ui::keycode::u,
            /*0x56*/ui::keycode::v,
            /*0x57*/ui::keycode::w,
            /*0x58*/ui::keycode::x,
            /*0x59*/ui::keycode::y,
            /*0x5a*/ui::keycode::z,
            /*0x5b*/ui::keycode::winL,
            /*0x5c*/ui::keycode::winR,
            /*0x5d*/ui::keycode::apps,
            /*0x5e*/none,
            /*0x5f*/ui::keycode::sleep,

            /*0x60*/ui::keycode::numpad_0,
            /*0x61*/ui::keycode::numpad_1,
            /*0x62*/ui::keycode::numpad_2,
            /*0x63*/ui::keycode::numpad_3,
            /*0x64*/ui::keycode::numpad_4,
            /*0x65*/ui::keycode::numpad_5,
            /*0x66*/ui::keycode::numpad_6,
            /*0x67*/ui::keycode::numpad_7,
            /*0x68*/ui::keycode::numpad_8,
            /*0x69*/ui::keycode::numpad_9,
            /*0x6a*/ui::keycode::numpad_mul,
            /*0x6b*/ui::keycode::numpad_add,
            /*0x6c*/ui::keycode::numpad_enter,
            /*0x6d*/ui::keycode::numpad_sub,
            /*0x6e*/ui::keycode::numpad_decimal,
            /*0x6f*/ui::keycode::numpad_div,

            /*0x70*/ui::keycode::f1,
            /*0x71*/ui::keycode::f2,
            /*0x72*/ui::keycode::f3,
            /*0x73*/ui::keycode::f4,
            /*0x74*/ui::keycode::f5,
            /*0x75*/ui::keycode::f6,
            /*0x76*/ui::keycode::f7,
            /*0x77*/ui::keycode::f8,
            /*0x78*/ui::keycode::f9,
            /*0x79*/ui::keycode::f10,
            /*0x7a*/ui::keycode::f11,
            /*0x7b*/ui::keycode::f12,
            /*0x7c*/ui::keycode::f13,
            /*0x7d*/ui::keycode::f14,
            /*0x7e*/ui::keycode::f15,
            /*0x7f*/ui::keycode::f16,
            /*0x80*/ui::keycode::f17,
            /*0x81*/ui::keycode::f18,
            /*0x82*/ui::keycode::f19,
            /*0x83*/ui::keycode::f20,
            /*0x84*/ui::keycode::f21,
            /*0x85*/ui::keycode::f22,
            /*0x86*/ui::keycode::f23,
            /*0x87*/ui::keycode::f24,
            /*0x88*/none,
            /*0x89*/none,
            /*0x8a*/none,
            /*0x8b*/none,
            /*0x8c*/none,
            /*0x8d*/none,
            /*0x8e*/none,
            /*0x8f*/none,

            /*0x90*/ui::keycode::numpad_lock,
            /*0x91*/ui::keycode::scrolllock,
            /*0x92*/ui::keycode::numpad_equal,
            /*0x93*/none,
            /*0x94*/none,
            /*0x95*/none,
            /*0x96*/none,
            /*0x97*/none,
            /*0x98*/none,
            /*0x99*/none,
            /*0x9a*/none,
            /*0x9b*/none,
            /*0x9c*/none,
            /*0x9d*/none,
            /*0x9e*/none,
            /*0x9f*/none,

            /*0xa0*/ui::keycode::shiftL,
            /*0xa1*/ui::keycode::shiftR,
            /*0xa2*/ui::keycode::controlL,
            /*0xa3*/ui::keycode::controlR,
            /*0xa4*/ui::keycode::altL,
            /*0xa5*/ui::keycode::altR,
            /*0xa6*/ui::keycode::browser_back,
            /*0xa7*/ui::keycode::browser_forward,
            /*0xa8*/ui::keycode::browser_forward,
            /*0xa9*/ui::keycode::browser_stop,
            /*0xaa*/ui::keycode::browser_search,
            /*0xab*/ui::keycode::browser_favorites,
            /*0xac*/ui::keycode::browser_home,
            /*0xad*/ui::keycode::volume_mute,
            /*0xae*/ui::keycode::volume_down,
            /*0xaf*/ui::keycode::volume_up,

            /*0xb0*/ui::keycode::media_next,
            /*0xb1*/ui::keycode::media_prev,
            /*0xb2*/ui::keycode::media_stop,
            /*0xb3*/ui::keycode::media_playpause,
            /*0xb4*/ui::keycode::launch_mail,
            /*0xb5*/ui::keycode::launch_mediaselect,
            /*0xb6*/ui::keycode::launch_app1,
            /*0xb7*/ui::keycode::launch_app2,
            /*0xb8*/none,
            /*0xb9*/none,
            /*0xba*/ui::keycode::semicolon,
            /*0xbb*/ui::keycode::equal,
            /*0xbc*/ui::keycode::comma,
            /*0xbd*/ui::keycode::sub,
            /*0xbe*/ui::keycode::period,
            /*0xbf*/ui::keycode::slash,

            /*0xc0*/ui::keycode::grave,
            /*0xc1*/reserved,
            /*0xc2*/reserved,
            /*0xc3*/reserved,
            /*0xc4*/reserved,
            /*0xc5*/reserved,
            /*0xc6*/reserved,
            /*0xc7*/reserved,
            /*0xc8*/reserved,
            /*0xc9*/reserved,
            /*0xca*/reserved,
            /*0xcb*/reserved,
            /*0xcc*/reserved,
            /*0xcd*/reserved,
            /*0xce*/reserved,
            /*0xcf*/reserved,

            /*0xd0*/reserved,
            /*0xd1*/reserved,
            /*0xd2*/reserved,
            /*0xd3*/reserved,
            /*0xd4*/reserved,
            /*0xd5*/reserved,
            /*0xd6*/reserved,
            /*0xd7*/reserved,
            /*0xd8*/unassigned,
            /*0xd9*/unassigned,
            /*0xda*/unassigned,
            /*0xdb*/ui::keycode::bracketL,
            /*0xdc*/ui::keycode::backslash,
            /*0xdd*/ui::keycode::bracketR,
            /*0xde*/ui::keycode::apostrophe,
            /*0xdf*/none,

            /*0xe0*/reserved,
            /*0xe1*/notmatch,
            /*0xe2*/ui::keycode::comma,
            /*0xe3*/notmatch,
            /*0xe4*/notmatch,
            /*0xe5*/notmatch,
            /*0xe6*/notmatch,
            /*0xe7*/notmatch,
            /*0xe8*/notmatch,
            /*0xe9*/notmatch,
            /*0xea*/notmatch,
            /*0xeb*/notmatch,
            /*0xec*/notmatch,
            /*0xed*/notmatch,
            /*0xee*/notmatch,
            /*0xef*/notmatch,

            /*0xf0*/notmatch,
            /*0xf1*/notmatch,
            /*0xf2*/notmatch,
            /*0xf3*/notmatch,
            /*0xf4*/notmatch,
            /*0xf5*/notmatch,
            /*0xf6*/notmatch,
            /*0xf7*/notmatch,
            /*0xf8*/notmatch,
            /*0xf9*/notmatch,
            /*0xfa*/notmatch,
            /*0xfb*/notmatch,
            /*0xfc*/notmatch,
            /*0xfd*/notmatch,
            /*0xfe*/notmatch,
            /*0xff*/
        };

        ui::keycode key = keys[vkey];
        return key;
    }
    uint32_t keycode2virtualkey(ui::keycode code)
    {
        const uint8_t NONE = 0;

        if ((uint32_t)code >= 0xFF)
            return NONE;

        static const uint32_t KEYS[0xFF] =
        {
            NONE,
            '0',
            '1',
            '2',
            '3',
            '4',
            '5',
            '6',
            '7',
            '8',
            '9',

            VK_CAPITAL,
            'A',
            'B',
            'C',
            'D',
            'E',
            'F',
            'G',
            'H',
            'I',
            'J',
            'K',
            'L',
            'M',
            'N',
            'O',
            'P',
            'Q',
            'R',
            'S',
            'T',
            'U',
            'V',
            'W',
            'X',
            'Y',
            'Z',

            VK_BACK,
            VK_TAB,
            VK_RETURN,

            VK_ESCAPE,
            VK_SPACE,

            VK_NUMLOCK,
            VK_NUMPAD0,
            VK_NUMPAD1,
            VK_NUMPAD2,
            VK_NUMPAD3,
            VK_NUMPAD4,
            VK_NUMPAD5,
            VK_NUMPAD6,
            VK_NUMPAD7,
            VK_NUMPAD8,
            VK_NUMPAD9,

            VK_ADD,
            VK_SUBTRACT,
            VK_MULTIPLY,
            VK_DIVIDE,
            VK_DECIMAL,
            NONE,
            NONE,

            VK_F1,
            VK_F2,
            VK_F3,
            VK_F4,
            VK_F5,
            VK_F6,
            VK_F7,
            VK_F8,
            VK_F9,
            VK_F10,
            VK_F11,
            VK_F12,
            VK_F13,
            VK_F14,
            VK_F15,
            VK_F16,
            VK_F17,
            VK_F18,
            VK_F19,
            VK_F20,
            VK_F21,
            VK_F22,
            VK_F23,
            VK_F24,

            NONE,
            NONE,
            NONE,
            NONE,
            NONE,
            NONE,
            NONE,
            NONE,
            NONE,
            NONE,
            NONE,
            NONE,

            VK_INSERT,
            VK_DELETE,
            VK_HOME,
            VK_END,
            VK_PRIOR,
            VK_NEXT,

            VK_LEFT,
            VK_UP,
            VK_RIGHT,
            VK_DOWN,

            VK_PRINT,
            VK_SCROLL,
            VK_PAUSE,

            VK_LWIN,
            VK_RWIN,

            VK_CONTROL,
            VK_LCONTROL,
            VK_RCONTROL,

            VK_SHIFT,
            VK_LSHIFT,
            VK_RSHIFT,

            VK_MENU,
            VK_LMENU,
            VK_RMENU,

            VK_APPS,

            VK_SLEEP,
        };
        return KEYS[(uint32_t)code];
    }
}