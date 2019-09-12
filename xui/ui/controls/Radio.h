#pragma once

#include "ui/base/Radio.h"

namespace ui::controls
{
    class Radio : public base::Radio
    {
    public:
        Radio();
        Radio(const std::u8string & text);
        ~Radio();

        static void propertyTableCallback(core::property_table & properties);
        void propertyTable(core::property_table & properties) override;

        void setText(const std::u8string & text);
        const std::u8string & text() const { return _text; }

        std::u8string styleName() const override;

		void onEnterScene() override;
		void onLeaveScene() override;
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
		std::u8string _text;
		std::shared_ptr<drawing::Text> _text_object;

        core::dimenf _content_spacing = 0.2_em;

        core::dimenf _hole_border_size = 1_px;
        core::color _hole_border_color = core::colors::Gray;
        core::color _hole_color = core::colors::Transparent;

        core::dimenf _dot_border_size = 1_px;
        core::color _dot_border_color = core::colors::Gray;
        core::color _dot_color = core::colors::Black;
    };
}
