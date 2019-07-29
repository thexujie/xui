#pragma once

#include "ui/base/Check.h"

namespace ui::controls
{
    class Check : public base::Check
    {
    public:
        Check();
        Check(const std::u8string & text);
        ~Check();

        static void propertyTableCallback(core::property_table & properties);
        void propertyTable(core::property_table & properties) override;

        void setText(const std::u8string & text);
        const std::u8string & text() const { return _text.text(); }

        std::u8string styleName() const override;

        void update() override;
        void paint(drawing::Graphics & graphics, const core::rectf & clip) const override;

    public:
        void onActiveOut(const input_state & state) override;

    private:
        void _setHoleColor(core::color color);
        void _setHoleBorderSize(core::dimenf border_size);
        void _setHoleBorderColor(core::color color);
        void _setDotColor();
        void _setDotBorderColor();

    private:
        drawing::Text _text;

        core::dimenf _content_spacing = 0.2_em;

        core::dimenf _hole_border_size = 1_px;
        core::color _hole_border_color = core::colors::Gray;
        core::color _hole_color = core::colors::Transparent;

        core::dimenf _dot_border_size = 1_px;
        core::color _dot_border_color = core::colors::Black;
        core::color _dot_color = core::colors::Gray;
    };
}
