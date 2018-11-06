#include "stdafx.h"
#include "View.h"

namespace ui
{
    const core::vec4<core::dimensionf> & ItemData::margin() const
    {
        auto v = view();
        if (!v)
            return _margin;

        if (_margin)
            return _margin;
        else
            return v->itemMargin();
    }

    const core::vec4<core::dimensionf> & ItemData::padding() const
    {
        auto v = view();
        if (!v)
            return _padding;

        if (_padding)
            return _padding;
        else
            return v->itemPadding();
    }

    core::rectf ItemData::contentBox() const
    {
        auto v = view();
        if (!v)
            return {};

        auto p = padding();
        return core::rectf(_box.pos + v->calc(p).bleftTop(), _size - v->calc(p).bsize());
    }

    View::View()
	{
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

    size_t View::addItem(std::shared_ptr<ItemData> item)
    {
        return insertItem(_items.size(), item);
    }

    size_t View::insertItem(size_t index, std::shared_ptr<ItemData> item)
    {
        auto iter = _items.begin() + index;
        item->enterView(share_ref<View>());
        _items.insert(iter, item);
        relayout();
        return index;
    }

    size_t View::eraseItem(size_t index)
    {
        auto item = _items[index];
        auto iter = _items.begin() + index;
        _items.erase(iter);
        item->leaveView();
        relayout();
        return index;
    }

    std::shared_ptr<ItemData> View::findItem(const core::pointf & pos) const
    {
        _confirmLayout();
        for (size_t index = 0; index != _items.size(); ++index)
        {
            auto & item = _items[index];
            if(_select_mode == select_mode::full_row)
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
        return nullptr;
    }

    void View::_setMarkedItem(std::shared_ptr<ItemData> item)
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

    void View::_setSelectedItem(std::shared_ptr<ItemData> item)
    {
        if (item != _selected_item)
        {
            if (_selected_item)
            {
                _selected_item->setFlag(item_flag::selected, false);
                if (_marked_color)
                    repaint(_marked_item->box());
            }
            _selected_item = item;
            if (_selected_item)
            {
                _selected_item->setFlag(item_flag::selected, true);
                if (_marked_color)
                    repaint(_marked_item->box());
            }
        }
    }
}

