#include "stdafx.h"
#include "ListView.h"

namespace ui
{
    core::sizef ListViewItem::prefferdSize(float32_t width) const
    {
        auto v = view();
        if (!v)
            return {};

        auto mode = v->viewMode();
        if (mode == view_mode::details)
        {
            core::sizef icon_size = v->calc(_icon_size.value_or(core::dimen2f(1_em, 1_em)));
            drawing::TextWraper wraper(_text);
            wraper.itermize(v->font(), core::colors::Auto);
            wraper.layout(width, _wrap_mode);
            auto bounds = wraper.bounds();
            auto spacing = v->calc(_icon_text_spacing);
            return core::sizef(icon_size.cx + spacing + bounds.cx, bounds.cy) + v->calc(padding()).bsize();
        }
        else
            throw core::exception(core::error_not_supported);
    }

    void ListViewItem::paint(drawing::Graphics & graphics, const core::rectf & clip) const
    {
        auto v = view();
        if (!v)
            return;

        auto cbox = contentBox();
        auto mode = v->viewMode();
        if (mode == view_mode::details)
        {
            core::sizef icon_size = v->calc(_icon_size.value_or(core::dimen2f(1_em, 1_em)));
            auto spacing = v->calc(_icon_text_spacing);
            if (_icon)
            {
                graphics.drawImage(*_icon, core::rectf(cbox.x, cbox.y + (cbox.cy - icon_size.cy) / 2, icon_size.cx, icon_size.cy));
            }
            graphics.drawString(_text, cbox.offset(icon_size.cx + spacing, 0), drawing::StringFormat().font(v->font()).color(v->color()));
        }
        else
        {
            auto tile_size = v->calc(v->tileSize());
            if (_icon)
                graphics.drawImage(*_icon, core::rectf(cbox.x, cbox.y, tile_size.cy, tile_size.cy));

            auto spacing = v->calc(_icon_text_spacing);
            auto text_x = cbox.x + tile_size.cy + spacing;
            graphics.drawString(_text, core::pointf(text_x, cbox.y), drawing::StringFormat().font(v->font()).color(v->color()));

            drawing::fontmetrics fm(v->font());
            float_t pos_y = cbox.y + fm.height;
            size_t nprops = std::ceil(cbox.bottom() - pos_y) / fm.height;
            for(size_t iprop = 0; iprop < std::min(nprops, _propertiess.size()); ++iprop)
            {
                auto & prop = _propertiess[iprop];
                auto color = v->color();
                color.a = color.a * 2 / 3;
                graphics.drawString(prop, core::rectf(text_x, pos_y, cbox.right() - text_x, fm.height), drawing::StringFormat().font(v->font()).color(color));
                pos_y += fm.height;
            }
        }
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
		if(_actived)
			return "listview:active";
		else if(_hovered)
			return "listview:hover";
		else
			return "listview";
	}

    void ListView::update()
    {
        auto cbox = contentBox();
        core::sizef lsize;
        if(_mode == view_mode::details)
        {
            float32_t top = -_scroll_pos.y;
            for (size_t index = 0; index != _items.size(); ++index)
            {
                auto & item = _items[index];
                auto ps = item->prefferdSize(cbox.cx);
                auto m = calc(item->margin());

                {
                    core::rectf box = { cbox.x + m.bleft, cbox.y + top + m.btop, cbox.cx - m.bwidth(), ps.cy };
                    item->place(box, ps);
                    item->setShown(box.bottom() >= cbox.y && box.y < cbox.bottom());
                }

                top += ps.cy + m.bheight();
                lsize.cy += ps.cy + m.bheight();
            }
            lsize.cx = cbox.cx;
        }
        else if (_mode == view_mode::tile)
        {
            auto m = calc(_item_margin);
            core::sizef ps = calc(_tile_size) + calc(_item_padding).bsize();
            core::pointf pos = cbox.pos - _scroll_pos;
            size_t ncols = std::max(1, int(cbox.cx / (ps.cx + m.bwidth())));
            for (size_t index = 0; index != _items.size(); ++index)
            {
                auto & item = _items[index];
                size_t row = index / ncols;
                size_t col = index % ncols;
                {
                    core::rectf box = { pos.x + m.bleft + (ps.cx + m.bwidth()) * col, pos.y + m.btop + (ps.cy + m.bheight()) * row, ps.cx, ps.cy };
                    item->place(box, ps);
                    item->setShown(cbox.intersect_with(box));
                }
            }
            size_t nrows = _items.size() / ncols;
            lsize = {ncols * (ps.cx + m.bwidth()), nrows * (ps.cy + m.bheight())};
        }
        setScrollSize(lsize);
        setContentSize(lsize);
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

    void ListView::onScrollPosChanged(const core::vec2f & from, const core::vec2f & to)
    {
        review();
    }

	void ListView::onAction(action_t action)
    {
		switch (action)
		{
		case 0x100:
			setViewMode(view_mode::details);
			break;
		case 0x101:
			setViewMode(view_mode::tile);
			break;
		}
    }

	void ListView::onPopupMenu(const input_state & state, IMenuPresenter & presenter)
    {
		auto item_details = std::make_shared<ui::MenuItem>(drawing::Image("icon.png"), u8"œÍœ∏–≈œ¢", ui::shortcut(), 0x100, item_flag::none);
		auto item_tile = std::make_shared<ui::MenuItem>(drawing::Image("icon.png"), u8"∆Ω∆Ã", ui::shortcut(), 0x101, item_flag::none);
		presenter.appendItems({ item_details, item_tile });
    }

    void ListView::view()
    {
        auto cbox = contentBox();
        if (_mode == view_mode::details)
        {
            float32_t top = -_scroll_pos.y;
            for (size_t index = 0; index != _items.size(); ++index)
            {
                auto & item = _items[index];
                auto & b = item->box();
                auto m = calc(item->margin());

                {
                    core::rectf box = { cbox.x + m.bleft, cbox.y + top + m.btop, cbox.cx - m.bwidth(), b.cy };
                    item->place(box, b.size);
                    item->setShown(box.bottom() >= cbox.y && box.y < cbox.bottom());
                }

                top += b.cy + m.bheight();
            }
        }
        else if (_mode == view_mode::tile)
        {
            auto m = calc(_item_margin);
            core::sizef ps = calc(_tile_size) + calc(_item_padding).bsize();
            core::pointf pos = cbox.pos - _scroll_pos;
            size_t ncols = std::max(1, int(cbox.cx / (ps.cx + m.bwidth())));
            for (size_t index = 0; index != _items.size(); ++index)
            {
                auto & item = _items[index];
                size_t row = index / ncols;
                size_t col = index % ncols;
                {
                    core::rectf box = { pos.x + m.bleft + (ps.cx + m.bwidth()) * col, pos.y + m.btop + (ps.cy + m.bheight()) * row, ps.cx, ps.cy };
                    item->place(box, ps);
                    item->setShown(cbox.intersect_with(box));
                }
            }
        }
        repaint();
        if (auto f = form())
            f->setEvent(scene_event::update_mouse_pos);
    }
}

