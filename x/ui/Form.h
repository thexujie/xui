#pragma once
#include "component/Window.h"
#include "Container.h"

namespace ui
{
    class Form : public Container
    {
    public:
        Form(form_styles styles = nullptr);
        Form(core::vec2<core::dimensionf> & size, form_styles styles = nullptr);
        ~Form();

		void setStyles(form_styles styles) { if(styles != _styles) { auto old = _styles;  _styles = styles; onFormStylesChanged(old, _styles); } }
        form_styles styles() const { return _styles; }
        void setResizeBorders(const core::vec4<core::dimensionf> & borders) { _resize_borders = borders; }
        const core::vec4<core::dimensionf> & resizeBorders() const { return _resize_borders; }

        const core::pt32f & windowPos() const { return _rect_window.pos; }
        //const core::pt32f & windowSize() const { return _rect_window.size; }
        //const core::rc32f & windowRect() const { return _rect_window; }
        void setWindowPos(const core::pt32f & pos);
        void setWindowPos(const core::vec2<core::dimensionf> & pos) { setWindowPos(calc(pos)); }
        void setWindowSize(const core::si32f & size);
        void setWindowSize(const core::vec2<core::dimensionf> & size) { setWindowSize(calc(size)); }

        std::shared_ptr<Scene> formScene() const;

        void setFormState(form_state fs);
        form_state formState() const { return _form_state; }

		void show() { show(form_state::show); }
		void show(form_state fs);
        bool shown() const { return _form_state != form_state::hide; }
        void close();

        void centerScreen(int32_t screenIndex = 0);

    public:
        using Container::invalidate;
        void invalidate(const core::rc32f & rect) override;
        std::shared_ptr<Control> findChild(const core::pt32f & pos, std::shared_ptr<Control> last = nullptr, findchild_flags flags = nullptr) const override;
        void onEnteringScene(std::shared_ptr<Scene> & scene) override;
        void onSizeChanged(const core::si32f & from, const core::si32f & to) override;

        void ondraw(drawing::Graphics & graphics, const core::rc32f & clip) const override;

        virtual hittest_form hitTestForm(const core::pt32f & pos) const;

	public:
		virtual void onFormStylesChanged(form_styles from, form_styles to);
		virtual void onFormStateChanged(form_state from, form_state to);
		virtual void onClose();
		virtual void onTitleAction(title_action action);

    public:
        core::event<void(form_styles from, form_styles to)> stylesChanged;
        core::event<void(const core::pt32f & from, const core::pt32f & to)> windowPosChanged;
        //core::event<void(const core::si32f & from, const core::si32f & to)> windowSizeChanged;
        //core::event<void(const core::rc32f & from, const core::rc32f & to)> windowRectChanged;
        core::event<void(form_state from, form_state to)> stateChanged;
        core::event<void()> closed;

    private:
        std::shared_ptr<Scene> _form_scene;
        std::shared_ptr<component::Window> _window;
        core::rc32f _rect_window;
        core::attribute<core::vec4<core::dimensionf>> _resize_borders;

        form_state _form_state = form_state::hide;
        form_styles _styles = nullptr;
    };
}
