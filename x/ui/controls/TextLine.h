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

    class TextLine : public Control
    {
    public:
        TextLine();
        ~TextLine();

        static void propertyTableCallback(core::property_table & properties);
        void propertyTable(core::property_table & properties) override;

        void setText(const std::string & text);
        const std::string & text() const { return _text; }

        core::sizef contentSize() const override;
        std::string styleName() const override;

        void draw(drawing::Graphics & graphics, const core::rectf & clip) const override;
    public:
        void onSizeChanged(const core::sizef & from, const core::sizef & to) override;
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
        void _doshaper();
        void _docaret();

    private:
        void _setCursorShown(bool vis);
        bool _cursorShown() const { return _cursor_shown; }

    private:
        std::string _text;
        mutable std::shared_ptr<drawing::TextBlob> _text_blob;
        std::shared_ptr<core::property_animation> _cursor_anim;

        std::shared_ptr<ImeContext> _ime_context;

        bool _delay_shaper = false;
        shaper_flags _delay_shaper_flags = nullptr;
        std::shared_ptr<drawing::TextClusterizer> _clusterizer;

        bool _cursor_shown = false;
        size_t _cursor_gindex = 0;
        size_t _cursor_pos = 0;
        bool _cursor_far = false;

        float32_t _scroll_pos = 0;
        size_t _cursor_pos_selected = core::npos;
    };
}
