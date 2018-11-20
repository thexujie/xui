#pragma once

#include "ui/base/Button.h"
#include "ui/Container.h"
#include "ui/controls/Text.h"
#include "View.h"

namespace ui 
{
	class Form;
}

namespace ui
{
    class DataTemplate
    {
        
    };

    class ControlTemplate
    {
        
    };

    class ListViewItem : public ViewItem
    {
    public:
        ListViewItem(std::string text, drawing::wrap_mode wrap_mode = drawing::wrap_mode::none) : _text(text), _wrap_mode(wrap_mode) {}
        core::sizef prefferdSize(float32_t width) const;
        void paint(drawing::Graphics & graphics, const core::rectf & clip);

    private:
        std::string _text;
        drawing::wrap_mode _wrap_mode = drawing::wrap_mode::none;
    };

	class ListView : public ui::View
	{
	public:
        ListView();
		~ListView();

		static void propertyTableCallback(core::property_table & properties);
		void propertyTable(core::property_table & properties) override;

		std::string styleName() const override;

        core::sizef contentSize() const override;
        void update() override;
		void paint(drawing::Graphics & graphics, const core::rectf & clip) const override;
	};
}
