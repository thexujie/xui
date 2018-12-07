#include "stdafx.h"
#include "View.h"

namespace ui
{
    View::View()
	{
        _scroll_controls = false;
        _layout_direction = core::align::none;
	}

    View::~View()
	{

	}

	void View::propertyTableCallback(core::property_table & properties)
	{
		Control::propertyTableCallback(properties);
        properties["marked-color"] = make_accessor(&View::setMarkedColor, &View::markedColor);
        properties["selected-color"] = make_accessor(&View::setSelectedColor, &View::selectedColor);
        properties["item-margin"] = make_accessor(&View::setItemMargin, &View::itemMargin);
        properties["item-padding"] = make_accessor(&View::setItemPadding, &View::itemPadding);
	}

	void View::propertyTable(core::property_table & properties)
	{
        View::propertyTableCallback(properties);
	}

    size_t View::addItem(std::shared_ptr<ViewItem> item)
    {
        return insertItem(_items.size(), item);
    }

    size_t View::insertItem(size_t index, std::shared_ptr<ViewItem> item)
    {
        auto iter = _items.begin() + index;
        item->enterView(share_ref<View>());
        _items.insert(iter, item);
        refresh();
        return index;
    }

    size_t View::eraseItem(size_t index)
    {
        auto item = _items[index];
        auto iter = _items.begin() + index;
        _items.erase(iter);
        item->leaveView();
        refresh();
        return index;
    }

    std::shared_ptr<ViewItem> View::itemAt(size_t index) const
    {
        return _items[index];
    }

    std::shared_ptr<ViewItem> View::findItem(const core::pointf & pos) const
    {
        _confirmLayout();
        if (_mode == view_mode::details)
        {
            for (size_t index = 0; index != _items.size(); ++index)
            {
                auto & item = _items[index];
                if (_select_mode == select_mode::full_row)
                {
                    auto & rect = item->box();
                    if (rect.y <= pos.y && pos.y < rect.bottom())
                        return item;
                }
                else
                {
                    if (item->rect().contains(pos))
                        return item;
                }
            }
        }
        else if (_mode == view_mode::tile)
        {
            for (size_t index = 0; index != _items.size(); ++index)
            {
                auto & item = _items[index];
                if (item->rect().contains(pos))
                    return item;
            }
        }
        return nullptr;
    }

    void View::onRectChanged(const core::rectf & from, const core::rectf & to)
    {
        refresh();
        ui::Container::onRectChanged(from, to);
    }

    void View::onMouseMove(const ui::input_state & state)
    {
        auto item = findItem(state.pos());
        _setMarkedItem(item);
        ui::Container::onMouseMove(state);
    }

    void View::onMouseLeave(const ui::input_state & state)
    {
        _setMarkedItem(nullptr);
        ui::Container::onMouseLeave(state);
    }

    void View::onMouseDown(const ui::input_state & state, ui::mouse_button button)
    {
        _setSelectedItem(_marked_item);
        ui::Container::onMouseDown(state, button);
    }

    void View::onMouseClick(const ui::input_state & state, ui::mouse_button button)
    {
        ui::Container::onMouseClick(state, button);
    }

    void View::_setMarkedItem(std::shared_ptr<ViewItem> item)
    {
        if (item != _marked_item)
        {
            if (_marked_item)
            {
                _marked_item->setFlag(item_flag::marked, false);
                if (_marked_color)
                    repaint(_marked_item->box());
            }
            _marked_item = item;
            if (_marked_item)
            {
                _marked_item->setFlag(item_flag::marked, true);
                if (_marked_color)
                    repaint(_marked_item->box());
            }
        }
    }

    void View::_setSelectedItem(std::shared_ptr<ViewItem> item)
    {
        if (item != _selected_item)
        {
            if (_selected_item)
            {
                _selected_item->setFlag(item_flag::selected, false);
                if (_marked_color)
                    repaint(_selected_item->box());
            }
            _selected_item = item;
            if (_selected_item)
            {
                _selected_item->setFlag(item_flag::selected, true);
                if (_marked_color)
                    repaint(_selected_item->box());
            }
        }
    }
}

