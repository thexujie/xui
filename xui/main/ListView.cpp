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

        graphics.drawString(_text, _rect, drawing::StringFormat().font(v->font()).color(v->color()));
    }

    ListView::ListView()
	{
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
        float top = -_scroll_pos.y;
        for (size_t index = 0; index != _items.size(); ++index)
        {
            auto & item = _items[index];
            auto psize = item->prefferdSize(box.cx);
            item->place({ box.x, box.y + top, psize.cx, psize.cy });
            top += psize.cy;
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

        setLayoutedSize({ box.cx, top });
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
}

