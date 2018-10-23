#pragma once

#include "ui/base/Button.h"
#include "ui/Container.h"

namespace ui 
{
	class Form;
}

namespace ui::controlsex
{
	class TitleButton : public ui::base::Button
	{
	public:
		TitleButton();
		TitleButton(std::string text);
		~TitleButton();

		static void propertyTableCallback(core::property_table & properties);
		void propertyTable(core::property_table & properties) override;

		void setPath(std::shared_ptr<drawing::Path> path);
		void setTitleAction(ui::title_action a) { _action = a; }
		ui::title_action titleAction() const { return _action; }

		std::string styleName() const override;

		void draw(drawing::Graphics & graphics, const core::rc32f & clip) const override;

	public:
		void onMouseClick(const ui::input_state & state, ui::mouse_button button) override;

	private:
		void _setShapeColor(core::color color);

	private:
		std::shared_ptr<drawing::Path> _path;
		ui::title_action _action = ui::title_action::none;
		core::color _shape_color = core::colors::Black;

	public:
		core::event<void(ui::title_action)> action;
	};

    class TitleBar : public ui::Container
    {
    public:
        TitleBar(std::shared_ptr< ui::Form> form) : _form(form)
        {
            setLayoutDirection(core::align::right);
        }

		void onEnterScene(std::shared_ptr<ui::Scene> & scene);
		void onLeaveScene();
		std::shared_ptr< ui::Form> form() { return _form.lock(); }

		void onFormStylesChanged(ui::form_styles, ui::form_styles styles)
        {
			_updateButtons();
        }

		void onFormStateChanged(ui::form_state, ui::form_state state)
		{
			_updateButtons();
		}

		void onAction(ui::title_action action);

    private:
		void _updateButtons();

	private:
		std::weak_ptr< ui::Form> _form;

		std::shared_ptr<TitleButton> _close;
		std::shared_ptr<TitleButton> _maximize;
		std::shared_ptr<TitleButton> _minimize;
    };
}
