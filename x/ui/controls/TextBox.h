#pragma once
#include "ui/Control.h"

namespace ui::controls
{
    enum class shaper_flag
    {
        none = 0x0000,
        shaper = 0x0001,
        caret = 0x0002,
    };
    template<> struct enable_bitmasks<shaper_flag> { static const bool enable = true;};
    typedef core::bitflag<shaper_flag> shaper_flags;

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
        void reshaper(shaper_flags flags = nullptr);
        void caretLeft();
        void caretRight();
        void backSpace();
        void insert(size_t offset, const char * text, size_t count);

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
        shaper_flags _delay_shaper_flags = nullptr;
        std::shared_ptr<drawing::Shaper> _shaper;

        bool _cursor_shown = false;
        size_t _cursor_pos = core::npos;
    };
}
