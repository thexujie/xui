#pragma once
#include "ui/base/Button.h"

namespace ui::controls
{
    class Button : public ui::base::Button
    {
    public:
        Button();
        Button(std::string text);
        ~Button();

        static void propertyTableCallback(core::property_table & properties);
        void propertyTable(core::property_table & properties) override;

        void setText(const std::string & text);
        const std::string & text() const { return _text.text(); }

        core::sizef contentSize() const override;

        void paint(drawing::Graphics & graphics, const core::rectf & clip) const override;

    private:
        drawing::Text _text;
    };
}
