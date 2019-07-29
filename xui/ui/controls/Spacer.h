#pragma once
#include "ui/Control.h"

namespace ui::controls
{
    class Spacer : public Control
    {
    public:
        Spacer();
        ~Spacer();

        void paint(drawing::Graphics & graphics, const core::rectf & clip) const override;
    };
}
