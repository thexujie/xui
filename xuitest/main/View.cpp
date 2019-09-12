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
        properties[u8"marked-color"] = make_accessor(&View::setMarkedColor, &View::markedColor);
        properties[u8"selected-color"] = make_accessor(&View::setSelectedColor, &View::selectedColor);
        properties[u8"item-margin"] = make_accessor(&View::setItemMargin, &View::itemMargin);
        properties[u8"item-padding"] = make_accessor(&View::setItemPadding, &View::itemPadding);
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
        valid();
        if (_mode == view_mode::details)
        {
            for (size_t index = 0; index != _items.size(); ++index)
            {
                auto & item = _items[index];
                if (_select_mode == select_mode::full_row)
                {
                    auto rect = item->marginBox();
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
                if (item->marginBox().contains(pos))
                    return item;
            }
        }
        return nullptr;
    }

	void View::setViewMode(view_mode m)
    {
	    if(_mode != m)
	    {
			_mode = m;
			restyle();
			refresh();
			repaint();
	    }
    }

    void View::review()
    {
        if (!_delay_view)
        {
            _delay_view = true;
            invoke([this]()
            {
                _delay_view = false;
                view();
            });
        }
    }

    void View::valid() const
    {
        ui::Container::valid();
        if (_delay_view)
        {
            const_cast<View *>(this)->_delay_view = false;
            const_cast<View *>(this)->view();
        }
    }

    void View::onRectChanged(const core::rectf & from, const core::rectf & to)
    {
        refresh();
        ui::Container::onRectChanged(from, to);
    }

    void View::onHover(const ui::input_state & state)
    {
        auto item = findItem(state.pos());
        _setMarkedItem(item);
        ui::Container::onHover(state);
    }

    void View::onHoverOut(const ui::input_state & state)
    {
        _setMarkedItem(nullptr);
        ui::Container::onHoverOut(state);
    }

    void View::onActiveIn(const ui::input_state & state)
    {
        _setSelectedItem(_marked_item);
        ui::Container::onActiveIn(state);
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

