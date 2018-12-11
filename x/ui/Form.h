#pragma once
#include "component/Window.h"
#include "Container.h"

namespace ui
{
    class Form : public Container
    {
    public:
        Form(form_styles styles = nullptr);
        Form(core::vec2<core::dimenf> & size, form_styles styles = nullptr);
        ~Form();

        void setTitle(const std::string & text);
        const std::string & title() const { return _title; }

		void setStyles(form_styles styles) { if(styles != _styles) { auto old = _styles;  _styles = styles; onFormStylesChanged(old, _styles); } }
        form_styles styles() const { return _styles; }
        void setResizeBorders(const core::vec4<core::dimenf> & borders) { _resize_borders = borders; }
        const core::vec4<core::dimenf> & resizeBorders() const { return _resize_borders; }

        const core::pointf & windowPos() const { return _rect_window.pos; }
        //const core::pointf & windowSize() const { return _rect_window.size; }
        //const core::rectf & windowRect() const { return _rect_window; }
        void setWindowPos(const core::pointf & pos);
        void setWindowPos(const core::vec2<core::dimenf> & pos) { setWindowPos(calc(pos)); }
        void setWindowSize(const core::sizef & size);
        void setWindowSize(const core::vec2<core::dimenf> & size) { setWindowSize(calc(size)); }

        void setFormScene(std::shared_ptr<Scene> s);
        void setFormState(form_state fs);
        form_state formState() const { return _form_state; }


 		void show() { show(form_state::show); }
		void show(form_state fs);
        bool shown() const { return _form_state != form_state::hide; }
        void close();

        void centerScreen(int32_t screenIndex = 0);

    public:
        float_t ratio() const override;
        void invalidate(const core::rectf & rect) override;
        std::shared_ptr<Control> findChild(const core::pointf & pos, std::shared_ptr<Control> last = nullptr, findchild_flags flags = nullptr) const override;
        void onEnteringScene(std::shared_ptr<Scene> & scene) override;
        void onSizeChanged(const core::sizef & from, const core::sizef & to) override;

        void prepaint(drawing::Graphics & graphics, const core::rectf & clip) const override;

        virtual hittest_form hitTestForm(const core::pointf & pos) const;

	public:
		virtual void onFormStylesChanged(form_styles from, form_styles to);
		virtual void onFormStateChanged(form_state from, form_state to);
		virtual void onClose();
		virtual void onTitleAction(title_action action);

    public:
        core::event<void(const std::string & title)> titleChanged;
        core::event<void(form_styles from, form_styles to)> stylesChanged;
        core::event<void(const core::pointf & from, const core::pointf & to)> windowPosChanged;
        //core::event<void(const core::sizef & from, const core::sizef & to)> windowSizeChanged;
        //core::event<void(const core::rectf & from, const core::rectf & to)> windowRectChanged;
        core::event<void(form_state from, form_state to)> stateChanged;
        core::event<void()> closed;

    private:
        std::shared_ptr<Scene> _form_scene;
        std::shared_ptr<component::Window> _window;
        core::rectf _rect_window;
        core::attribute<core::vec4<core::dimenf>> _resize_borders;

        form_state _form_state = form_state::hide;
        form_styles _styles = nullptr;

        std::string _title;
    };
}
