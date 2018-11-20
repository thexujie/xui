#include "stdafx.h"
#include "View.h"

namespace ui
{
    const core::vec4<core::dimensionf> & ViewItem::margin() const
    {
        auto v = view();
        if (!v)
            return _margin;

        if (_margin)
            return _margin;
        else
            return v->itemMargin();
    }

    const core::vec4<core::dimensionf> & ViewItem::padding() const
    {
        auto v = view();
        if (!v)
            return _padding;

        if (_padding)
            return _padding;
        else
            return v->itemPadding();
    }

    core::rectf ViewItem::contentBox() const
    {
        auto v = view();
        if (!v)
            return {};

        auto p = padding();
        return core::rectf(_box.pos + v->calc(p).bleftTop(), _size - v->calc(p).bsize());
    }

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

    std::shared_ptr<ViewItem> View::findItem(const core::pointf & pos) const
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

    void View::_viewItem(std::shared_ptr<ViewItem> item)
    {
        if (item->frame())
            return;

        std::shared_ptr<ViewFrame> frame;
        size_t hash = typeid(*item).hash_code();
        auto & cache = _template_caches[hash];
        if(!cache.empty())
        {
            frame = cache.front();
            cache.pop_front();
        }
        else
        {
            auto iter = _template_creator.find(hash);
            if(iter != _template_creator.end())
                frame = iter->second();
        }

        if (!frame)
            return;

        item->bind(frame);
    }

    void View::_unviewItem(std::shared_ptr<ViewItem> item)
    {
        auto frame = item->frame();
        if (frame)
            return;

        item->unbind();
        size_t hash = typeid(item.get()).hash_code();
        _template_caches[hash].push_back(frame);
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


    std::shared_ptr<Control> View::fetchControl(uintx_t hash)
    {
        
    }

    void View::returnControl(std::shared_ptr<Control> control)
    {
        
    }
}

