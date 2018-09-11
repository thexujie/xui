#pragma once
#include "ui/Control.h"

namespace ui::controls
{
    class Text : public Control
    {
    public:
        Text();
        Text(std::string text);
        ~Text();

        core::si32f contentSize() const override;

        void render(drawing::Graphics & graphics, const drawing::Region & region) const override;
    private:
        void _confirmBlob() const;

    private:
        std::string _text;
        mutable std::shared_ptr<drawing::TextBlob> _textBlob;
    };
}
