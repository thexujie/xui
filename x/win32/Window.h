#pragma once
#include "ui/component/Window.h"
#include "ui/Form.h"
#include "win32/win32.h"
#include "ImeContext.h"

namespace win32
{
    enum class windowmessage_bock
    {
        all = 0x10000,
        wm_size = 0x0001,
    };
    typedef core::bitflag<windowmessage_bock> windowmessage_bocks;

    class Window : public ui::component::Window
    {
    public:
        Window();
        ~Window();

        core::error attatch(std::shared_ptr<ui::Form> form);
        core::error attatch(handle_t handle);
        void detach();

        void move(const core::pt32f & pos);
        void resize(const core::sizef & size);

        std::shared_ptr<ui::Form> form() { return _form.lock(); }
        std::shared_ptr<ui::Scene> scene() { if (auto f = _form.lock()) { return f->scene(); } return nullptr; }
        handle_t handle() const;
        intx_t handleMSG(uint32_t uiMessage, uintx_t wParam, intx_t lParam);

    private:
        void onStateChanged(ui::form_state, ui::form_state state);
        void onStylesChanged(ui::form_styles, ui::form_styles styles);
        void onPosChanged(const core::pt32f & from, const core::pt32f & to);
        void onSizeChanged(const core::sizef & from, const core::sizef & to);
        void onSceneInvalidated(const core::recti & rect);
        void onSceneRendered(const drawing::Region & region);
        void onSceneRendered2(const core::recti & rect);
        void onSceneCaptured(bool capture);
        void onSceneEvented(ui::scene_event evt);

    private:
        core::error _createWindow();
        core::error _adjustWindow(const core::pt32i & pos, const core::sizei & size);

        core::pt32i _pos() const;
        core::sizei _size() const;
        core::recti _rect() const;
        core::vec4i _border() const;
        core::vec4i _padding() const;

        void _render(const core::recti & rect);
        void _render(const drawing::Region & region);

    private:

        intx_t OnDefault(uint32_t uiMessage, uintx_t wParam, intx_t lParam);

        virtual intx_t OnWmEraseBack(uintx_t wParam, intx_t lParam) { return 0; }
        virtual intx_t OnWmMove(uintx_t wParam, intx_t lParam);
        virtual intx_t OnWmSize(uintx_t wParam, intx_t lParam);
        virtual intx_t OnWmRefresh(uintx_t wParam, intx_t lParam);

        virtual intx_t OnWmShow(uintx_t wParam, intx_t lParam);
        virtual intx_t OnWmPaint(uintx_t wParam, intx_t lParam);
        virtual intx_t OnWmNcPaint(uintx_t wParam, intx_t lParam);
        virtual intx_t OnWmNcActivate(uintx_t wParam, intx_t lParam);

        virtual intx_t OnWmMouseMove(uintx_t wParam, intx_t lParam);
        virtual intx_t OnWmMouseLeave(uintx_t wParam, intx_t lParam);
        virtual intx_t OnWmMouseDownL(uintx_t wParam, intx_t lParam);
        virtual intx_t OnWmMouseUpL(uintx_t wParam, intx_t lParam);
        virtual intx_t OnWmMouseWheelV(uintx_t wParam, intx_t lParam);

        virtual intx_t OnWmNcMouseDownL(uintx_t wParam, intx_t lParam) { return 0; }
        virtual intx_t OnWmMouseDownR(uintx_t wParam, intx_t lParam){ return 0;}
        virtual intx_t OnWmMouseUpR(uintx_t wParam, intx_t lParam){ return 0;}
        virtual intx_t OnWmMouseDownM(uintx_t wParam, intx_t lParam){ return 0;}
        virtual intx_t OnWmMouseUpM(uintx_t wParam, intx_t lParam){ return 0;}

        virtual intx_t OnWmSetText(uintx_t wParam, intx_t lParam){ return 0;}
        virtual intx_t OnWmActive(uintx_t wParam, intx_t lParam){ return 0;}
        virtual intx_t OnWmSetFocus(uintx_t wParam, intx_t lParam);
        virtual intx_t OnWmKillFocus(uintx_t wParam, intx_t lParam);

        virtual intx_t OnWmChar(uintx_t wParam, intx_t lParam);
        virtual intx_t OnWmUnicodeChar(uintx_t wParam, intx_t lParam)
        {
            //if(wParam != UNICODE_NOCHAR)
            //	m_pControl->PreOnKeyInput((char_32)wParam);

            // utf16 窗口无法收到这个消息 
            // https://msdn.microsoft.com/en-us/library/windows/desktop/ms646288(v=vs.85).aspx
            // WM_UNICHAR 返回 true 可以拒绝 WM_CHAR.
            return 0;
        }
        virtual intx_t OnWmKeyDown(uintx_t wParam, intx_t lParam);
        virtual intx_t OnWmKeyUp(uintx_t wParam, intx_t lParam);

        virtual intx_t OnWmMouseDBClick(uintx_t wParam, intx_t lParam){ return 0;}

        virtual intx_t OnWmSetCursor(uintx_t wParam, intx_t lParam){ return 0;}
        virtual intx_t OnWmDropFiles(uintx_t wParam, intx_t lParam){ return 0;}
        virtual intx_t OnWmQueryDrag(uintx_t wParam, intx_t lParam){ return 0;}

        virtual intx_t OnWmHitTest(uintx_t wParam, intx_t lParam);
        virtual intx_t OnWmNotify(uintx_t wParam, intx_t lParam){ return 0;}

        virtual intx_t OnWmClose(uintx_t wParam, intx_t lParam){ return 0;}
        virtual intx_t OnWmDestroy(uintx_t wParam, intx_t lParam){ return 0;}
		virtual intx_t OnWmCaptureChanged(uintx_t wParam, intx_t lParam);
        virtual intx_t OnWmMouseActive(uintx_t wParam, intx_t lParam){ return 0;}

        virtual intx_t OnWmNcCalcSize(uintx_t wParam, intx_t lParam);
        virtual intx_t OnWmNcMouseMove(uintx_t wParam, intx_t lParam){ return 0;}
        virtual intx_t OnWmGetMinMaxInfo(uintx_t wParam, intx_t lParam);
        virtual intx_t OnWmSysCommand(uintx_t wParam, intx_t lParam);
        virtual intx_t OnWmWindowPosChanging(uintx_t wParam, intx_t lParam);
        virtual intx_t OnWmWindowPosChanged(uintx_t wParam, intx_t lParam);

    private:
        std::weak_ptr<ui::Form> _form;
        ui::form_styles _form_styles = nullptr;
        handle_t _handle = nullptr;
        windowmessage_bocks _message_blocks = nullptr;

        ui::input_state _mouse_state;
        bool _mouseIn = false;
        bool _trackingMouse = false;

        std::shared_ptr<win32::ImeContext> _ime_context;
        std::shared_ptr<win32::CursorContext> _cursor_context;

    public:
        core::event<void()> closed;

    public:
        static Window * fromHandle(handle_t handle);
    };

    ui::keycode virtualkey2keycode(uint32_t ivirtualkey);
    uint32_t keycode2virtualkey(ui::keycode code);
}
