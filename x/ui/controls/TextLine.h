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
        const std::string & text() const { return _text.text(); }

        core::sizef contentSize() const override;
        std::string styleName() const override;

        void update();
        void paint(drawing::Graphics & graphics, const core::rectf & clip) const override;
    public:
        void onSizeChanged(const core::sizef & from, const core::sizef & to) override;
        void onHover(const input_state & state) override;
        void onActiveIn(const input_state & state) override;
        void onActiveOut(const input_state & state) override;
        void onKeyDown(const input_state & state, keycode key) override;
        void onFocusIn(const input_state & state) override;
        void onFocusOut(const input_state & state) override;
        void onChar(char32_t ch) override;
		void onAction(action_t action) override;
		void onPopupMenu(const input_state & state, IMenuPresenter & presenter) override;

    public:
        void caretLeft();
        void caretRight();
        void backSpace();
        void del();
        void insert(const char * text, size_t count);
        void updateIme();

    private:
        void _docaret();
        void _setCursorShown(bool vis);
        bool _cursorShown() const { return _cursor_shown; }
		void _setSelectColor(const core::color & color) { _color_select = color; repaint(); }

    private:
        drawing::Text _text;

		core::color _color_select = core::colors::AliceBlue;

        std::shared_ptr<core::property_animation> _cursor_anim;

        bool _cursor_shown = false;
        size_t _cursor_gindex = 0;
        size_t _cursor_pos = 0;
        bool _cursor_far = false;

        float32_t _scroll_pos = 0;
        size_t _cursor_pos_selected = core::npos;
    };
}
