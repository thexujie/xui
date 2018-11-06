#include "stdafx.h"
#include "ListView.h"

namespace ui::controlsex
{
    core::sizef ListViewItem::prefferdSize(float32_t width) const
    {
        auto v = view();
        if (!v)
            return {};
        drawing::TextWraper wraper;
        wraper.itermize(_text, v->font(), core::colors::Auto);
        wraper.layout(width, _wrap_mode);
        return wraper.bounds();
    }

    void ListViewItem::draw(drawing::Graphics & graphics, const core::rectf & clip)
    {
        auto v = view();
        if (!v)
            return;
        if (_flags.any(item_flag::hoving))
            graphics.drawRectangle(_rect, drawing::PathStyle().fill(core::colors::IndianRed));
        graphics.drawString(_text, _rect, drawing::StringFormat().font(v->font()).color(v->color()));
    }

    ListView::ListView()
	{
        //_interactable = true;
	}

    ListView::~ListView()
	{

	}

	void ListView::propertyTableCallback(core::property_table & properties)
	{
		Control::propertyTableCallback(properties);
	}

	void ListView::propertyTable(core::property_table & properties)
	{
		TitleButton::propertyTableCallback(properties);
	}

	std::string ListView::styleName() const
	{
		if(_pressed)
			return "listview:active";
		else if(_mousein)
			return "listview:hover";
		else
			return "listview";
	}

    core::sizef ListView::contentSize() const
    {
        return {};
    }

    void ListView::layout(layout_flags flags)
    {
        auto box = contentBox();
        float32_t top = -_scroll_pos.y;
        float32_t height = 0;
        for (size_t index = 0; index != _items.size(); ++index)
        {
            auto & item = _items[index];
            auto psize = item->prefferdSize(box.cx);
            item->place({ box.x, box.y + top, psize.cx, psize.cy });
            top += psize.cy;
            height += psize.cy;
        }

        {
            for (auto & control : _controls)
            {
                if (!control->aviliable())
                    continue;

                auto lo = control->layoutOrigin();
                auto preffer_size = control->prefferSize(box.size);
                switch (lo)
                {
                case layout_origin::parent:
                    control->place(contentBox(), preffer_size);
                    break;
                case layout_origin::scene:
                    control->place(scene()->rect(), preffer_size);
                    break;
                default:
                    break;
                }
            }
        }

        setLayoutedSize({ box.cx, height });
        refresh();
    }

	void ListView::draw(drawing::Graphics & graphics, const core::rectf & clip) const
	{
        graphics.save();
        graphics.setClipRect(contentBox());
        for(size_t index = 0; index != _items.size(); ++index)
        {
            auto & item = _items[index];
            auto & rect = item->rect();
            if (rect.bottom() <= clip.y)
                continue;

            if (rect.y >= clip.bottom())
                break;

            item->draw(graphics, clip);
        }
        graphics.restore();
	}

    void ListView::onMouseMove(const ui::input_state & state)
    {
        auto item = findItem(state.pos());
        if(item != _current_item)
        {
            if(_current_item)
                _current_item->setFlag(item_flag::hoving, false);
            _current_item = item;
            if (_current_item)
            _current_item->setFlag(item_flag::hoving, true);
            refresh();
        }
        ui::Control::onMouseMove(state);
    }

	void ListView::onMouseClick(const ui::input_state & state, ui::mouse_button button)
	{
		ui::Control::onMouseClick(state, button);
	}

    size_t ListView::addItem(std::shared_ptr<ViewItem> item)
    {
        return insertItem(_items.size(), item);
    }

    size_t ListView::insertItem(size_t index, std::shared_ptr<ViewItem> item)
    {
        auto iter = _items.begin() + index;
        item->enterView(share_ref<ListView>());
        _items.insert(iter, item);
        relayout();
        return index;
    }

    size_t ListView::eraseItem(size_t index)
    {
        auto item = _items[index];
        auto iter = _items.begin() + index;
        _items.erase(iter);
        item->leaveView();
        relayout();
        return index;
    }

    std::shared_ptr<ViewItem> ListView::findItem(const core::pointf & pos) const
    {
        _confirmLayout();
        for (size_t index = 0; index != _items.size(); ++index)
        {
            auto & item = _items[index];
            if(_select_mode == select_mode::full_row)
            {
                auto & rect = item->rect();
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
}

