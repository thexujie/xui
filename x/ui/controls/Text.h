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

        core::sizef contentSize() const override;

        void paint(drawing::Graphics & graphics, const core::rectf & clip) const override;

    public:
        void setText(std::string text);
        const std::string & text() const { return _text; }
    private:
        void _confirmBlob() const;

    private:
        std::string _text;
        mutable std::shared_ptr<drawing::TextBlob> _textBlob;
    };
}
