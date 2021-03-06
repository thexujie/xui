#pragma once
#include "ui/Control.h"

namespace ui::controls
{
    class ScrollBar : public Control
    {
    public:
        ScrollBar();
        ~ScrollBar();

        static void propertyTableCallback(core::property_table & properties);
        void propertyTable(core::property_table & properties) override;

        void setDirection(core::align direction) { if (_direction != direction) { _direction = direction; repaint(); } }
        core::align direction() const { return _direction; }

        std::u8string styleName() const override;

        void setBarColor(const core::color & color) { if (_bar_color != color) { _bar_color = color; repaint(); } }
        const core::color & barColor() const { return _bar_color; }

        void setBarBorder(const core::dimenf & border) { if (_bar_border != border) { _bar_border = border; repaint(); } }
        const core::dimenf & barBorder() const { return _bar_border; }

        void setBarBorderColor(const core::color & color) { if (_bar_border_color != color) { _bar_border_color = color; repaint(); } }
        const core::color & barBorderColor() const { return _bar_border_color; }

        void setBarBorderStyle(const drawing::stroke_style & style) { if (_bar_border_style != style) { _bar_border_style = style; repaint(); } }
        const drawing::stroke_style & barBorderStyle() const { return _bar_border_style; }

        void paint(drawing::Graphics & graphics, const core::rectf & clip) const override;
    public:
        float32_t value() const { return _val; }
        float32_t minValue() const { return _min; }
        float32_t maxValue() const { return _max; }
        void setValue(float32_t val);
        void setPageValue(float32_t pageValue) { _page = pageValue; }
        void setValues(float32_t val, float32_t min, float32_t max);
        float32_t rangeValue() const { return _max - _min; }
        float32_t lineValue() const { return std::isnan(_line) || core::is_zero(_line) ? rangeValue() * 0.05f : _line; }
        float32_t pageValue() const { return std::isnan(_page) || core::is_zero(_page) ? rangeValue() * 0.1f : _page; }

        float32_t scrollRate() const { return _max > _min ? _val / (_max - _min) : 0; }
        float32_t barRate() const { return pageValue() / (rangeValue() + pageValue()); }

        float32_t barSize() const;
        float32_t barSpace() const;
        float32_t barPos() const { return barSpace() * scrollRate(); }
        core::rectf barRect() const;
    public:
        void onHover(const input_state & state);
        void onHoverOut(const input_state & state);
        void onActiveIn(const input_state & state);
        void onActiveOut(const input_state & state);
        void onWheel(const input_state & state);

    public:
        void onSizeChanged(const core::sizef & from, const core::sizef & to) override;

    private:
        void _updateMouse(const input_state & state);
        void _updateBarState(bool hover, bool active);

    public:
        core::event<void(float32_t from, float32_t to)> valueChagned;

    private:
        core::align _direction = core::align::top;

        // bar
        core::attribute<core::color> _bar_color;
        core::attribute<core::dimenf> _bar_border;
        core::attribute<core::color> _bar_border_color;
        core::attribute<drawing::stroke_style> _bar_border_style;

        float32_t _min = 0.0f;
        float32_t _max = 100.0f;
        float32_t _val = 0.0f;
        float32_t _line = std::nanf("0");
        float32_t _page = std::nanf("0");

        float32_t _bar_drag_start_vallue = 0.0f;
        core::pointf _bar_drag_mouse_pos = { std::nanf("0"), std::nanf("0") };

        bool _bar_hover = false;
        bool _bar_active = false;
    };
}
