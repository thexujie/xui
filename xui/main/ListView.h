#pragma once

#include "ui/base/Button.h"
#include "ui/Container.h"
#include "ui/controls/Lable.h"
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
        void paint(drawing::Graphics & graphics, const core::rectf & clip) const;

        void addProperty(std::string p) { _propertiess.push_back(p); }
    private:
        std::shared_ptr<drawing::Image> _icon;
        std::string _text;
        drawing::wrap_mode _wrap_mode = drawing::wrap_mode::none;

        std::optional<core::dimen2f> _icon_size;
        core::dimenf _icon_text_spacing = 0.2_em;
        std::vector<std::string> _propertiess;
    };

	class ListView : public ui::View
	{
	public:
        ListView();
		~ListView();

		static void propertyTableCallback(core::property_table & properties);
		void propertyTable(core::property_table & properties) override;

		std::string styleName() const override;

        void update() override;
		void paint(drawing::Graphics & graphics, const core::rectf & clip) const override;
        void onScrollPosChanged(const core::vec2f & from, const core::vec2f & to);

		void onAction(action_t action) override;
		void onPopupMenu(const input_state & state, IMenuPresenter & presenter) override;

	public:
        void view() override;
	};
}
