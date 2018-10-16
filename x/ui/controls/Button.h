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
        const std::string & text() const { return _text; }

        core::si32f contentSize() const override;

        void draw(drawing::Graphics & graphics, const drawing::Region & region) const override;

    private:
        void _confirmBlob() const;

    private:
        std::string _text;
        mutable std::shared_ptr<drawing::TextBlob> _textBlob;
    };
}
