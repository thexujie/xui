#include "stdafx.h"
#include "Menu.h"

namespace ui
{
    Menu::Menu()
	{
        _scroll_controls = false;
        _interactable = true;
	}

    Menu::Menu(form_styles styles, std::shared_ptr<Form> parentForm) : Form(styles, parentForm)
    {
        _scroll_controls = false;
        _interactable = true;
    }

    Menu::~Menu()
	{

	}

	void Menu::propertyTableCallback(core::property_table & properties)
	{
        Form::propertyTableCallback(properties);
        properties["item-margin"] = make_accessor(&Menu::setItemMargin, &Menu::itemMargin);
        properties["item-padding"] = make_accessor(&Menu::setItemPadding, &Menu::itemPadding);
        properties["marked-color"] = make_accessor(&Menu::setMarkedColor, &Menu::markedColor);
        properties["selected-color"] = make_accessor(&Menu::setSelectedColor, &Menu::selectedColor);
	}

	void Menu::propertyTable(core::property_table & properties)
	{
        Menu::propertyTableCallback(properties);
	}

	std::string Menu::styleName() const
	{
		if(_actived)
			return "menu:active";
		else if(_hovered)
			return "menu:hover";
		else
			return "menu";
	}

    core::sizef Menu::contentSize() const
    {
        float32_t widthA = 0;
        float32_t widthB = 0;
        float32_t widthC = 0;
        core::sizef size;
        auto is = calc(_icon_size);
        auto spliter_size = calc(_spliter_size);
        auto item_padding = calc(_item_padding);
        auto item_margin = calc(_item_margin);
        auto item_height = calc(1_em);
        for (size_t index = 0; index != _items.size(); ++index)
        {
            auto & item = _items[index];
            if(item->text.empty())
            {
                size.cy += spliter_size;
            }
            else
            {
                widthA = std::max(widthA, item->text.bounds().cx);
                widthB = std::max(widthB, item->shortcut.bounds().cx);
                if(item->flags.any(container))
                    widthC = std::max(widthB, is.cx);
                size.cy += item_height;
            }
            size.cy += item_margin.bheight();
            size.cy += item_padding.bheight();
        }

        size.cx = is.cx + 
            calc(_icon_text_spacing) + widthA + 
            calc(_text_shortcut_spacing) + widthB +
            calc(_shortcut_arrow_spacing) + widthC + 
            item_padding.bwidth() + item_margin.bwidth();
        return size;
    }

    void Menu::update()
    {
        float32_t widthA = 0;
        float32_t widthB = 0;
        float32_t widthC = 0;
        core::sizef size;
        auto is = calc(_icon_size);
        auto p = calc(_item_padding);
        auto m = calc(_item_margin);
        auto spliter_size = calc(_spliter_size);
        auto item_height = calc(1_em);
        for (size_t index = 0; index != _items.size(); ++index)
        {
            auto & item = _items[index];
            item->text.update(font(), color());
            item->shortcut.update(font(), color());

            if (item->text.empty())
            {
                size.cy += spliter_size;
            }
            else
            {
                widthA = std::max(widthA, item->text.bounds().cx);
                widthB = std::max(widthB, item->shortcut.bounds().cx);
                if (item->flags.any(container))
                    widthC = std::max(widthB, is.cx);
                size.cy += item_height;
            }
            size.cy += m.bheight();
            size.cy += p.bheight();
        }

        size.cx = is.cx +
            calc(_icon_text_spacing) + widthA +
            calc(_text_shortcut_spacing) + widthB +
            calc(_shortcut_arrow_spacing) + widthC +
            p.bwidth() + m.bwidth();

        setLayoutedSize(size);
    }

	void Menu::paint(drawing::Graphics & graphics, const core::rectf & clip) const
	{
        graphics.save();
        graphics.setClipRect(contentBox());
        auto box = contentBox();

        float32_t widthA = 0;
        float32_t widthB = 0;
        float32_t widthC = 0;
        auto is = calc(_icon_size);
        auto item_padding = calc(_item_padding);
        auto item_height = calc(1_em);
        auto spliter_size = calc(_spliter_size);
        for (size_t index = 0; index != _items.size(); ++index)
        {
            auto & item = _items[index];
            if (!item->text.empty())
            {
                widthA = std::max(widthA, item->text.bounds().cx);
                widthB = std::max(widthB, item->shortcut.bounds().cx);
                if (item->flags.any(container))
                    widthC = std::max(widthB, is.cx);
            }
        }

        float32_t posY = -_scroll_pos.y;
        auto itspacing = calc(_icon_text_spacing);
        auto tsspacing = calc(_text_shortcut_spacing);
        auto item_margin = calc(_item_margin);
        for(size_t index = 0; index != _items.size(); ++index)
        {
            auto & item = _items[index];
            if (posY + item_margin.bheight() + item_padding.bheight() + item_height < 0)
            {
                posY += item_margin.bheight();
                posY += item_padding.bheight();
                posY += item_height;
                continue;
            }

            if (posY >= clip.bottom() || posY >= box.bottom())
                break;

            if(item->text.empty())
            {
				core::rectf content_rect = { box.x + item_margin.bleft + item_padding.bleft, box.y + posY + item_margin.btop + item_padding.btop, box.cx - item_margin.bwidth() - item_padding.bwidth(), spliter_size };
				graphics.drawRectangle(content_rect, drawing::PathStyle().fill(_spliiter_color));
                posY += spliter_size;
            }
            else
            {
				core::rectf item_rect = { box.x + item_margin.bleft, box.y + posY + item_margin.btop, box.cx - item_margin.bwidth(), item_height + item_padding.bheight() };
				core::rectf content_rect = { box.x + item_margin.bleft + item_padding.bleft, box.y + posY + item_margin.btop + item_padding.btop, box.cx - item_margin.bwidth() - item_padding.bwidth(), item_height };
				auto & flags = item->flags;
				if (flags.any(item_flag::selected))
					graphics.drawRectangle(item_rect, drawing::PathStyle().fill(_selected_color));
				else if (flags.any(item_flag::marked))
					graphics.drawRectangle(item_rect, drawing::PathStyle().fill(_marked_color));
				else {}

                if (item->icon)
                    graphics.drawImage(item->icon, core::rectf(content_rect.x, content_rect.y, is.cx, is.cy));
                graphics.drawText(item->text, core::pointf(item_rect.x + itspacing, content_rect.y), drawing::StringFormat());
                graphics.drawText(item->shortcut, core::pointf(item_rect.x + itspacing + widthA + tsspacing, content_rect.y), drawing::StringFormat());
                posY += item_height;
            }
            posY += item_margin.bheight();
            posY += item_padding.bheight();
        }
        graphics.restore();
	}

    void Menu::onScrollPosChanged(const core::vec2f & from, const core::vec2f & to)
    {
        refresh();
        repaint();
    }

    void Menu::onHover(const ui::input_state & state)
    {
        auto item = findItem(state.pos());
        _setMarkedItem(item);
        ui::Container::onHover(state);
    }

    void Menu::onHoverOut(const ui::input_state & state)
    {
        _setMarkedItem(nullptr);
        ui::Container::onHoverOut(state);
    }

	void Menu::onActive(const input_state & state)
    {
		ui::Container::onActive(state);
	    if(_marked_item)
	    {
			_marked_item->active(_marked_item->action);
			hide();
	    }
    }

	size_t Menu::appendItem(std::shared_ptr<MenuItem> item)
    {
        return insertItem(_items.size(), item);
    }

	size_t Menu::appendItems(std::initializer_list<std::shared_ptr<MenuItem>> && items)
    {
		for (auto item : items)
			appendItem(item);
		return _items.size();
    }

	size_t Menu::insertItem(size_t index, std::shared_ptr<MenuItem> item)
    {
        auto iter = _items.begin() + index;
        _items.insert(iter, item);
        refresh();
        return _items.size();
    }

    size_t Menu::eraseItem(size_t index)
    {
        auto item = _items[index];
        auto iter = _items.begin() + index;
        _items.erase(iter);
        refresh();
        return index;
    }

    std::shared_ptr<MenuItem> Menu::findItem(const core::pointf & pos) const
    {
        float32_t offset = -_scroll_pos.y;
        core::sizef size;
        auto is = calc(_icon_size);
        auto p = calc(_item_padding);
        auto m = calc(_item_margin);
		if (pos.y < m.btop)
			return nullptr;

        auto item_height = calc(1_em);
        auto spliter_size = calc(_spliter_size);
        for (size_t index = 0; index != _items.size(); ++index)
        {
            auto & item = _items[index];

            float32_t height = item->text.empty() ? spliter_size : item_height;
            height += m.bheight();
            height += p.bheight();

            if (pos.y < offset + height)
                return item;

            offset += height;
        }
        return nullptr;
    }

    void Menu::_setMarkedItem(std::shared_ptr<MenuItem> item)
    {
        if (item != _marked_item)
        {
            if (_marked_item)
            {
                _marked_item->flags.set(item_flag::marked, false);
                if (_marked_color)
                    repaint();
            }
            _marked_item = item;
            if (_marked_item)
            {
                _marked_item->flags.set(item_flag::marked, true);
                if (_marked_color)
                    repaint();
            }
        }
    }
}

