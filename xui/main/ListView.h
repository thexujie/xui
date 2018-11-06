#pragma once

#include "ui/base/Button.h"
#include "ui/Container.h"
#include "ui/controls/Text.h"

namespace ui 
{
	class Form;
}

namespace ui::controlsex
{
    class ListView;
    enum item_flag
    {
        hoving = 0x0001,
    };
    typedef core::bitflag<item_flag> item_flags;

    enum select_mode
    {
        restrictly = 0,
        full_row,
    };
    class ViewItem
    {
    public:
        virtual ~ViewItem() {}
        std::shared_ptr<ListView> view() const { return _view.lock(); }
        virtual void enterView(std::shared_ptr<ListView> view) { _view = view; }
        virtual void leaveView() { _view.reset(); }
        void place(const core::rectf & rect) { _rect = rect; }
        const core::rectf & rect() const { return _rect; }
        const item_flags & flags() const { return _flags; }
        void setFlag(item_flag flag, bool set) { _flags.set(flag, set); }

    public:
        virtual core::sizef prefferdSize(float32_t width) const = 0;
        virtual void draw(drawing::Graphics & graphics, const core::rectf & clip) = 0;

    protected:
        std::weak_ptr<ListView> _view;
        core::rectf _rect;
        item_flags _flags;
    };

    class ListViewItem : public ViewItem
    {
    public:
        ListViewItem(std::string text, drawing::wrap_mode wrap_mode = drawing::wrap_mode::none) : _text(text), _wrap_mode(wrap_mode) {}
        core::sizef prefferdSize(float32_t width) const;
        void draw(drawing::Graphics & graphics, const core::rectf & clip);

    private:
        std::string _text;
        drawing::wrap_mode _wrap_mode = drawing::wrap_mode::none;
    };

	class ListView : public ui::Container
	{
	public:
        ListView();
		~ListView();

		static void propertyTableCallback(core::property_table & properties);
		void propertyTable(core::property_table & properties) override;

		std::string styleName() const override;

        core::sizef contentSize() const override;
        void layout(layout_flags flags) override;
		void draw(drawing::Graphics & graphics, const core::rectf & clip) const override;

	public:
        void onMouseMove(const ui::input_state & state) override;
        void onMouseClick(const ui::input_state & state, ui::mouse_button button) override;

	public:
        size_t addItem(std::shared_ptr<ViewItem> item);
        size_t insertItem(size_t index, std::shared_ptr<ViewItem> item);
        size_t eraseItem(size_t index);
        std::shared_ptr<ViewItem> findItem(const core::pointf & pos) const;

	private:
        std::vector<std::shared_ptr<ViewItem>> _items;
        std::shared_ptr<ViewItem> _current_item;
        select_mode _select_mode = select_mode::full_row;
	};
}
