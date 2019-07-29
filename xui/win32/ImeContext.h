#pragma once

#include "ui/Control.h"
#include "win32/win32.h"

namespace win32
{
    class ImeContext : public ui::IImeContext
    {
    public:
        ImeContext(handle_t hwnd);
        ~ImeContext();

        void setImeMode(ui::ime_mode mode);
        void setCompositionPos(core::pointf pos);
        void setCompositionFont(const drawing::font & font);

        void release();
    private:
        handle_t _hwnd = nullptr;
        handle_t _imc = nullptr;
		ui::ime_mode _mode = ui::ime_mode::disabled;
    };

    class CursorContext : public ui::ICursorContext
    {
    public:
        CursorContext(handle_t hwnd);
        ~CursorContext();

        void setCursor(ui::cursor c) override;
        void resetCursor() override;

    public:
        void reset();
        void apply();
    private:
        handle_t _hwnd = nullptr;
        void * _id = nullptr;
        void * _id_last = nullptr;
    };

}

