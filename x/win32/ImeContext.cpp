#include "stdafx.h"
#include "ImeContext.h"
#include "win32/windows.h"
#include <Imm.h>
#pragma comment(lib, "imm32.lib")

namespace win32
{
    ImeContext::ImeContext(handle_t hwnd) :_hwnd(hwnd)
    {
        _imc = ImmGetContext(HWND(_hwnd));
        ImmSetOpenStatus(HIMC(_imc), FALSE);
        ImmAssociateContext(HWND(_hwnd), NULL);
    }

    ImeContext::~ImeContext()
    {
        release();
    }

    void ImeContext::setImeMode(ui::ime_mode mode)
    {
        if (!_imc)
            _imc = ImmGetContext(HWND(_hwnd));

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
        default: ;
        }
    }

    void ImeContext::setCompositionPos(core::pt32f pos)
    {
        if(!_imc)
            _imc = ImmGetContext(HWND(_hwnd));

        COMPOSITIONFORM cpf = { 0 };
        cpf.dwStyle = CFS_FORCE_POSITION;
        cpf.ptCurrentPos.x = (int32_t)pos.x;
        cpf.ptCurrentPos.y = (int32_t)pos.y;

        ImmSetCompositionWindow(HIMC(_imc), &cpf);
    }

    void ImeContext::setCompositionFont(const drawing::font & font)
    {
        LOGFONTW lfont = {};
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
}
