#pragma once
#include "ui/Control.h"
#include "ui/interactables/MouseRectangle.h"

namespace ui::controls
{
    class ScrollBar : public Control
    {
    public:
        ScrollBar();
        ~ScrollBar();

        static void propertyTableCallback(core::property_table & properties);
        void propertyTable(core::property_table & properties) override;

        void setDirection(core::align direction) { if (_direction != direction) { _direction = direction; refresh(); } }
        core::align direction() const { return _direction; }

        void updateContent() override;
        std::string styleName() const override;

        void setBarColor(const core::color32 & color) { if (_bar_color != color) { _bar_color = color; refresh(); } }
        const core::color32 & barColor() const { return _bar_color; }

        void setBarBorder(const core::dimensionf & border) { if (_bar_border != border) { _bar_border = border; refresh(); } }
        const core::dimensionf & barBorder() const { return _bar_border; }

        void setBarBorderColor(const core::color32 & color) { if (_bar_border_color != color) { _bar_border_color = color; refresh(); } }
        const core::color32 & barBorderColor() const { return _bar_border_color; }

        void setBarBorderStyle(const graphics::stroke_style & style) { if (_bar_border_style != style) { _bar_border_style = style; refresh(); } }
        const graphics::stroke_style & barBorderStyle() const { return _bar_border_style; }

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
        core::rc32f barRect() const;
    public:
        void onBarMouseEnter(const component::mosue_state & state);
        void onBarMouseMove(const component::mosue_state & state);
        void onBarMouseLeave(const component::mosue_state & state);
        void onBarMouseDown(const component::mosue_state & state);
        void onBarMouseUp(const component::mosue_state & state);
        void onMouseWheel(const component::mosue_state & state);

    public:
        void onSizeChanged(const core::si32f & from, const core::si32f & to) override;

    public:
        core::event<void(float32_t from, float32_t to)> valueChagned;

    private:
        core::align _direction = core::align::top;

        // bar
        core::attribute<core::color32> _bar_color;
        core::attribute<core::dimensionf> _bar_border;
        core::attribute<core::color32> _bar_border_color;
        core::attribute<graphics::stroke_style> _bar_border_style;

        float32_t _min = 0.0f;
        float32_t _max = 100.0f;
        float32_t _val = 0.0f;
        float32_t _line = std::nan("0");
        float32_t _page = std::nan("0");

        core::float32_t _bar_drag_start_vallue = 0.0f;
        core::pt32f _bar_drag_mouse_pos = { std::nanf("0"), std::nanf("0") };

        std::shared_ptr<interactables::MouseRectangle> _mc;
        std::shared_ptr<interactables::MouseRectangle> _mc_bar;
        std::shared_ptr<renderables::Rectangle> _bar;
    };
}
