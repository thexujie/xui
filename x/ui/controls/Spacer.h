#pragma once
#include "ui/Control.h"

namespace ui::controls
{
    class Spacer : public Control
    {
    public:
        Spacer();
        Spacer(std::string text);
        ~Spacer();

        void draw(drawing::Graphics & graphics, const drawing::Region & region) const override;
    };
}