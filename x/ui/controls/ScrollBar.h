#pragma once
#include "ui/Control.h"
#include "ui/interactables/MouseRectangle.h"

namespace ui::controls
{
    class ScrollBar : public Control
    {
    public:
        ScrollBar();
        ScrollBar(std::string text);
        ~ScrollBar();

        static void propertyTableCallback(core::property_table & properties);
        void propertyTable(core::property_table & properties) override;

        void setText(const std::string & text);
        const std::string & text() const { return _text; }

        core::si32f contentSize() const override;
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

        void setBarBorder(const core::vec4<core::dimensionf> & border) { if (_bar_border != border) { _bar_border = border; invalid(); } }
        const core::vec4<core::dimensionf> & barBorder() const { return _bar_border; }

        void setBarBorderColors(const core::vec4<core::color32> & boderColors) { if (_bar_border_colors != boderColors) { _bar_border_colors = boderColors; invalid(); } }
        const core::vec4<core::color32> & barBorderColors() const { return _bar_border_colors; }

        void setBarBorderStyles(const core::vec4<graphics::stroke_style> & styles) { if (_bar_border_styles != styles) { _bar_border_styles = styles; invalid(); } }
        const core::vec4<graphics::stroke_style> & barBorderStyles() const { return _bar_border_styles; }



    public:
        void onMouseEnter(const component::mosue_state & state);
        void onMouseMove(const component::mosue_state & state);
        void onMouseLeave(const component::mosue_state & state);
        void onMouseDown(const component::mosue_state & state);
        void onMouseUp(const component::mosue_state & state);

    private:
        // 确保 textBlob 已经被创建
        void _confirmBlob() const;

    private:
        std::string _text;
        std::shared_ptr<graphics::TextBlob> _textBlob;

        std::shared_ptr<interactables::MouseRectangle> _mc_bar;

        std::shared_ptr<ui::Element> _bar;
        // bar
        core::attribute<core::color32> _bar_color;
        core::vec4<core::dimensionf> _bar_border;
        core::attribute<core::vec4<core::color32>> _bar_border_colors;
        core::attribute<core::vec4<graphics::stroke_style>> _bar_border_styles;
    };
}
