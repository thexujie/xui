#include "stdafx.h"
#include "ListView.h"

namespace ui
{
    core::sizef ListViewItem::prefferdSize(float32_t width) const
    {
        auto v = view();
        if (!v)
            return {};

        core::sizef icon_size = _icon_size.has_value() ? v->calc(_icon_size.value()) : (_icon ? _icon->size().to<float_t>() : core::sizef{});
        drawing::TextWraper wraper;
        wraper.itermize(_text, v->font(), core::colors::Auto);
        wraper.layout(width, _wrap_mode);
        auto bounds = wraper.bounds();
        auto spacing = v->calc(_icon_text_spacing);
        return { icon_size.cx + spacing + bounds.cx, bounds.cy };
    }

    void ListViewItem::paint(drawing::Graphics & graphics, const core::rectf & clip)
    {
        auto v = view();
        if (!v)
            return;

        auto cbox = contentBox();
        core::vec2f icon_size = _icon_size.has_value() ? v->calc(_icon_size.value()) : _icon->size().to<float_t>();
        auto spacing = v->calc(_icon_text_spacing);
        if (_icon)
        {
            graphics.drawImage(*_icon, core::rectf(cbox.x, cbox.y + (cbox.cy - icon_size.cy) / 2, icon_size.cx, icon_size.cy));
        }
        graphics.drawString(_text, cbox.offset(icon_size.cx + spacing, 0), drawing::StringFormat().font(v->font()).color(v->color()));
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

    void ListView::update()
    {
        auto cbox = contentBox();
        float32_t top = -_scroll_pos.y;
        float32_t height = 0;
        for (size_t index = 0; index != _items.size(); ++index)
        {
            auto & item = _items[index];
            auto s = item->prefferdSize(cbox.cx);
            auto m = calc(item->margin());
            auto p = calc(item->padding());

            //if (!item->flags().any(item_flag::placed))
            {
                core::rectf box = { cbox.x + m.bleft, cbox.y + top + m.btop, cbox.cx - m.bwidth(), p.bheight() + s.cy };
                item->place(box, s);
                item->setShown(box.bottom() >= cbox.y && box.y < cbox.bottom());
            }


            top += s.cy + m.bheight() + p.bheight();
            height += s.cy + m.bheight() + p.bheight();
        }

        setLayoutedSize({ cbox.cx, height });
        scene()->setEvent(scene_event::update_mouse_pos);
        repaint();
    }

	void ListView::paint(drawing::Graphics & graphics, const core::rectf & clip) const
	{
        graphics.save();
        graphics.setClipRect(contentBox());
        for(size_t index = 0; index != _items.size(); ++index)
        {
            auto & item = _items[index];
            auto rect = item->box();
            if (rect.bottom() <= clip.y)
                continue;

            if (rect.y >= clip.bottom())
                break;

            auto & flags = item->flags();
            if (flags.any(item_flag::selected))
                graphics.drawRectangle(item->box(), drawing::PathStyle().fill(_selected_color));
            else if (flags.any(item_flag::marked))
                graphics.drawRectangle(item->box(), drawing::PathStyle().fill(_marked_color));
            else {}
            item->paint(graphics, clip);
        }
        graphics.restore();
	}
}

