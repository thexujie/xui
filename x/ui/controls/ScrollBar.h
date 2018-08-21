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

        void updateContent(std::shared_ptr<component::View> & view) override;
        std::string styleName() const override;

        //void setBorder(const core::vec4<core::dimensionf> & border) { _border = border; }
        //const core::vec4<core::dimensionf> & border() const { return _border; }

        //void setPadding(const core::vec4<core::dimensionf> & padding) { if (_padding != padding) { _padding = padding; rearrange(); } }
        //const core::vec4<core::dimensionf> & padding() const { return _padding; }

        //void setBorderColors(const core::vec4<core::color32> & boderColors) { _border_colors = boderColors; }
        //const core::vec4<core::color32> & borderColors() const { return _border_colors; }
        //void setBorderStyles(const core::vec4<graphics::stroke_style> & boderStyles) { _border_styles = boderStyles; }
        //const core::vec4<core::color32> & boderColors() const { return _border_colors; }
        //std::array<core::pt32f, 4> boderPoints(core::align edge) const;
        //std::array<core::pt32f, 2> boderLine(core::align edge) const;

        void setBarColor(const core::color32 & color) { if (_bar_color != color) { _bar_color = color; invalid(); } }
        const core::color32 & barColor() const { return _bar_color; }

        void setBarBorder(const core::dimensionf & border) { if (_bar_border != border) { _bar_border = border; invalid(); } }
        const core::dimensionf & barBorder() const { return _bar_border; }

        void setBarBorderColor(const core::color32 & color) { if (_bar_border_color != color) { _bar_border_color = color; invalid(); } }
        const core::color32 & barBorderColor() const { return _bar_border_color; }

        void setBarBorderStyle(const graphics::stroke_style & style) { if (_bar_border_style != style) { _bar_border_style = style; invalid(); } }
        const graphics::stroke_style & barBorderStyle() const { return _bar_border_style; }


    public:
        void setValue(float32_t val);
        float32_t rangeValue() const { return _max - _min; }
        float32_t pageValue() const { return core::equal(_page, 0.0f) ? rangeValue() * 0.1f : _page; }

        float32_t barHeight() const;
        float32_t barSpace() const;
        core::rc32f barRect() const;
    public:
        void onBarMouseEnter(const component::mosue_state & state);
        void onBarMouseMove(const component::mosue_state & state);
        void onBarMouseLeave(const component::mosue_state & state);
        void onBarMouseDown(const component::mosue_state & state);
        void onBarMouseUp(const component::mosue_state & state);

    public:
        void onSizeChanged(const core::si32f & from, const core::si32f & to) override;

    private:
        std::shared_ptr<interactables::MouseRectangle> _mc_bar;
        std::shared_ptr<renderables::Rectangle> _bar;

        // bar
        core::attribute<core::color32> _bar_color;
        core::attribute<core::dimensionf> _bar_border;
        core::attribute<core::color32> _bar_border_color;
        core::attribute<graphics::stroke_style> _bar_border_style;

        float32_t _min = 0.0f;
        float32_t _max = 100.0f;
        float32_t _val = 50.0f;
        float32_t _page = 10.0f;
        float32_t _line = 1.0f;

        core::float32_t _bar_drag_start_vallue = 0.0f;
        core::pt32f _bar_drag_mouse_pos;
    };
}
