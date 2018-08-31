#pragma once

#include "ui/Control.h"
#include "win32/win32.h"

namespace win32
{
    class ImeContext : public ui::ImeContext
    {
    public:
        ImeContext(handle_t hwnd);
        ~ImeContext();

        void setImeMode(ui::ime_mode mode);
        void setCompositionPos(core::pt32f pos);
        void setCompositionFont(const graphics::font & font);

        void release();
    private:
        handle_t _hwnd = nullptr;
        handle_t _imc = nullptr;
    };
}

