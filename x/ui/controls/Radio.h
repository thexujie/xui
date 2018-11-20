#pragma once

#include "ui/base/Radio.h"

namespace ui::controls
{
    class Radio : public base::Radio
    {
    public:
        Radio();
        Radio(std::string text);
        ~Radio();

        static void propertyTableCallback(core::property_table & properties);
        void propertyTable(core::property_table & properties) override;

        void setText(const std::string & text);
        const std::string & text() const { return _text; }

        core::sizef contentSize() const override;
        std::string styleName() const override;

        void paint(drawing::Graphics & graphics, const core::rectf & clip) const override;

    public:
        void onMouseEnter(const input_state & state) override;
        void onMouseMove(const input_state & state) override;
        void onMouseLeave(const input_state & state) override;
        void onMouseDown(const input_state & state, ui::mouse_button button) override;
        void onMouseUp(const input_state & state, ui::mouse_button button) override;

    private:
        void _confirmBlob() const;

        void _setHoleColor(core::color color);
        void _setHoleBorderSize(core::dimenf border_size);
        void _setHoleBorderColor(core::color color);
        void _setDotColor();
        void _setDotBorderColor();

    private:
        std::string _text;

        mutable std::shared_ptr<drawing::TextBlob> _textBlob;

        core::dimenf _content_spacing = 0.2_em;

        core::dimenf _hole_border_size = 1_px;
        core::color _hole_border_color = core::colors::Gray;
        core::color _hole_color = core::colors::Transparent;

        core::dimenf _dot_border_size = 1_px;
        core::color _dot_border_color = core::colors::Gray;
        core::color _dot_color = core::colors::Black;
    };
}
