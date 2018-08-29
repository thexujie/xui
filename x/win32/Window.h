#pragma once
#include "ui/component/Window.h"
#include "ui/Form.h"
#include "win32/win32.h"

namespace win32
{
    /**
    * Êó±ê°´Å¥¡£
    */
    enum MouseButtonE
    {
        MouseButtonNone = 0,
        MouseButtonL = 1,
        MouseButtonR = 2,
        MouseButtonM = 3,
        MouseButtonBack = 4,
        MouseButtonForward = 5,
    };

    /**
    * ¼üÅÌ°´¼ü¡£
    */
    enum KeyCodeE
    {
        //! ²»¿ÉÓÃµÄ°´¼üÂë¡£
        KeyCodeInvalid = 0x0,

        // Êý×Ö¼ü
        KeyCode0,
        KeyCode1,
        KeyCode2,
        KeyCode3,
        KeyCode4,
        KeyCode5,
        KeyCode6,
        KeyCode7,
        KeyCode8,
        KeyCode9,
        // ×Ö·û¼ü
        KeyCodeCapsLock,
        KeyCodeA,
        KeyCodeB,
        KeyCodeC,
        KeyCodeD,
        KeyCodeEe,
        KeyCodeF,
        KeyCodeG,
        KeyCodeH,
        KeyCodeI,
        KeyCodeJ,
        KeyCodeK,
        KeyCodeL,
        KeyCodeM,
        KeyCodeN,
        KeyCodeO,
        KeyCodeP,
        KeyCodeQ,
        KeyCodeR,
        KeyCodeS,
        KeyCodeT,
        KeyCodeU,
        KeyCodeV,
        KeyCodeW,
        KeyCodeX,
        KeyCodeY,
        KeyCodeZ,

        KeyCodeBackspace,
        KeyCodeTab,
        KeyCodeEnter,

        KeyCodeEscape,
        KeyCodeSpace,

        // Ð¡¼üÅÌ
        KeyCodeNumlock,
        KeyCodeNum0,
        KeyCodeNum1,
        KeyCodeNum2,
        KeyCodeNum3,
        KeyCodeNum4,
        KeyCodeNum5,
        KeyCodeNum6,
        KeyCodeNum7,
        KeyCodeNum8,
        KeyCodeNum9,
        KeyCodeNumAdd, // +
        KeyCodeNumSub, // -
        KeyCodeNumMul, // *
        KeyCodeNumDiv,  // /
        KeyCodeNumDecimal, // .
        KeyCodeNumEnter, // enter
        KeyCodeNumEqual, // equal

                         // ¹¦ÄÜ¼ü
                         KeyCodeF1,
                         KeyCodeF2,
                         KeyCodeF3,
                         KeyCodeF4,
                         KeyCodeF5,
                         KeyCodeF6,
                         KeyCodeF7,
                         KeyCodeF8,
                         KeyCodeF9,
                         KeyCodeF10,
                         KeyCodeF11,
                         KeyCodeF12,
                         KeyCodeF13,
                         KeyCodeF14,
                         KeyCodeF15,
                         KeyCodeF16,
                         KeyCodeF17,
                         KeyCodeF18,
                         KeyCodeF19,
                         KeyCodeF20,
                         KeyCodeF21,
                         KeyCodeF22,
                         KeyCodeF23,
                         KeyCodeF24,

                         // --  Sign
                         KeyCodeSub, // ¼õºÅ -_
                         KeyCodeEqual, // µÈÓÚ =+
                         KeyCodeGrave, // ÒÖÒô·û `~
                         KeyCodeComma, // ¶ººÅ £¬<
                         KeyCodePeriod, // ¾äºÅ .>
                         KeyCodeSlash, // Ð±¸Ü /?
                         KeyCodeSemicolon, // ·ÖºÅ ;:
                         KeyCodeColon, // Ã°ºÅ : (Ä³Ð©¼üÅÌÓÐµ¥¶ÀµÄÃ°ºÅ)
                         KeyCodeApostrophe, // µ¥ÒýºÅ¡¢Ê¡×Ö·û '
                         KeyCodeBracketL, // ×ó·½À¨ºÅ [{
                         KeyCodeBracketR, // ÓÒ·½À¨ºÅ ]}
                         KeyCodeBackslash, // ·´Ð±¸Ü \|

                         KeyCodeInsert,
                         KeyCodeDelete,
                         KeyCodeHome,
                         KeyCodeEnd,
                         KeyCodePageUp,
                         KeyCodePageDown,

                         KeyCodeLeft,
                         KeyCodeUp,
                         KeyCodeRight,
                         KeyCodeDown,

                         KeyCodePrintScreen,
                         KeyCodeScrollLock,
                         KeyCodePauseBreak,

                         // -- Win
                         KeyCodeWinL,
                         KeyCodeWinR,
                         // -- Ctrl
                         KeyCodeControl,
                         KeyCodeControlL,
                         KeyCodeControlR,
                         // -- Shift
                         KeyCodeShift,
                         KeyCodeShiftL,
                         KeyCodeShiftR,
                         // -- Alt
                         KeyCodeAlt,
                         KeyCodeAltL,
                         KeyCodeAltR,
                         // -- App
                         KeyCodeApps,

                         // ÌØÊâ¼ü
                         KeyCodeSleep,

                         KeyCodeClear,
                         KeyCodeSelect,
                         KeyCodePrint,
                         KeyCodeExcute,
                         KeyCodeHelp,

                         // -- IME
                         KeyCodeImeKana,
                         KeyCodeImeHangul,
                         KeyCodeImeJunja,
                         KeyCodeImeFinal,
                         KeyCodeImeHanja,
                         KeyCodeImeKanji,
                         KeyCodeImeConvert,
                         KeyCodeImeNonConvert,
                         KeyCodeImeAccept,
                         KeyCodeImeModeChange,
                         KeyCodeImeProcessKey,

                         // -- Browser
                         KeyCodeBrowserBack,
                         KeyCodeBrowserForward,
                         KeyCodeBrowserRefresh,
                         KeyCodeBrowserStop,
                         KeyCodeBrowserSearch,
                         KeyCodeBrowserFavorites,
                         KeyCodeBrowserHome,

                         // -- Volume
                         //! ¾²Òô
                         KeyCodeVolumeMute,
                         KeyCodeVolumeDown,
                         KeyCodeVolumeUp,

                         // -- Media
                         KeyCodeMediaNext,
                         KeyCodeMediaPrev,
                         KeyCodeMediaStop,
                         KeyCodeMediaPlayPause,

                         // -- Launch
                         KeyCodeLaunchMail,
                         KeyCodeLaunchMediaSelect,
                         KeyCodeLaunchApp1,
                         KeyCodeLaunchApp2,

                         // -- ÆäËû
                         KeyCodeElseAttn,
                         KeyCodeElseArsel,
                         KeyCodeElseExsel,
                         KeyCodeElseEreof,
                         KeyCodeElsePlay,
                         KeyCodeElseZoom,
                         KeyCodeElsePa1,
                         KeyCodeElseClear,

                         KeyCodeCount,
    };

    class Window : public ui::component::Window
    {
    public:
        Window();
        ~Window();

        core::error attatch(std::shared_ptr<ui::Form> form);
        core::error attatch(handle_t handle);
        void detach();

        void move(const core::pt32f & pos);
        void resize(const core::si32f & size);

        std::shared_ptr<ui::Form> form() { return _form.lock(); }
        handle_t handle() const;
        intx_t handleMSG(uint32_t uiMessage, uintx_t uiParam, intx_t iParam);

    private:
        void onShownChanged(bool shown);
        void onPosChanged(const core::pt32f & from, const core::pt32f & to);
        void onSizeChanged(const core::si32f & from, const core::si32f & to);
        void onSceneInvalidated(const core::rc32i & rect);
        void onSceneRendered(const graphics::Region & region);
        void onSceneCaptured(bool capture);

    private:
        core::error _createWindow();
        core::error _adjustWindow(const core::pt32i & pos, const core::si32i & size);

        core::pt32i _pos() const;
        core::si32i _size() const;
        core::vec4i _border() const;
        core::vec4i _padding() const;

        void _render(const core::rc32i & rect);
        void _render(const graphics::Region & region);

    private:

        intx_t OnDefault(uint32_t uiMessage, uintx_t uiParam, intx_t iParam);

        virtual intx_t OnWmEraseBack(uintx_t uiParam, intx_t iParam) { return 0; }
        virtual intx_t OnWmMove(uintx_t uiParam, intx_t iParam);
        virtual intx_t OnWmSize(uintx_t uiParam, intx_t iParam);
        virtual intx_t OnWmRefresh(uintx_t uiParam, intx_t iParam);

        virtual intx_t OnWmShow(uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmPaint(uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmNcPaint(uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmNcActivate(uintx_t uiParam, intx_t iParam){ return 0;}

        virtual intx_t OnWmMouseMove(uintx_t uiParam, intx_t iParam);
        virtual intx_t OnWmMouseLeave(uintx_t uiParam, intx_t iParam);
        virtual intx_t OnWmMouseDownL(uintx_t uiParam, intx_t iParam);
        virtual intx_t OnWmMouseUpL(uintx_t uiParam, intx_t iParam);
        virtual intx_t OnWmMouseWheelV(uintx_t uiParam, intx_t iParam);

        virtual intx_t OnWmMouseDown(uintx_t uiParam, intx_t iParam, MouseButtonE eButton){ return 0;}
        virtual intx_t OnWmMouseUp(uintx_t uiParam, intx_t iParam, MouseButtonE eButton){ return 0;}
        virtual intx_t OnWmMouseClick(uintx_t uiParam, intx_t iParam, MouseButtonE eButton){ return 0;}

        virtual intx_t OnWmNcMouseDownL(uintx_t uiParam, intx_t iParam) { return 0; }
        virtual intx_t OnWmMouseDownR(uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmMouseUpR(uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmMouseDownM(uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmMouseUpM(uintx_t uiParam, intx_t iParam){ return 0;}

        virtual intx_t OnWmSetText(uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmActive(uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmSetFocus(uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmKillFocus(uintx_t uiParam, intx_t iParam){ return 0;}

        virtual intx_t OnWmChar(uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmUnicodeChar(uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmKeyDown(uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmKeyUp(uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmMouseDBClick(uintx_t uiParam, intx_t iParam){ return 0;}

        virtual intx_t OnWmSetCursor(uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmDropFiles(uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmQueryDrag(uintx_t uiParam, intx_t iParam){ return 0;}

        virtual intx_t OnWmHitTest(uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmNotify(uintx_t uiParam, intx_t iParam){ return 0;}

        virtual intx_t OnWmClose(uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmDestroy(uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmCaptureChanged(uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmMouseActive(uintx_t uiParam, intx_t iParam){ return 0;}

        virtual intx_t OnWmNcCalcSize(uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmNcMouseMove(uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmGetMinMaxInfo(uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmSysCommand(uintx_t uiParam, intx_t iParam){ return 0;}

    private:
        std::weak_ptr<ui::Form> _form;
        uint32_t _style = 0;
        uint32_t _styleEx = 0;
        handle_t _handle = nullptr;

        ui::component::mosue_state _mouse_state;
        bool _mouseIn = false;
        bool _trackingMouse = false;
    public:
        core::event<void()> closed;

    public:
        static Window * fromHandle(handle_t handle);
    };
}
