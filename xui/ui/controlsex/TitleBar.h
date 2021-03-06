#pragma once

#include "ui/base/Button.h"
#include "ui/Container.h"
#include "ui/controls/Lable.h"

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
		TitleButton(const std::u8string & text);
		~TitleButton();

		static void propertyTableCallback(core::property_table & properties);
		void propertyTable(core::property_table & properties) override;

		void setPath(drawing::path_source & source);

		std::u8string styleName() const override;

		void onEnterScene() override;
		void onLeaveScene() override;
		void update() override;
		void paint(drawing::Graphics & graphics, const core::rectf & clip) const override;

	private:
		void _setShapeColor(core::color color);

	private:
		drawing::path_source _path;
		std::shared_ptr<drawing::Path> _path_object;
		
		core::color _shape_color = core::colors::Black;
	};

    class TitleBar : public ui::Container
    {
    public:
        TitleBar(std::shared_ptr< ui::Form> form) : _form(form)
        {
            setLayoutDirection(core::align::right);
        }

		void onEnterScene() override;
		void onLeaveScene() override;
		std::shared_ptr< ui::Form> form() { return _form.lock(); }

		void onFormStylesChanged(ui::form_styles, ui::form_styles styles)
        {
            refresh();
        }

		void onFormStateChanged(ui::form_state, ui::form_state state)
		{
			refresh();
		}

        void onFormTitleChanged(const std::u8string &)
        {
            refresh();
        }

		void onAction(action_t action) override;

    private:
		void update() override;

	private:
		std::weak_ptr< ui::Form> _form;

		std::shared_ptr<TitleButton> _close;
		std::shared_ptr<TitleButton> _maximize;
        std::shared_ptr<TitleButton> _minimize;
        std::shared_ptr<controls::Lable> _title;
    };
}
