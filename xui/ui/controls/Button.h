#pragma once
#include "ui/base/Button.h"

namespace ui::controls
{
    class Button : public ui::base::Button
    {
    public:
        Button();
        Button(const std::u8string & text);
        ~Button();

        static void propertyTableCallback(core::property_table & properties);
        void propertyTable(core::property_table & properties) override;

        void setText(const std::u8string & text);
        const std::u8string & text() const { return _text; }

		void onEnterScene() override;
		void onLeaveScene() override;
        void update() override;
        void paint(drawing::Graphics & graphics, const core::rectf & clip) const override;

    private:
		std::u8string _text;
		std::shared_ptr<drawing::Text> _text_object;
    };
}
