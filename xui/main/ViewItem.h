#pragma once

namespace ui
{
    class View;

    enum item_flag
    {
        marked = 0x0001,
        activated = 0x0002,
        selected = 0x0004,
        shown = 0x0008,

        placed = 0x10000,
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
        std::shared_ptr<View> view() const { return _view.lock(); }

        virtual void enterView(std::shared_ptr<View> view) { _view = view; }
        virtual void leaveView() { _view.reset(); }

        void setMargin(const core::vec4<core::dimensionf> & margin) { _margin = margin; }
        const core::vec4<core::dimensionf> & margin() const;
        void setPadding(const core::vec4<core::dimensionf> & padding) { _padding = padding; }
        const core::vec4<core::dimensionf> & padding() const;

        virtual void place(const core::rectf & box, const core::sizef & size) { _box = box, _size = size; }
        const core::rectf & box() const { return _box; }
        const core::sizef & size() const { return _size; }
        core::rectf contentBox() const;
        core::rectf rect() const { return core::rectf{_box.pos, _size}; }
        const item_flags & flags() const { return _flags; }
        void setFlag(item_flag flag, bool set);

        void setMarked(bool m) { setFlag(item_flag::marked, m); }
        bool marked() const { return _flags.any(item_flag::marked); }
        void setActivated(bool a) { setFlag(item_flag::activated, a); }
        bool activated() const { return _flags.any(item_flag::activated); }
        void setSelected(bool s) { setFlag(item_flag::selected, s); }
        bool selected() const { return _flags.any(item_flag::selected); }
        void setShown(bool s) { setFlag(item_flag::shown, s); }
        bool shown() const { return _flags.any(item_flag::shown); }

    public:
        virtual core::sizef prefferdSize(float32_t width) const = 0;
        virtual void paint(drawing::Graphics & graphics, const core::rectf & clip) = 0;

    protected:
        virtual void onFlagChanged(item_flag flag, bool set) {}

    protected:
        std::weak_ptr<View> _view;
        core::attribute<core::vec4<core::dimensionf>> _margin;
        core::attribute<core::vec4<core::dimensionf>> _padding;

        core::rectf _box;
        core::sizef _size;
        item_flags _flags;
    };
}
