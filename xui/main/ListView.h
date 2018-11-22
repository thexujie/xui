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
    class ListViewItem : public ViewItem
    {
    public:
        ListViewItem(std::string text, std::shared_ptr<drawing::Image> icon, drawing::wrap_mode wrap_mode = drawing::wrap_mode::none) : _text(text), _icon(icon), _wrap_mode(wrap_mode) {}

        void setIconSize(const core::dimen2f & size) { _icon_size = size; }
        core::sizef prefferdSize(float32_t width) const;
        void paint(drawing::Graphics & graphics, const core::rectf & clip);

    private:
        std::shared_ptr<drawing::Image> _icon;
        std::string _text;
        drawing::wrap_mode _wrap_mode = drawing::wrap_mode::none;

        std::optional<core::dimen2f> _icon_size;
        core::dimenf _icon_text_spacing = 0.2_em;
    };

    enum listview_mode
    {
        details = 0,
        titled
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

	protected:
        core::dimen2f _icon_size = {1_em, 1_em};
	};
}
