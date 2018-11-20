#pragma once

#include "ui/base/Button.h"
#include "ui/Container.h"
#include "ui/controls/Text.h"
#include "ViewItem.h"

namespace ui
{
	class View : public ui::Container
	{
	public:
        View();
		~View();

		static void propertyTableCallback(core::property_table & properties);
		void propertyTable(core::property_table & properties) override;

	public:
        size_t addItem(std::shared_ptr<ViewItem> item);
        size_t insertItem(size_t index, std::shared_ptr<ViewItem> item);
        size_t eraseItem(size_t index);
        std::shared_ptr<ViewItem> findItem(const core::pointf & pos) const;

        void setItemMargin(const core::vec4<core::dimensionf> & spacing) { _item_margin = spacing; refresh(); }
        const core::vec4<core::dimensionf> & itemMargin() const { return _item_margin; }
        void setItemPadding(const core::vec4<core::dimensionf> & spacing) { _item_padding = spacing; refresh(); }
        const core::vec4<core::dimensionf> & itemPadding() const { return _item_padding; }
        void setMarkedColor(const core::color & c) { _marked_color = c; }
        core::color markedColor() const { return _marked_color; }
        void setSelectedColor(const core::color & c) { _selected_color = c; }
        core::color selectedColor() const { return _selected_color; }

    public:
        void onRectChanged(const core::rectf & from, const core::rectf & to) override;
        void onMouseMove(const ui::input_state & state) override;
        void onMouseLeave(const ui::input_state & state) override;
        void onMouseDown(const ui::input_state & state, ui::mouse_button button) override;
        void onMouseClick(const ui::input_state & state, ui::mouse_button button) override;

    protected:
        void _setMarkedItem(std::shared_ptr<ViewItem> item);
        void _setSelectedItem(std::shared_ptr<ViewItem> item);

	protected:
        std::vector<std::shared_ptr<ViewItem>> _items;
        std::map<size_t, std::list<std::shared_ptr<Control>>> _controls;

        std::shared_ptr<ViewItem> _marked_item;
        std::shared_ptr<ViewItem> _selected_item;
        select_mode _select_mode = select_mode::full_row;
        core::attribute<core::vec4<core::dimensionf>> _item_margin;
        core::attribute<core::vec4<core::dimensionf>> _item_padding;

       core::color _marked_color = core::colors::Auto;
       core::color _selected_color = core::colors::Auto;
	};
}
