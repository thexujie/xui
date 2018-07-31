#pragma once
#include "controls/component/Window.h"
#include "controls/Form.h"
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

    class Window : public controls::component::Window, public std::enable_shared_from_this<Window>
    {
    public:
        Window();
        ~Window();

        core::error attatch(std::shared_ptr<controls::Form> form);
        core::error attatch(handle_t handle);
        handle_t handle() const;
        intx_t handleMSG(uint32_t uiMessage, uintx_t uiParam, intx_t iParam);

    private:
        void onShownChanged(bool shown);
        void onPosChagned(const core::pt32f & from, const core::pt32f & to);

    private:
        core::error _createWindow();

    private:

        intx_t OnDefault(uint32_t uiMessage, uintx_t uiParam, intx_t iParam);

        virtual intx_t OnWmMove(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmSize(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}

        virtual intx_t OnWmShow(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmPaint(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmNcPaint(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmNcActivate(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmEraseBack(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmMouseMove(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}

        virtual intx_t OnWmMouseEnter(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmMouseLeave(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}

        virtual intx_t OnWmMouseDown(uint32_t uiMessage, uintx_t uiParam, intx_t iParam, MouseButtonE eButton){ return 0;}
        virtual intx_t OnWmMouseUp(uint32_t uiMessage, uintx_t uiParam, intx_t iParam, MouseButtonE eButton){ return 0;}
        virtual intx_t OnWmMouseClick(uint32_t uiMessage, uintx_t uiParam, intx_t iParam, MouseButtonE eButton){ return 0;}

        virtual intx_t OnWmMouseDownL(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmNcMouseDownL(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmMouseUpL(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmMouseDownR(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmMouseUpR(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmMouseDownM(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmMouseUpM(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmMouseWheelV(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}

        virtual intx_t OnWmSetText(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmActive(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmSetFocus(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmKillFocus(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}

        virtual intx_t OnWmChar(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmUnicodeChar(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmKeyDown(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmKeyUp(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmMouseDBClick(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}

        virtual intx_t OnWmSetCursor(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmDropFiles(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmQueryDrag(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}

        virtual intx_t OnWmHitTest(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmNotify(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}

        virtual intx_t OnWmClose(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmDestroy(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmCaptureChanged(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmMouseActive(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}

        virtual intx_t OnWmNcCalcSize(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmNcMouseMove(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmGetMinMaxInfo(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmRefresh(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}
        virtual intx_t OnWmSysCommand(uint32_t uiMessage, uintx_t uiParam, intx_t iParam){ return 0;}

    private:
        std::shared_ptr<controls::Form> _form;
        uint32_t _style = 0;
        uint32_t _styleEx = 0;
        handle_t _handle = nullptr;

    public:
        static Window * fromHandle(handle_t handle);
    };
}
