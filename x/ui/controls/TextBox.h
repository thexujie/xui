#pragma once
#include "ui/Control.h"

namespace ui::controls
{
    struct cursor
    {
        cursor() = default;
        cursor(size_t pos_) : pos(pos_) {}
        cursor(size_t pos_, bool far_) : pos(pos_), far(far_) {}

        union
        {
            size_t pos = drawing::cursor_pos_nopos;
            struct
            {
                size_t index : 25;
                size_t length : 6;
                size_t far : 1;
            };
        };

        size_t curr() const { return far ? index + length : index; }
        size_t end() const { return index + length; }
        cursor & operator = (size_t pos_)
        {
            pos = pos_;
            return *this;
        }

        explicit operator size_t ()
        {
            return index;
        }

        bool operator == (const cursor & c) const { return curr() == c.curr(); }
        bool operator != (const cursor & c) const { return curr() != c.curr(); }

        bool operator == (size_t pos_) const { return index == pos_; }
        bool operator != (size_t pos_) const { return index != pos_; }
        bool operator < (size_t pos_) const { return index < pos_; }
        bool operator <= (size_t pos_) const { return index <= pos_; }
        bool operator > (size_t pos_) const { return index > pos_; }
        bool operator >= (size_t pos_) const { return index >= pos_; }

        static inline size_t mask = size_t(-1) >> 1;
        static inline size_t npos = size_t(-1);
    };

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
        void onSizeChanged(const core::si32f & from, const core::si32f & to) override;
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
        void del();
        void insert(const char * text, size_t count);

    private:
        void _updateIme();
        void _confirmBlob() const;
        void _doshaper();

        const drawing::glyph & _lastGlyph(cursor pos) const;
        const drawing::glyph & _nextGlyph(cursor pos) const;

        cursor _makeCursor(size_t index, bool far);

    private:
        void _setCursorShown(bool vis);
        bool _cursorShown() const { return _cursor_shown; }

    private:
        std::string _text;
        mutable std::shared_ptr<drawing::TextBlob> _textblob;
        std::shared_ptr<core::property_animation> _cursor_anim;

        std::shared_ptr<ImeContext> _imecontext;

        std::atomic_bool _delay_shaper = false;
        shaper_flags _delay_shaper_flags = nullptr;
        std::shared_ptr<drawing::Shaper> _shaper;

        bool _cursor_shown = false;
        cursor _cursor_pos = 0;
        float32_t _scroll_pos = 0;
    };
}
