#include "stdafx.h"
#include "ListView.h"

namespace ui
{
    core::sizef ListViewItem::prefferdSize(float32_t width) const
    {
        auto v = view();
        if (!v)
            return {};

        drawing::TextWraper wraper;
        wraper.itermize(_text, v->font(), core::colors::Auto);
        wraper.layout(width, _wrap_mode);
        return wraper.bounds() + v->calc(_padding).bsize();
    }

    void ListViewItem::draw(drawing::Graphics & graphics, const core::rectf & clip)
    {
        auto v = view();
        if (!v)
            return;
        graphics.drawString(_text, contentBox(), drawing::StringFormat().font(v->font()).color(v->color()));
    }

    ListView::ListView()
	{
        _interactable = true;
	}

    ListView::~ListView()
	{

	}

	void ListView::propertyTableCallback(core::property_table & properties)
	{
		View::propertyTableCallback(properties);
	}

	void ListView::propertyTable(core::property_table & properties)
	{
        ListView::propertyTableCallback(properties);
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
            auto s = item->prefferdSize(box.cx);
            auto m = calc(item->margin());
            auto p = calc(item->padding());

            if (item->flags().any(item_flag::placed))
            {
                top += s.cy + m.bheight() + p.bheight();
                height += s.cy + m.bheight() + p.bheight();
                continue;
            }

            core::rectf item_box = {m.bleft, top + m.btop, box.cx - m.bwidth(), p.bheight() + s.cy};
            item->place(item_box,  s);
            top += s.cy + m.bheight() + p.bheight();
            height += s.cy + m.bheight() + p.bheight();
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
                    control->place(box, preffer_size);
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
        scene()->setEvent(scene_event::update_mouse_pos);
        repaint();
    }

	void ListView::paint(drawing::Graphics & graphics, const core::rectf & clip) const
	{
        graphics.save();
        graphics.setClipRect(contentBox());
        for(size_t index = 0; index != _views.size(); ++index)
        {
            auto & view = _views[index];
            auto data = view->data();
            auto rect = view->box();
            if (rect.bottom() <= clip.y)
                continue;

            if (rect.y >= clip.bottom())
                break;

            auto & flags = data->flags();
            if (flags.any(item_flag::selected))
                graphics.drawRectangle(data->box(), drawing::PathStyle().fill(_selected_color));
            else if (flags.any(item_flag::marked))
                graphics.drawRectangle(data->box(), drawing::PathStyle().fill(_marked_color));
            else {}
            data->draw(graphics, clip);
        }
        graphics.restore();
	}

    void ListView::onMouseMove(const ui::input_state & state)
    {
        auto item = findItem(state.pos());
        _setMarkedItem(item);
        View::onMouseMove(state);
    }

    void ListView::onMouseLeave(const ui::input_state & state)
    {
        _setMarkedItem(nullptr);
        View::onMouseLeave(state);
    }

    void ListView::onMouseDown(const ui::input_state & state, ui::mouse_button button)
    {
        _setSelectedItem(_marked_item);
        View::onMouseDown(state, button);
    }

	void ListView::onMouseClick(const ui::input_state & state, ui::mouse_button button)
	{
        View::onMouseClick(state, button);
	}
}

