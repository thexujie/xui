#pragma once

namespace win32
{
    class ImeContext : public ui::IImeContext
    {
    public:
        ImeContext(pointer_t hwnd);
        ~ImeContext();

        void setImeMode(ui::ime_mode mode);
        void setCompositionPos(core::pointf pos);
        void setCompositionFont(const drawing::font & font);

        void release();
    private:
		pointer_t _hwnd = nullptr;
		pointer_t _imc = nullptr;
		ui::ime_mode _mode = ui::ime_mode::disabled;
    };

    class CursorContext : public ui::ICursorContext
    {
    public:
        CursorContext(pointer_t hwnd);
        ~CursorContext();

        void setCursor(ui::cursor c) override;
        void resetCursor() override;

    public:
        void reset();
        void apply();
    private:
		pointer_t _hwnd = nullptr;
        void * _id = nullptr;
        void * _id_last = nullptr;
    };

}

