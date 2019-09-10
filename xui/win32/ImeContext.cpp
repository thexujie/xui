#include "stdafx.h"
#include "ImeContext.h"
#include <Imm.h>
#pragma comment(lib, "imm32.lib")

namespace platform::win32
{
    ImeContext::ImeContext(pointer_t hwnd) :_hwnd(hwnd)
    {
        //_imc = ImmGetContext(HWND(_hwnd));
        //ImmSetOpenStatus(HIMC(_imc), FALSE);
        //ImmAssociateContext(HWND(_hwnd), NULL);
    }

    ImeContext::~ImeContext()
    {
        release();
    }

    void ImeContext::setImeMode(ui::ime_mode mode)
    {
        if (!_imc)
            _imc = ImmGetContext(HWND(_hwnd));

		if(mode == _mode)
			return;
		_mode = mode;

        switch(mode)
        {
        case ui::ime_mode::disabled:
            ImmAssociateContext(HWND(_hwnd), NULL);
            break;
        case ui::ime_mode::on:
            ImmAssociateContext(HWND(_hwnd), HIMC(_imc));
            ImmSetOpenStatus(HIMC(_imc), TRUE);
            break;
        case ui::ime_mode::off:
            ImmAssociateContext(HWND(_hwnd), HIMC(_imc));
            ImmSetOpenStatus(HIMC(_imc), FALSE);
            break;
        default: 
        	break;
        }
    }

    void ImeContext::setCompositionPos(core::pointf pos)
    {
        if(!_imc)
            _imc = ImmGetContext(HWND(_hwnd));

        COMPOSITIONFORM cpf = { 0 };
        cpf.dwStyle = CFS_POINT;
        cpf.ptCurrentPos.x = (int32_t)pos.x;
        cpf.ptCurrentPos.y = (int32_t)pos.y;

        ImmSetCompositionWindow(HIMC(_imc), &cpf);
    }

    void ImeContext::setCompositionFont(const drawing::font & font)
    {
        LOGFONTW lfont = {};
		ImmGetCompositionFontW(HIMC(_imc), &lfont);
        core::textcpy(lfont.lfFaceName, std::size(lfont.lfFaceName), core::u8str_wstr(font.family).c_str());
        lfont.lfHeight = -font.size;
        ImmSetCompositionFontW(HIMC(_imc), &lfont);
    }

    void ImeContext::release()
    {
        if (_imc)
        {
            ImmAssociateContext(HWND(_hwnd), HIMC(_imc));
            ImmReleaseContext(HWND(_hwnd), HIMC(_imc));
            _imc = nullptr;
        }
    }


    CursorContext::CursorContext(pointer_t hwnd)
    {
        
    }

    CursorContext::~CursorContext()
    {
        
    }

    static LPCTSTR win_cursor_id(ui::cursor c)
    {
        switch(c)
        {
        case ui::cursor::unknown: return IDC_ARROW;
        case ui::cursor::normal: return IDC_ARROW;
        case ui::cursor::cross: return IDC_CROSS;
        case ui::cursor::hand: return IDC_HAND;
        case ui::cursor::help: return IDC_HELP;
        case ui::cursor::wait: return IDC_WAIT;
        case ui::cursor::ibeam: return IDC_IBEAM;
        case ui::cursor::notaviliable: return IDC_NO;
        case ui::cursor::resizelr: return IDC_SIZEWE;
        case ui::cursor::resizetb: return IDC_SIZENS;
        case ui::cursor::resizeltrb: return IDC_SIZENESW;
        case ui::cursor::resizelbrt: return IDC_SIZENWSE;
        case ui::cursor::resizeall: return IDC_SIZEALL;
        default: return NULL;
        }
    }

    void CursorContext::setCursor(ui::cursor c)
    {
        _id_last = _id ? _id : IDC_ARROW;
        auto id = win_cursor_id(c);
        if (!id || id == _id)
            return;

        _id = (void *)id;
        ::SetCursor(::LoadCursor(NULL, id));
    }

    void CursorContext::resetCursor()
    {
        if(_id_last)
        {
            ::SetCursor(::LoadCursor(NULL, (LPCTSTR)_id_last));
            _id = _id_last;
            _id_last = nullptr;
        }
    }

    void CursorContext::reset()
    {
        _id = nullptr;
        _id_last = nullptr;
    }

    void CursorContext::apply()
    {
        if (_id)
            ::SetCursor(::LoadCursor(NULL, (LPCTSTR)_id));
    }
}
