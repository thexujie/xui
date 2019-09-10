#include "stdafx.h"
#include "Window.h"
#include "x/platform/win32/win32.h"

namespace win32
{
    const uint32_t WM_REFRESH = WM_USER + 1;

    static std::tuple<uint32_t, uint32_t> formStylesToWinStyles(ui::form_styles styles)
    {
		uint32_t style = 0;
		uint32_t styleEx = 0;
        if (styles.any(ui::form_style::frameless))
            style = WS_OVERLAPPED;
        else
            style = WS_OVERLAPPED | WS_BORDER | WS_DLGFRAME;
        if (!styles.any(ui::form_style::nomin))
            style |= WS_MINIMIZEBOX;
        if (!styles.any(ui::form_style::nomax))
            style |= WS_MAXIMIZEBOX;
		if(!styles.any(ui::form_style::noresizable))
			style |= WS_THICKFRAME;

		style |= WS_SYSMENU;
		style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

		if(styles.all(ui::form_style::layered))
			styleEx |= WS_EX_LAYERED;

		if(styles.any(ui::form_style::notaskbutton))
			styleEx |= WS_EX_TOOLWINDOW;

		if(styles.any(ui::form_style::topmost))
			styleEx |= WS_EX_TOPMOST;

		//styleEx |= WS_EX_OVERLAPPEDWINDOW;
		//styleEx |= WS_EX_PALETTEWINDOW;
		return { style, styleEx };
    }

    static thread_local Window * __currentWindow = nullptr;
	LRESULT CALLBACK DefaultWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
        if(__currentWindow)
        {
            auto currentWindow = __currentWindow;
            __currentWindow = nullptr;
            currentWindow->attatch(hWnd);
        }

		//if(message == WM_CREATE)
		//{
		//	CREATESTRUCT * cs = reinterpret_cast<CREATESTRUCT *>(lParam);
		//	Window * window = static_cast<Window *>(cs->lpCreateParams);
		//	if(window)
		//		window->attatch(hWnd);
		//}

		return DefWindowProc(hWnd, message, wParam, lParam);
	}

    static LRESULT CALLBACK WindowWndProc(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);

    Window::Window()
    {
    }

    Window::~Window()
    {
        if (_handle)
            detach();
    }


    core::error Window::attatch(std::shared_ptr<ui::Form> form)
    {
        _form = form;
		_form_styles = form ? form->styles() : nullptr;

        form->stylesChanged += std::bind(&Window::onStylesChanged, this, std::placeholders::_1, std::placeholders::_2);
        form->rendered += std::bind(&Window::onSceneRendered2, this, std::placeholders::_1);
        form->captured += std::bind(&Window::onSceneCaptured, this, std::placeholders::_1);
        form->evented += std::bind(&Window::onSceneEvented, this, std::placeholders::_1);

        if (!_handle)
            _createWindow();
        return core::ok;
    }

    core::error Window::attatch(pointer_t handle)
    {
        if (!handle)
            return core::e_args;

        HWND hwnd = (HWND)handle;

        if (GetPropW(hwnd, win32::WINDOW_PROP_THIS_PTR))
            return core::e_state;

        WNDPROC pfnWndProcOld = (WNDPROC)GetWindowLongPtrW(hwnd, GWLP_WNDPROC);
        if (pfnWndProcOld == WindowWndProc)
            return core::e_state;

        SetPropW(hwnd, win32::WINDOW_PROP_THIS_PTR, (HANDLE)(void *)this);
        SetPropW(hwnd, win32::WINDOW_PROP_DLG_RESULT, (HANDLE)0);
        SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)WindowWndProc);
        SetPropW(hwnd, win32::WINDOW_PROP_OLD_WNDPROC, (HANDLE)pfnWndProcOld);
        _handle = handle;

        _ime_context = std::make_shared<win32::ImeContext>(handle);
        _cursor_context = std::make_shared<win32::CursorContext>(handle);

        return core::ok;
    }

    void Window::detach()
    {
        HWND hwnd = (HWND)_handle;
        if (!hwnd)
            return;

        WNDPROC pfnWndProcOld = (WNDPROC)GetPropW(hwnd, win32::WINDOW_PROP_OLD_WNDPROC);
        if (!pfnWndProcOld)
            return;

        SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)pfnWndProcOld);

        RemovePropW(hwnd, win32::WINDOW_PROP_THIS_PTR);
        RemovePropW(hwnd, win32::WINDOW_PROP_DLG_RESULT);
        RemovePropW(hwnd, win32::WINDOW_PROP_OLD_WNDPROC);
        _ime_context->release();
        _ime_context.reset();
		_handle = nullptr;
    }

    void Window::move(const core::pointf & pos)
    {
        HWND hwnd = (HWND)_handle;
        if (!hwnd)
            return;

        auto p = pos.to<int32_t>();
        if (p != _pos())
            _adjustWindowPos(p);
    }

    void Window::resize(const core::sizef & size)
    {
        HWND hwnd = (HWND)_handle;
        if (!hwnd)
            return;

        auto s = size.to<int32_t>();
        if (s != _size())
            _adjustWindow(_pos(), s);
    }

    void Window::show(ui::form_state state)
    {
        if (!_handle)
            return;

        switch (state)
        {
        case ui::form_state::hide:
        {
            ::ShowWindow((HWND)_handle, SW_HIDE);
            break;
        }
        case ui::form_state::noactive:
        {
            ::ShowWindow((HWND)_handle, SW_SHOWNOACTIVATE);
            break;
        }
        case ui::form_state::normalize:
        {
            ::ShowWindow((HWND)_handle, SW_SHOWNORMAL);
            break;
        }
        case ui::form_state::minimize:
        {
            ::ShowWindow((HWND)_handle, SW_SHOWMINIMIZED);
            break;
        }
        case ui::form_state::maximize:
        {
            ::ShowWindow((HWND)_handle, SW_SHOWMAXIMIZED);
            break;
        }
        default:
            break;
        }
    }

    void Window::onStylesChanged(ui::form_styles styels_old, ui::form_styles styles)
    {
		auto f = form();
		if(!f)
			return;

		//if(styels_old.any(ui::form_style::tool) != styles.any(ui::form_style::tool))
		//{
		//	HWND hwnd = (HWND)_handle;
		//	if(hwnd)
		//	{
		//		detach();
		//		::DestroyWindow(hwnd);
		//	}
		//	std::tie(_style, _styleEx) = formStylesToWinStyles(styles);
		//	_createWindow();
		//	if(f->shown())
		//		onStateChanged(ui::form_state::hide, f->formState());
		//	return;
		//}

        HWND hwnd = (HWND)_handle;
        if (!hwnd)
            return;

        auto size = f->rect().size.to<int32_t>();
		RECT rect = {};
		uint32_t flags = SWP_FRAMECHANGED;
        _form_styles = styles;
        _message_blocks.set(windowmessage_bock::all, true);
		auto[style, styleEx] = formStylesToWinStyles(f->styles());
		if(styles.any(ui::form_style::frameless))
		{
			POINT pt = { 0, 0 };
			::ClientToScreen(hwnd, &pt);
			rect = { pt.x, pt.y, pt.x + size.cx, pt.y + size.cy };
		}
		else
		{
			auto pos = f->windowPos().to<int32_t>();
			rect = { pos.x, pos.y, pos.x + size.cx, pos.y + size.cy };
			//::AdjustWindowRect(&rect, _style, FALSE);
			::AdjustWindowRectEx(&rect, style, FALSE, styleEx);
		}

		// ������������ť����Ҫ�����أ��� show
		if(styels_old.any(ui::form_style::notaskbutton) != styles.any(ui::form_style::notaskbutton))
		{
			::ShowWindow(hwnd, SW_HIDE);
			flags |= SWP_SHOWWINDOW;
		}
		else if(f->shown())
			style |= WS_VISIBLE;
		else {}

        ::SetWindowLongPtrW(hwnd, GWL_STYLE, style);
        ::SetWindowLongPtrW(hwnd, GWL_EXSTYLE, styleEx);
        _message_blocks.set(windowmessage_bock::all, false);

		HWND hwndInsert = NULL;
		// �����Ѿ�������styleex �� WS_EX_TOPMOST ������Ч��
		if(styels_old.any(ui::form_style::topmost) == styles.any(ui::form_style::topmost))
			flags |= SWP_NOZORDER;
		else
			hwndInsert = styles.any(ui::form_style::topmost) ? HWND_TOPMOST : HWND_NOTOPMOST;
        SetWindowPos(hwnd, hwndInsert, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, flags);

        f->repaint();
    }

    void Window::onPosChanged(const core::pointf & from, const core::pointf & to)
    {
        HWND hwnd = (HWND)_handle;
        if (!hwnd)
            return;

        auto pos = to.to<int32_t>();
        if (pos != _pos())
            _adjustWindow(pos, _size());
    }

    void Window::onSizeChanged(const core::sizef & from, const core::sizef & to)
    {
        //HWND hwnd = (HWND)_handle;
        //if (!hwnd)
        //    return;

        //auto size = to.to<int32_t>();
        //if (size != _size())
        //    _adjustWindow(_pos(), size);
    }

    void Window::onSceneInvalidated(const core::recti & rect)
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

    void Window::onSceneRendered2(const core::recti & rect)
    {
        _render(rect);
    }

    void Window::onSceneCaptured(bool capture)
    {
		check_invoke();

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

    void Window::onSceneEvented(ui::scene_event evt)
    {
        auto f = form();
        if (!f)
            throw core::exception(core::e_nullptr);

        _mouse_state.setWheelLines(0);
        switch(evt)
        {
        case ui::scene_event::update_mouse_pos:
            if(_mouse_state.hoving())
                f->notifyWindowMouse(_mouse_state, ui::mouse_button::none, ui::mouse_action::move);
            break;
        default:
            break;
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

        auto pf = f->parentForm();

        if (_handle)
            return core::ok;

        HINSTANCE hInstance = (HINSTANCE)core::instance();
        WNDCLASSEXW wcex = { sizeof(WNDCLASSEXW) };
        if (!GetClassInfoExW(hInstance, WINDOW_CLASS_NAME, &wcex))
        {
            wcex.cbSize = sizeof(WNDCLASSEXW);
            wcex.style = 0;
            wcex.lpfnWndProc = DefaultWndProc;
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
        auto size = f->rect().size;

		auto[style, styleEx] = formStylesToWinStyles(f->styles());
        RECT rect = { pos.x, pos.y, pos.x + size.cx, pos.y + size.cy };
        //::AdjustWindowRectEx(&rect, style, FALSE, styleEx);

        __currentWindow = this;
        std::u16string t = core::u8str_u16str(f->title());
        HWND hwnd = CreateWindowExW(
			styleEx, win32::WINDOW_CLASS_NAME, (const wchar_t *)t.c_str(), style | WS_BORDER | WS_DLGFRAME,
            rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
            pf ? (HWND)pf->handle() : NULL, NULL, hInstance, this);
		// TRICK: ������һ�£����Ա����� SetWindowRgn���� OnWmSize��
        ::SetWindowLongPtrW(hwnd, GWL_STYLE, style);

        //attatch(hwnd);
		assert(hwnd == _handle);
		//SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE);
        return core::ok;
    }

    core::error Window::_adjustWindowPos(const core::pointi & pos)
    {
        HWND hwnd = (HWND)_handle;
        if (!hwnd)
            return core::ok;

        uint32_t style = GetWindowLongW(hwnd, GWL_STYLE);
        uint32_t styleEx = GetWindowLongW(hwnd, GWL_EXSTYLE);
        RECT rect = { pos.x, pos.y, pos.x, pos.y };
        if (!_form_styles.any(ui::form_style::frameless))
            ::AdjustWindowRectEx(&rect, style, FALSE, styleEx);
        SetWindowPos(hwnd, 0, pos.x, pos.y, rect.right - rect.left, rect.bottom - rect.top,
            SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOCOPYBITS);
        return core::ok;
    }

    core::error Window::_adjustWindow(const core::pointi & pos, const core::sizei & size)
    {
        HWND hwnd = (HWND)_handle;
        if (!hwnd)
            return core::ok;

		uint32_t style = GetWindowLongW(hwnd, GWL_STYLE);
		uint32_t styleEx = GetWindowLongW(hwnd, GWL_EXSTYLE);
        RECT rect = { pos.x, pos.y, pos.x + size.cx, pos.y + size.cy };
        if (!_form_styles.any(ui::form_style::frameless))
            ::AdjustWindowRectEx(&rect, style, FALSE, styleEx);
        SetWindowPos(hwnd, 0, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
            SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOCOPYBITS);
        return core::ok;
    }

    core::pointi Window::_pos() const
    {
        if (!_handle)
            return {};
        HWND hwnd = (HWND)_handle;

        WINDOWINFO winfo;
        ::GetWindowInfo(hwnd, &winfo);

        if (_form_styles.any(ui::form_style::frameless))
            return core::vec2i(winfo.rcWindow.left, winfo.rcWindow.top);
        else
            return core::vec2i(winfo.rcClient.left, winfo.rcClient.top);
    }

    core::sizei Window::_size() const
    {
        if (!_handle)
            return {};

        HWND hwnd = (HWND)_handle;
        WINDOWINFO winfo = { sizeof(WINDOWINFO) };
        ::GetWindowInfo(hwnd, &winfo);
        return core::vec2i(winfo.rcClient.right - winfo.rcClient.left, winfo.rcClient.bottom - winfo.rcClient.top);
    }

    core::recti Window::_rect() const
    {
        if (!_handle)
            return {};
        HWND hwnd = (HWND)_handle;

        WINDOWINFO winfo;
        ::GetWindowInfo(hwnd, &winfo);

        if (_form_styles.any(ui::form_style::frameless))
            return core::recti(winfo.rcWindow.left, winfo.rcWindow.top, winfo.rcClient.right - winfo.rcClient.left, winfo.rcClient.bottom - winfo.rcClient.top);
        else
            return core::recti(winfo.rcClient.left, winfo.rcClient.top, winfo.rcClient.right - winfo.rcClient.left, winfo.rcClient.bottom - winfo.rcClient.top);
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

    void Window::_render(const core::recti & rect)
    {
		auto f = form();
		if(!f)
			return;

		HWND hwnd = (HWND)_handle;
		if(!hwnd)
			return;

        std::shared_ptr<drawing::GraphicsDevice> bitmap = f->bitmap();
		if(!bitmap)
			return;

		if(_form_styles.all(ui::form_style::layered))
		{
			auto rcwindow = _rect();
			POINT pos_dst = { rcwindow.x, rcwindow.y };
			SIZE size_dst = { rcwindow.cx, rcwindow.cy };
			POINT pos_src = { 0, 0 };
			BLENDFUNCTION bf = { 0 };
			bf.AlphaFormat = AC_SRC_ALPHA;
			bf.BlendFlags = 0;
			bf.BlendOp = AC_SRC_OVER;
			bf.SourceConstantAlpha = 0xFF;
			UpdateLayeredWindow(hwnd, NULL, &pos_dst, &size_dst, (HDC)bitmap->hdc(), &pos_src, 0, &bf, ULW_ALPHA);
		}
		else
		{
			HDC hsrc = (HDC)bitmap->hdc();
            HDC hdst = GetDC(hwnd);
			if(hsrc)
			{
				BitBlt(hdst, rect.x, rect.y, rect.cx, rect.cy, hsrc, rect.x, rect.y, SRCCOPY);
			}
			else
			{
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

				auto rc = rect.intersected(core::recti(core::pointi(), _size()));
				SetDIBitsToDevice(hdst,
					rc.x, rc.y, rc.cx, rc.cy,
					rc.x, buffer.size.cy - rc.y - rc.cy, 0, buffer.size.cy, buffer.data, &bmi, DIB_RGB_COLORS);
			}
            ReleaseDC(hwnd, hdst);
		}
    }

    void Window::_render(const drawing::Region & region)
    {
        auto f = form();
        if (!f)
            return;

        HWND hwnd = (HWND)_handle;
        if (!hwnd)
            return;

        std::shared_ptr<drawing::GraphicsDevice> bitmap = f->bitmap();
        if (!bitmap)
            return;

        if(_form_styles.all(ui::form_style::layered))
        {
            //async([this, &hwnd, &bitmap]()
            {
                auto rect = _rect();
                POINT pos_dst = { rect.x, rect.y };
                SIZE size_dst = { rect.cx, rect.cy };
                POINT pos_src = { 0, 0 };
                BLENDFUNCTION bf = { 0 };
                bf.AlphaFormat = AC_SRC_ALPHA;
                bf.BlendFlags = 0;
                bf.BlendOp = AC_SRC_OVER;
                bf.SourceConstantAlpha = 0xFF;
                UpdateLayeredWindow(hwnd, NULL, &pos_dst, &size_dst, (HDC)bitmap->hdc(), &pos_src, 0, &bf, ULW_ALPHA);
            }
            //);
        }
        else
        {
            HDC hdst = GetDC(hwnd);
            HDC hsrc = (HDC)bitmap->hdc();
            if(hsrc)
            {
                drawing::RegionIterator ri(region);
                while (!ri.done())
                {
                    auto rect = ri.rect();
                    ri.next();

                    auto rc = rect.intersected(core::recti(core::pointi(), _size()));
                    BitBlt(hdst, rc.x, rc.y, rc.cx, rc.cy, hsrc, rc.x, rc.y, SRCCOPY);
                }
            }
            else
            {
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

                drawing::RegionIterator ri(region);
                while (!ri.done())
                {
                    auto rect = ri.rect();
                    ri.next();

                    auto rc = rect.intersected(core::recti(core::pointi(), _size()));
                    SetDIBitsToDevice(hdst,
                        rc.x, rc.y, rc.cx, rc.cy,
                        rc.x, buffer.size.cy - rc.y - rc.cy, 0, buffer.size.cy, buffer.data, &bmi, DIB_RGB_COLORS);
                }
            }
            ReleaseDC(hwnd, hdst);
        }
    }

#define CASE_MSG(M, F) case M: return F(wParam, lParam)
    intx_t Window::handleMSG(uint32_t uiMessage, uintx_t wParam, intx_t lParam)
    {
        if (_message_blocks.any(windowmessage_bock::all))
            return OnDefault(uiMessage, wParam, lParam);

        message(uiMessage, wParam, lParam);
        auto f = form();
        if (!f)
            return OnDefault(uiMessage, wParam, lParam);

        switch (uiMessage)
        {
            CASE_MSG(WM_NCHITTEST, OnWmHitTest);
            CASE_MSG(WM_NCCALCSIZE, OnWmNcCalcSize);
            CASE_MSG(WM_SHOWWINDOW, OnWmShow);
            CASE_MSG(WM_ERASEBKGND, OnWmEraseBack);
            CASE_MSG(WM_PAINT, OnWmPaint);
            CASE_MSG(WM_NCPAINT, OnWmNcPaint);
            CASE_MSG(WM_NCACTIVATE, OnWmNcActivate);

            CASE_MSG(WM_MOUSEMOVE, OnWmMouseMove);
            CASE_MSG(WM_MOUSELEAVE, OnWmMouseLeave);

            CASE_MSG(WM_LBUTTONDOWN, OnWmMouseDownL);
            CASE_MSG(WM_LBUTTONUP, OnWmMouseUpL);

            CASE_MSG(WM_RBUTTONDOWN, OnWmMouseDownR);
            CASE_MSG(WM_RBUTTONUP, OnWmMouseUpR);

            //CASE_MSG(WM_MBUTTONDOWN, OnWmMouseDownM);
            //CASE_MSG(WM_MBUTTONUP, OnWmMouseUpM);

            CASE_MSG(WM_NCLBUTTONDOWN, OnWmNcMouseDownL);

            //CASE_MSG(WM_LBUTTONDBLCLK, OnWmMouseDBClick);
            CASE_MSG(WM_MOUSEWHEEL, OnWmMouseWheelV);

            CASE_MSG(WM_MOVE, OnWmMove);
            CASE_MSG(WM_SIZE, OnWmSize);
            //CASE_MSG(WM_SETTEXT, OnWmSetText);
            //CASE_MSG(WM_ACTIVATE, OnWmActive);
            CASE_MSG(WM_SETFOCUS, OnWmSetFocus);
            CASE_MSG(WM_KILLFOCUS, OnWmKillFocus);
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
            CASE_MSG(WM_CAPTURECHANGED, OnWmCaptureChanged);
            //CASE_MSG(WM_MOUSEACTIVATE, OnWmMouseActive);
            //CASE_MSG(WM_NCMOUSEMOVE, OnWmNcMouseMove);

            //CASE_MSG(WM_CLOSE, OnWmClose);
            //CASE_MSG(WM_DESTROY, OnWmDestroy);

            CASE_MSG(WM_GETMINMAXINFO, OnWmGetMinMaxInfo);
            CASE_MSG(WM_SYSCOMMAND, OnWmSysCommand);
            CASE_MSG(WM_WINDOWPOSCHANGING, OnWmWindowPosChanging);
            CASE_MSG(WM_WINDOWPOSCHANGED, OnWmWindowPosChanged);

            CASE_MSG(WM_REFRESH, OnWmRefresh);
        case WM_CLOSE:
            //return OnDefault(uiMessage, wParam, lParam);
            f->onAction(ui::system_action::close);
            break;
        default:
            return OnDefault(uiMessage, wParam, lParam);
        }
    }

    intx_t Window::OnDefault(uint32_t uiMessage, uintx_t wParam, intx_t lParam)
    {
        assert(_handle);
        HWND hwnd = (HWND)_handle;
        WNDPROC pfnOldWndProc = (WNDPROC)GetPropW(hwnd, win32::WINDOW_PROP_OLD_WNDPROC);
        if (!pfnOldWndProc)
            pfnOldWndProc = DefWindowProcW;
        return (intx_t)CallWindowProcW(pfnOldWndProc, hwnd, (UINT)uiMessage, (WPARAM)wParam, (LPARAM)lParam);
    }

    intx_t Window::OnWmMove(uintx_t wParam, intx_t lParam)
    {
        auto f = form();
        if (!f)
            throw core::exception(core::e_nullptr);

        f->setWindowPos(_pos().to<float32_t>());
        return 0;
    }

    intx_t Window::OnWmSize(uintx_t wParam, intx_t lParam)
    {
        auto size = _size();
        // https://docs.microsoft.com/zh-cn/windows/desktop/Controls/cookbook-overview
        //To avoid applying visual styles to a top level window, give the window a non-null region (SetWindowRgn). 
        //The system assumes that a window with a non-NULL region is a specialized window that does not use visual styles.
        //A child window associated with a non-visual-styles top level window may still apply visual styles even though the parent window does not.
        //WINDOWPOS & wp = *reinterpret_cast<WINDOWPOS *>(lParam);
        //if (_form_styles.any(ui::form_style::frameless))
        //{
        //    HRGN hrgn = CreateRectRgn(0, 0, size.cx, size.cy);
        //    SetWindowRgn((HWND)_handle, hrgn, FALSE);
        //}

        if (_message_blocks.any(windowmessage_bock::wm_size))
            return OnDefault(WM_SIZE, wParam, lParam);

        auto f = form();
        if (!f)
            throw core::exception(core::e_nullptr);

		if(wParam != SIZE_MINIMIZED)
			f->setWindowSize(size.to<float32_t>());
        switch(wParam)
        {
        case SIZE_MINIMIZED: f->setFormState(ui::form_state::minimize); break;
        case SIZE_MAXIMIZED: f->setFormState(ui::form_state::maximize); break;
        case SIZE_RESTORED: f->setFormState(ui::form_state::normalize); break;
        default: break;
        }
        return 0;
    }

     intx_t Window::OnWmRefresh(uintx_t wParam, intx_t lParam)
    {
        return 0;
    }

    intx_t Window::OnWmShow(uintx_t wParam, intx_t lParam)
    {
        if(!lParam)
        {
			if (auto f = form())
                f->notifyWindowShown(!!wParam);
        }
        return OnDefault(WM_SHOWWINDOW, wParam, lParam);
    }

    intx_t Window::OnWmPaint(uintx_t wParam, intx_t lParam)
    {
        //return OnDefault(WM_PAINT, wParam, lParam);
        HWND hwnd = (HWND)_handle;
        if (!hwnd)
            return OnDefault(WM_PAINT, wParam, lParam);

        //_render
        PAINTSTRUCT ps;
        HDC hdc = ::BeginPaint(hwnd, &ps);
		::EndPaint(hwnd, &ps);

        _render({ ps.rcPaint.left, ps.rcPaint.top,
            ps.rcPaint.right - ps.rcPaint.left,
            ps.rcPaint.bottom - ps.rcPaint.top });
        return 0;
    }

    intx_t Window::OnWmNcPaint(uintx_t wParam, intx_t lParam)
    {
        //return OnDefault(WM_NCPAINT, wParam, lParam);
        auto f = form();
        if (!f)
            throw core::exception(core::e_nullptr);
        auto styles = f->styles();
        if(styles.any(ui::form_style::frameless))
            return 0;

        return OnDefault(WM_NCPAINT, wParam, lParam);
    }

    intx_t Window::OnWmNcActivate(uintx_t wParam, intx_t lParam)
    {
        // ��������� frameless��ż������һ�� ncpaint �ұߡ�
        HWND hwnd = (HWND)_handle;
        if(!hwnd || ::IsIconic(hwnd))
            return OnDefault(WM_NCACTIVATE, wParam, lParam);

        if (_form_styles.any(ui::form_style::frameless))
            return 1;
        return OnDefault(WM_NCACTIVATE, wParam, lParam);

    }

    intx_t Window::OnWmMouseMove(uintx_t wParam, intx_t lParam)
    {
        auto f = form();
        if (!f)
            throw core::exception(core::e_nullptr);

        _mouse_state.setWheelLines(0);
        _mouse_state.setPos(core::pointi(core::i32li16((int32_t)lParam), core::i32hi16((int32_t)lParam)).to<float32_t>());
        if (!_trackingMouse)
        {
            //OnWmMouseEnter(uiMessage, wParam, lParam);
            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_LEAVE;
            tme.dwHoverTime = 1;
            tme.hwndTrack = (HWND)_handle;
            TrackMouseEvent(&tme);
            _trackingMouse = true;
            _mouse_state.setHoving(true);
            f->notifyWindowMouse(_mouse_state, ui::mouse_button::none, ui::mouse_action::enter);
        }

        f->notifyWindowMouse(_mouse_state, ui::mouse_button::none, ui::mouse_action::move);
        return 0;
    }

    intx_t Window::OnWmMouseLeave(uintx_t wParam, intx_t lParam)
    {
        if (!_trackingMouse)
            return 0;
        _trackingMouse = false;

        auto f = form();
        if (!f)
            throw core::exception(core::e_nullptr);

        _mouse_state.setWheelLines(0);
        _mouse_state.setButton(ui::mouse_button::mask, false);
        _mouse_state.setHoving(false);
        _mouse_state.setPos(core::pointi(core::i32li16((int32_t)lParam), core::i32hi16((int32_t)lParam)).to<float32_t>());
        f->notifyWindowMouse(_mouse_state, ui::mouse_button::none, ui::mouse_action::leave);
        if(_cursor_context)
            _cursor_context->reset();
        return 0;
    }

    intx_t Window::OnWmMouseDownL(uintx_t wParam, intx_t lParam)
    {
        auto f = form();
        if (!f)
            throw core::exception(core::e_nullptr);

        _mouse_state.setWheelLines(0);
        _mouse_state.setButton(ui::mouse_button::left, true);
        _mouse_state.setPos(core::pointi(core::i32li16((int32_t)lParam), core::i32hi16((int32_t)lParam)).to<float32_t>());
        f->notifyWindowMouse(_mouse_state, ui::mouse_button::left, ui::mouse_action::press);
        return 0;
    }

    intx_t Window::OnWmMouseDownR(uintx_t wParam, intx_t lParam)
    {
        auto f = form();
        if (!f)
            throw core::exception(core::e_nullptr);

        _mouse_state.setWheelLines(0);
        _mouse_state.setButton(ui::mouse_button::right, true);
        _mouse_state.setPos(core::pointi(core::i32li16((int32_t)lParam), core::i32hi16((int32_t)lParam)).to<float32_t>());
        f->notifyWindowMouse(_mouse_state, ui::mouse_button::right, ui::mouse_action::press);
        return 0;
    }

    intx_t Window::OnWmMouseUpL(uintx_t wParam, intx_t lParam)
    {
        auto f = form();
        if (!f)
            throw core::exception(core::e_nullptr);

        _mouse_state.setWheelLines(0);
        _mouse_state.setButton(ui::mouse_button::left, false);
        _mouse_state.setPos(core::pointi(core::i32li16((int32_t)lParam), core::i32hi16((int32_t)lParam)).to<float32_t>());
        f->notifyWindowMouse(_mouse_state, ui::mouse_button::left, ui::mouse_action::click);
        f->notifyWindowMouse(_mouse_state, ui::mouse_button::left, ui::mouse_action::release);
        return 0;
    }

    intx_t Window::OnWmMouseUpR(uintx_t wParam, intx_t lParam)
    {
        auto f = form();
        if (!f)
            throw core::exception(core::e_nullptr);

        _mouse_state.setWheelLines(0);
        _mouse_state.setButton(ui::mouse_button::right, false);
        _mouse_state.setPos(core::pointi(core::i32li16((int32_t)lParam), core::i32hi16((int32_t)lParam)).to<float32_t>());
        f->notifyWindowMouse(_mouse_state, ui::mouse_button::right, ui::mouse_action::click);
        f->notifyWindowMouse(_mouse_state, ui::mouse_button::right, ui::mouse_action::release);
        return 0;
    }

    intx_t Window::OnWmMouseWheelV(uintx_t wParam, intx_t lParam)
    {
        HWND hwnd = (HWND)_handle;
        if (!hwnd)
            return 0;

        auto f = form();
        if (!f)
            throw core::exception(core::e_nullptr);

        POINT point = { core::i32li16((int32_t)lParam), core::i32hi16((int32_t)lParam) };
        ::ScreenToClient(hwnd, &point);
        //core::pointi point = core::pointi(core::i32li16(lParam), core::i32hi16(lParam));
        //core::pointi wheel = core::pointi(core::u32li16(wParam), core::u32hi16(wParam));
        _mouse_state.setWheelLines(core::u32hi16((uint32_t)wParam) / WHEEL_DELTA);
        _mouse_state.setPos(core::pointi(point.x, point.y).to<float32_t>());
        f->notifyWindowMouse(_mouse_state, ui::mouse_button::none,  ui::mouse_action::wheel_v);
        return 0;
    }

	intx_t Window::OnWmNcMouseDownL(uintx_t wParam, intx_t lParam)
    {
		return OnDefault(WM_NCLBUTTONDOWN, wParam, lParam);
    }

    intx_t Window::OnWmChar(uintx_t wParam, intx_t lParam)
    {
        if (auto f = form())
            f->notifyWindowCharInput(char32_t(wParam));
        return 0;
    }

    intx_t Window::OnWmKeyDown(uintx_t wParam, intx_t lParam)
    {
        auto f = form();
        if (!f)
            throw core::exception(core::e_nullptr);

        ui::keycode key = virtualkey2keycode(wParam);
        _mouse_state.setKey(key, true);
        f->notifyWindowKey(_mouse_state, key, ui::key_action::press);
        return 0;
    }

    intx_t Window::OnWmKeyUp(uintx_t wParam, intx_t lParam)
    {
        auto f = form();
        if (!f)
            throw core::exception(core::e_nullptr);

        ui::keycode key = virtualkey2keycode(wParam);
        _mouse_state.setKey(key, false);
        f->notifyWindowKey(_mouse_state, key, ui::key_action::release);
        return 0;
    }

    intx_t Window::OnWmHitTest(uintx_t wParam, intx_t lParam)
    {
        intx_t def = OnDefault(WM_NCHITTEST, wParam, lParam);
        if (def != HTCLIENT)
            return def;

        HWND hwnd = (HWND)_handle;
        if (!hwnd)
            return def;

        auto f =form();

        POINT posi = { core::i32li16((int32_t)lParam), core::i32hi16((int32_t)lParam) };
        ScreenToClient(hwnd, &posi);
        auto pos = core::pointf((float32_t)posi.x, (float32_t)posi.y);
        auto child = f->findChild(pos);
        if(!child)
            return HTNOWHERE;

        auto htf = child->hitTestForm(pos - child->rect().pos);
        switch (htf)
        {
        case ui::hittest_form::client: return HTCLIENT;
        case ui::hittest_form::caption: return HTCAPTION;
        case ui::hittest_form::resize_leftTop: return HTTOPLEFT;
        case ui::hittest_form::resize_top: return HTTOP;
        case ui::hittest_form::resize_rightTop: return HTTOPRIGHT;
        case ui::hittest_form::resize_right: return HTRIGHT;
        case ui::hittest_form::resize_rightBottom: return HTBOTTOMRIGHT;
        case ui::hittest_form::resize_bottom: return HTBOTTOM;
        case ui::hittest_form::resize_leftBottom: return HTBOTTOMLEFT;
        case ui::hittest_form::resize_left: return HTLEFT;
        default: return HTNOWHERE;
        }
    }

	intx_t Window::OnWmCaptureChanged(uintx_t wParam, intx_t lParam)
    {
		if(auto f = form())
			f->notifyWindowCaptured(_handle == GetCapture());
		return OnDefault(WM_CAPTURECHANGED, wParam, lParam);
    }

	intx_t Window::OnWmMouseActive(uintx_t wParam, intx_t lParam)
    {
		if (auto f = form())
			f->notifyWindowCaptured(_handle == GetCapture());
		return OnDefault(WM_MOUSEACTIVATE, wParam, lParam);
    }

    intx_t Window::OnWmSetFocus(uintx_t wParam, intx_t lParam)
    {
		_mouse_state.setFocused(true);
		if (auto f = form())
			f->notifyWindowFocused(_mouse_state, true);
		return OnDefault(WM_SETFOCUS, wParam, lParam);
    }

    intx_t Window::OnWmKillFocus(uintx_t wParam, intx_t lParam)
    {
		_mouse_state.setFocused(false);
		_mouse_state.setAllKeys(false);
		if (auto f = form())
			f->notifyWindowFocused(_mouse_state, false);
		return OnDefault(WM_KILLFOCUS, wParam, lParam);
    }

    intx_t Window::OnWmNcCalcSize(uintx_t wParam, intx_t lParam)
    {
        // �����Ϣֻ������ȷ���ͻ�����С���������ƴ�������
        HWND hwnd = (HWND)_handle;
        if (!hwnd)
            return OnDefault(WM_NCCALCSIZE, wParam, lParam);

        if(wParam)
        {
            NCCALCSIZE_PARAMS & ncsize = *reinterpret_cast<NCCALCSIZE_PARAMS *>(lParam);
            if (_form_styles.any(ui::form_style::frameless))
            {
                //WINDOWPOS & wpos = *ncsize.lppos;
                //RECT & wrect = ncsize.rgrc[0];
                //ncsize.rgrc[1] = ncsize.rgrc[0];
                //wpos.x = 0;
                //wpos.y = 0;
                //wpos.cx = 0;
                //wpos.cy = 0;
                return 0;
            }
        }
        else
        {
            // ֻ��Ҫ��������λ��
            RECT & rect = *reinterpret_cast<RECT *>(lParam);
            if (_form_styles.any(ui::form_style::frameless))
                return 0;
        }
        return OnDefault(WM_NCCALCSIZE, wParam, lParam);
    }

    intx_t Window::OnWmGetMinMaxInfo(uintx_t wParam, intx_t lParam)
    {
        HWND hwnd = (HWND)_handle;
        auto f = form();
        if (!hwnd || !f)
            return OnDefault(WM_GETMINMAXINFO, wParam, lParam);

        if (_form_styles.any(ui::form_style::frameless))
        {
            MINMAXINFO & info = *reinterpret_cast<MINMAXINFO *>(lParam);

            HMONITOR hm = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY);
            MONITORINFO mi = { sizeof(MONITORINFO) };
            ::GetMonitorInfoW(hm, &mi);
            info.ptMaxPosition = { mi.rcWork.left - mi.rcMonitor.left, mi.rcWork.top - mi.rcMonitor.top };
            info.ptMaxSize = { mi.rcWork.right - mi.rcWork.left, mi.rcWork.bottom - mi.rcWork.top };
            info.ptMaxTrackSize = info.ptMaxSize;
            auto & min = f->minSize();
            auto & max = f->maxSize();
            if (min.cx.avi())
                info.ptMinTrackSize.x = (int32_t)f->calc(min.cx);
            if (min.cy.avi())
                info.ptMinTrackSize.x = (int32_t)f->calc(min.cy);
            if (max.cx.avi())
                info.ptMaxTrackSize.x = (int32_t)f->calc(max.cx);
            if (max.cy.avi())
                info.ptMaxTrackSize.x = (int32_t)f->calc(max.cy);
            return OnDefault(WM_GETMINMAXINFO, wParam, lParam);
        }
        else
            return OnDefault(WM_GETMINMAXINFO, wParam, lParam);
    }

    intx_t Window::OnWmSysCommand(uintx_t wParam, intx_t lParam)
    {
        return OnDefault(WM_SYSCOMMAND, wParam, lParam);
    }

    intx_t Window::OnWmWindowPosChanging(uintx_t wParam, intx_t lParam)
    {
        return OnDefault(WM_WINDOWPOSCHANGING, wParam, lParam);
    }

    intx_t Window::OnWmWindowPosChanged(uintx_t wParam, intx_t lParam) 
    {
        return OnDefault(WM_WINDOWPOSCHANGED, wParam, lParam);
    }

    Window * Window::fromHandle(pointer_t handle)
    {
        return (Window *)(void *)GetPropW((HWND)handle, win32::WINDOW_PROP_THIS_PTR);
    }


    static LRESULT CALLBACK WindowWndProc(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
    {
        Window * pWindow = __currentWindow ? __currentWindow : Window::fromHandle(hWnd);
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
            //Printf("��Ϣ��[%02d:%02d:%02d:%03d][0x%04X][%s](%u, %d)\n", time.Hour, time.Minute, time.Second, time.MimiSecond, 
            //uiMessage, GetWmMessageText(uiMessage), wParam, lParam);
            break;
        }

        if (pWindow)
        {
            res = pWindow->handleMSG(uiMessage, wParam, lParam);
        }
        else
        {
            //TimeT time;
            //Printf(_T("δ�������Ϣ��[%02d:%02d:%02d:%03d][0x%04X][%s](%u, %d)\n"), time.Hour, time.Minute, time.Second, time.MimiSecond,
            //	   uiMessage, GetWmMessageText(uiMessage), wParam, lParam);
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
				core::app().quit(0);
                break;
            default:
                res = DefWindowProcW(hWnd, uiMessage, wParam, lParam);
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
            //! �����õİ����롣
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
            /*0x11*/ui::keycode::ctrl,
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
            /*0x41*/ui::keycode::A,
            /*0x42*/ui::keycode::B,
            /*0x43*/ui::keycode::C,
            /*0x44*/ui::keycode::D,
            /*0x45*/ui::keycode::E,
            /*0x46*/ui::keycode::F,
            /*0x47*/ui::keycode::G,
            /*0x48*/ui::keycode::H,
            /*0x49*/ui::keycode::I,
            /*0x4a*/ui::keycode::J,
            /*0x4b*/ui::keycode::K,
            /*0x4c*/ui::keycode::L,
            /*0x4d*/ui::keycode::M,
            /*0x4e*/ui::keycode::N,
            /*0x4f*/ui::keycode::O,
            /*0x50*/ui::keycode::P,
            /*0x51*/ui::keycode::Q,
            /*0x52*/ui::keycode::R,
            /*0x53*/ui::keycode::S,
            /*0x54*/ui::keycode::T,
            /*0x55*/ui::keycode::U,
            /*0x56*/ui::keycode::V,
            /*0x57*/ui::keycode::W,
            /*0x58*/ui::keycode::X,
            /*0x59*/ui::keycode::Y,
            /*0x5a*/ui::keycode::Z,
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

            /*0x70*/ui::keycode::F1,
            /*0x71*/ui::keycode::F2,
            /*0x72*/ui::keycode::F3,
            /*0x73*/ui::keycode::F4,
            /*0x74*/ui::keycode::F5,
            /*0x75*/ui::keycode::F6,
            /*0x76*/ui::keycode::F7,
            /*0x77*/ui::keycode::F8,
            /*0x78*/ui::keycode::F9,
            /*0x79*/ui::keycode::F10,
            /*0x7a*/ui::keycode::F11,
            /*0x7b*/ui::keycode::F12,
            /*0x7c*/ui::keycode::F13,
            /*0x7d*/ui::keycode::F14,
            /*0x7e*/ui::keycode::F15,
            /*0x7f*/ui::keycode::F16,
            /*0x80*/ui::keycode::F17,
            /*0x81*/ui::keycode::F18,
            /*0x82*/ui::keycode::F19,
            /*0x83*/ui::keycode::F20,
            /*0x84*/ui::keycode::F21,
            /*0x85*/ui::keycode::F22,
            /*0x86*/ui::keycode::F23,
            /*0x87*/ui::keycode::F24,
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
            /*0xa2*/ui::keycode::ctrlL,
            /*0xa3*/ui::keycode::ctrlR,
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

			NONE,
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