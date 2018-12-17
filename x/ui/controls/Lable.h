#pragma once
#include "ui/Control.h"

namespace ui::controls
{
    class Lable : public Control
    {
    public:
        Lable();
        ~Lable();

        void update() override;
        void paint(drawing::Graphics & graphics, const core::rectf & clip) const override;

    public:
        void setText(std::string text);
        const std::string & text() const { return _text.text(); }

    private:
        drawing::Text _text;
    };
}
