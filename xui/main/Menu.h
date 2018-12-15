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
	class Menu : public ui::Form, public IMenuPresenter
	{
	public:
        Menu();
        Menu(form_styles styles, std::shared_ptr<Form> parentForm);
		~Menu();

		static void propertyTableCallback(core::property_table & properties);
		void propertyTable(core::property_table & properties) override;

		std::string styleName() const override;

        void update() override;
        core::sizef contentSize() const override;
		void paint(drawing::Graphics & graphics, const core::rectf & clip) const override;

        void onScrollPosChanged(const core::vec2f & from, const core::vec2f & to);

        void onHover(const ui::input_state & state) override;
		void onHoverOut(const input_state & state) override;
		void onActive(const input_state & state) override;

	public:
		void clear() { _items.clear(); }
		size_t appendItem(std::shared_ptr<MenuItem> item);
		size_t appendItems(std::initializer_list<std::shared_ptr<MenuItem>> && items);
		size_t insertItem(size_t index, std::shared_ptr<MenuItem> item);
        size_t eraseItem(size_t index);
        std::shared_ptr<MenuItem> findItem(const core::pointf & pos) const;
		size_t itemCount() const { return _items.size(); }

        void setItemMargin(const core::vec4<core::dimenf> & spacing) { _item_margin = spacing; refresh(); }
        const core::vec4<core::dimenf> & itemMargin() const { return _item_margin; }
        void setItemPadding(const core::vec4<core::dimenf> & spacing) { _item_padding = spacing; refresh(); }
        const core::vec4<core::dimenf> & itemPadding() const { return _item_padding; }
        void setMarkedColor(const core::color & c) { _marked_color = c; }
        core::color markedColor() const { return _marked_color; }
        void setSelectedColor(const core::color & c) { _selected_color = c; }
        core::color selectedColor() const { return _selected_color; }
        void setTextShortcutSpacing(const core::dimenf & spacing) { _text_shortcut_spacing = spacing; }
        const core::dimenf & textShortcutSpacing() const { return _text_shortcut_spacing; }

    protected:
        void _setMarkedItem(std::shared_ptr<MenuItem> item);

	private:
        core::dimen2f _icon_size = { 1_em, 1_em };
        core::dimen4f _item_margin = { 0.1_em, 0.1_em, 0.1_em, 0.1_em };
        core::dimen4f _item_padding = { 0.2_em, 0.1_em, 0.5_em, 0.2_em };
        core::dimenf _spliter_size = 1_px;
        core::dimenf _icon_text_spacing = 1.5_em;
        core::dimenf _text_shortcut_spacing = 1.5_em;
        core::dimenf _shortcut_arrow_spacing = 0.5_em;
        std::vector<std::shared_ptr<MenuItem>> _items;

		core::color _item_color = core::colors::Black;
		core::color _item_color_disabled = core::colors::Gray;
		core::color _spliter_color = core::colors::Gray;
        core::color _marked_color = core::colors::AliceBlue;
		core::color _selected_color = core::colors::Blue;

        std::shared_ptr<MenuItem> _marked_item;

	public:
		core::event<void(action_t action)> active;
	};
}
