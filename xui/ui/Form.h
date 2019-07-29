#pragma once
#include "Container.h"

namespace ui
{
    class RadioGroup;
    ////class Menu;

    class Form : public Container
    {
    public:
        Form(form_styles styles = nullptr, std::shared_ptr<Form> parentForm = nullptr);
        Form(core::vec2<core::dimenf> & size, form_styles styles = nullptr, std::shared_ptr<Form> parentForm = nullptr);
        ~Form();

        void setParentForm(std::shared_ptr<Form> parentForm);
        std::shared_ptr<Form> parentForm() { return _parentForm.lock(); }

        void setTitle(const std::u8string & text);
        const std::u8string & title() const { return _title; }
        void setStyleSheet(std::shared_ptr<component::StyleSheet> styleSheet) { _style_sheet = styleSheet; }
		void setStyles(form_styles styles) { if(styles != _styles) { auto old = _styles;  _styles = styles; onFormStylesChanged(old, _styles); } }
        form_styles styles() const { return _styles; }
        void setResizeBorders(const core::vec4<core::dimenf> & borders) { _resize_borders = borders; }
        const core::vec4<core::dimenf> & resizeBorders() const { return _resize_borders; }

        std::shared_ptr<ui::IWindow> window() const;
        pointer_t handle() const;
        const core::pointf & windowPos() const { return _window_pos; }
        //const core::pointf & windowSize() const { return _rect_window.size; }
        //const core::rectf & windowRect() const { return _rect_window; }
        void setWindowPos(const core::pointf & pos);
        void setWindowPos(const core::vec2<core::dimenf> & pos) { setWindowPos(calc(pos)); }
        void setWindowSize(const core::sizef & size);
        void setWindowSize(const core::vec2<core::dimenf> & size) { setWindowSize(calc(size)); }

        void setFormState(form_state fs);
        form_state formState() const { return _form_state; }

        void show(form_state fs = form_state::normalize);
        void hide() { show(form_state::hide); }
        void close();

        void centerScreen(int32_t screenIndex = 0);

    public:
        void notifyWindowMouse(const input_state & state, mouse_button button, mouse_action action);
        void notifyWindowKey(const input_state & state, keycode key, key_action action);
        void notifyWindowCharInput(char32_t ch);
		void notifyWindowCaptured(bool c);
        void notifyWindowFocused(const input_state & state, bool f);
        void notifyWindowShown(bool s);

    public:
        std::shared_ptr<component::StyleSheet> styleSheet() const override { return _style_sheet; }
        std::shared_ptr<Form> form() const override { return share_ref<Form>(); }
        float_t ratio() const override;
        void setEvent(scene_event evt);
        void invalidate(const core::rectf & rect) override;
        void animate() override;
        std::shared_ptr<Control> findChild(const core::pointf & pos, std::shared_ptr<Control> last = nullptr, findchild_flags flags = nullptr) const override;
        void onSizeChanged(const core::sizef & from, const core::sizef & to) override;

        void onPaint(drawing::Graphics & graphics, const core::rectf & clip) const override;

        virtual hittest_form hitTestForm(const core::pointf & pos) const;

		void onAction(action_t action) override;
	public:
		void onFormStylesChanged(form_styles from, form_styles to);
		void onFormStateChanged(form_state from, form_state to);

    public:
        std::shared_ptr<RadioGroup> radioGroup(const std::u8string & name);

        std::shared_ptr<ui::IImeContext> imeContext() const { if (!_window) return nullptr; return _window->imeContext(); }
        std::shared_ptr<ICursorContext> cursorContext() const { if (!_window) return nullptr; return _window->cursorContext(); }
        std::shared_ptr<drawing::GraphicsDevice> bitmap() const { return _draw_buffer; }

    private:
        void _updateMouseArea(const input_state & state, mouse_action action);
        void _render();
        void _animationTimerTick(core::timer & t, int64_t tick);

		std::shared_ptr<Control> _findChild(const input_state & state, std::shared_ptr<Control> last = nullptr, findchild_flags flags = nullptr);
		void _setCurrentControl(const input_state & state, std::shared_ptr<Control> control);
		void _setSelectedControl(const input_state & state, std::shared_ptr<Control> control);
		void _setActivedControl(const input_state & state, std::shared_ptr<Control> control);
		void _setFocusedControl(const input_state & state, std::shared_ptr<Control> control);
		void _setCapturedControl(const input_state & state, std::shared_ptr<Control> control);

    private:
        std::shared_ptr<ui::IWindow> _window;
        ////std::shared_ptr<ui::Menu> _menu;
        std::weak_ptr<Form> _parentForm;

        core::pointf _window_pos;
        core::attribute<core::vec4<core::dimenf>> _resize_borders;
        std::map<std::u8string, std::weak_ptr<RadioGroup>> _radio_groups;
        core::timer _animation_timer;
        std::shared_ptr<component::StyleSheet> _style_sheet;

        form_state _form_state = form_state::hide;
        form_styles _styles = nullptr;

        std::u8string _title;

        bool _invaliding = false;
        drawing::Region _invalid_region;
        core::color _color_default = core::colors::AliceBlue;

        std::shared_ptr<drawing::Surface> _draw_buffer;

		std::shared_ptr<Control> _current_control;
		std::shared_ptr<Control> _current_select;
		std::shared_ptr<Control> _current_active;
        std::shared_ptr<Control> _current_focus;
        std::shared_ptr<Control> _captured_control;

    public:
        core::event<void(const std::u8string & title)> titleChanged;
        core::event<void(form_styles from, form_styles to)> stylesChanged;
        core::event<void(const core::pointf & from, const core::pointf & to)> windowPosChanged;
        //core::event<void(const core::sizef & from, const core::sizef & to)> windowSizeChanged;
        //core::event<void(const core::rectf & from, const core::rectf & to)> windowRectChanged;
        core::event<void(form_state from, form_state to)> stateChanged;
        core::event<void(action_t, core::event_flags & flags)> titleAction;
        core::event<void()> closed;

        core::event<void(const core::recti & rect)> invalidated;
        core::event<void(const core::recti & rect)> rendered;
        core::event<void(bool capture)> captured;
        core::event<void(scene_event evt)> evented;
    };
}
