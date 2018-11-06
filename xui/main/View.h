#pragma once

#include "ui/base/Button.h"
#include "ui/Container.h"
#include "ui/controls/Text.h"

namespace ui
{
    class View;

    enum item_flag
    {
        marked = 0x0001,
        activated = 0x0002,
        selected = 0x0004,

        placed = 0x10000,
    };
    typedef core::bitflag<item_flag> item_flags;

    enum select_mode
    {
        restrictly = 0,
        full_row,
    };

    class ItemData
    {
    public:
        virtual ~ItemData() {}
        std::shared_ptr<View> view() const { return _view.lock(); }
        virtual void enterView(std::shared_ptr<View> view) { _view = view; }
        virtual void leaveView() { _view.reset(); }

        void setMargin(const core::vec4<core::dimensionf> & margin) { _margin = margin; }
        const core::vec4<core::dimensionf> & margin() const;
        void setPadding(const core::vec4<core::dimensionf> & padding) { _padding = padding; }
        const core::vec4<core::dimensionf> & padding() const;

        void place(const core::rectf & box, const core::sizef & size) { _box = box, _size = size; }
        const core::rectf & box() const { return _box; }
        const core::sizef & size() const { return _size; }
        core::rectf contentBox() const;
        core::rectf rect() const { return core::rectf{_box.pos, _size}; }
        const item_flags & flags() const { return _flags; }
        void setFlag(item_flag flag, bool set) { _flags.set(flag, set); }

    public:
        virtual core::sizef prefferdSize(float32_t width) const = 0;
        virtual void draw(drawing::Graphics & graphics, const core::rectf & clip) = 0;

    protected:
        std::weak_ptr<View> _view;
        core::attribute<core::vec4<core::dimensionf>> _margin;
        core::attribute<core::vec4<core::dimensionf>> _padding;

        core::rectf _box;
        core::sizef _size;
        item_flags _flags;
    };

    class ItemView
    {
    public:
        virtual ~ItemView() {}
        std::shared_ptr<View> view() const { return _view.lock(); }
        virtual void enterView(std::shared_ptr<View> view) { _view = view; }
        virtual void leaveView() { _view.reset(); }

        std::shared_ptr<ItemData> data() const { return _data; }
        const core::rectf & box() const { return _box; }
    protected:
        std::weak_ptr<View> _view;
        std::shared_ptr<ItemData> _data;
        core::rectf _box;
        core::sizef _size;
    };

	class View : public ui::Container
	{
	public:
        View();
		~View();

		static void propertyTableCallback(core::property_table & properties);
		void propertyTable(core::property_table & properties) override;

	public:
        size_t addItem(std::shared_ptr<ItemData> item);
        size_t insertItem(size_t index, std::shared_ptr<ItemData> item);
        size_t eraseItem(size_t index);
        std::shared_ptr<ItemData> findItem(const core::pointf & pos) const;

        void setItemMargin(const core::vec4<core::dimensionf> & spacing) { _item_margin = spacing; }
        const core::vec4<core::dimensionf> & itemMargin() const { return _item_margin; }
        void setItemPadding(const core::vec4<core::dimensionf> & spacing) { _item_padding = spacing; }
        const core::vec4<core::dimensionf> & itemPadding() const { return _item_padding; }
        void setMarkedColor(const core::color & c) { _marked_color = c; }
        core::color markedColor() const { return _marked_color; }
        void setSelectedColor(const core::color & c) { _selected_color = c; }
        core::color selectedColor() const { return _selected_color; }

    protected:
        void _setMarkedItem(std::shared_ptr<ItemData> item);
        void _setSelectedItem(std::shared_ptr<ItemData> item);

	protected:
        std::vector<std::shared_ptr<ItemData>> _items;
        std::vector<std::shared_ptr<ItemView>> _views;
        std::shared_ptr<ItemData> _marked_item;
        std::shared_ptr<ItemData> _selected_item;
        select_mode _select_mode = select_mode::full_row;
        core::attribute<core::vec4<core::dimensionf>> _item_margin;
        core::attribute<core::vec4<core::dimensionf>> _item_padding;

       core::color _marked_color = core::colors::Auto;
       core::color _selected_color = core::colors::Auto;
	};
}
