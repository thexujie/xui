#pragma once

#include "ui/base/Button.h"
#include "ui/Container.h"
#include "ui/controls/Lable.h"
#include "ViewItem.h"

namespace ui
{
    enum class view_mode
    {
        details = 0,
        tile,
    };

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
        std::shared_ptr<ViewItem> itemAt(size_t index) const;
        std::shared_ptr<ViewItem> findItem(const core::pointf & pos) const;

        void setViewMode(view_mode m) { _mode = m; }
        view_mode viewMode() const { return _mode; }
        void setItemMargin(const core::vec4<core::dimenf> & spacing) { _item_margin = spacing; refresh(); }
        const core::vec4<core::dimenf> & itemMargin() const { return _item_margin; }
        void setItemPadding(const core::vec4<core::dimenf> & spacing) { _item_padding = spacing; refresh(); }
        const core::vec4<core::dimenf> & itemPadding() const { return _item_padding; }
        void setMarkedColor(const core::color & c) { _marked_color = c; }
        core::color markedColor() const { return _marked_color; }
        void setSelectedColor(const core::color & c) { _selected_color = c; }
        core::color selectedColor() const { return _selected_color; }
        void setIconSize(const core::dimen2f & s) { _icon_size = s; }
        const core::dimen2f & iconSize() const { return _icon_size; }
        void setTileSize(const core::dimen2f & s) { _tile_size = s; }
        const core::dimen2f & tileSize() const { return _tile_size; }
        void setContentSpacing(const core::dimenf & s) { _content_spacing = s; }
        const core::dimenf & contentSpacing() const { return _content_spacing; }

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

        view_mode _mode = view_mode::details;
        select_mode _select_mode = select_mode::full_row;
        core::attribute<core::vec4<core::dimenf>> _item_margin;
        core::attribute<core::vec4<core::dimenf>> _item_padding;
       core::color _marked_color = core::colors::Auto;
       core::color _selected_color = core::colors::Auto;
       core::dimen2f _icon_size = { 1_em, 1_em };
       core::dimen2f _tile_size = { 16_em, 3_em };
       core::dimenf _content_spacing = 0.2_em;

        std::shared_ptr<ViewItem> _marked_item;
        std::shared_ptr<ViewItem> _selected_item;
	};
}
