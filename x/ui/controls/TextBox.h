#pragma once
#include "ui/Control.h"

namespace ui::controls
{
    class TextBox : public Control
    {
    public:
        TextBox();
        ~TextBox();

        static void propertyTableCallback(core::property_table & properties);
        void propertyTable(core::property_table & properties) override;

        void setText(const std::string & text);
        const std::string & text() const { return _text; }

        core::si32f contentSize() const override;
        std::string styleName() const override;

        void render(drawing::Graphics & graphics, const drawing::Region & region) const override;
    public:
        void onMouseEnter(const input_state & state) override;
        void onMouseMove(const input_state & state) override;
        void onMouseLeave(const input_state & state) override;
        void onMouseDown(const input_state & state, ui::mouse_button button) override;
        void onMouseUp(const input_state & state, ui::mouse_button button) override;
        void onKeyDown(const input_state & state, keycode key) override;
        void onFocus(std::shared_ptr<ImeContext> imecontext) override;
        void onBlur() override;
        void onChar(char32_t ch) override;

    public:
        void reshaper();
        void caretLeft();
        void caretRight();
        void backSpace();

    private:
        void _updateIme();
        void _confirmBlob() const;
        void _doshaper();

    private:
        void _setCursorShown(bool vis);
        bool _cursorShown() const { return _cursor_shown; }

    private:
        std::string _text;
        mutable std::shared_ptr<drawing::TextBlob> _textblob;
        std::shared_ptr<core::property_animation> _cursor_anim;

        std::shared_ptr<ImeContext> _imecontext;

        bool _delay_shaper = false;
        mutable std::shared_ptr<drawing::Shaper> _shaper;

        bool _cursor_shown = false;
        size_t _cursor_pos = core::npos;
    };
}
