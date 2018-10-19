#include "stdafx.h"
#include "Spacer.h"

namespace ui::controls
{
    Spacer::Spacer()
    {
        
    }

    Spacer::~Spacer()
    {
        
    }

    void Spacer::draw(drawing::Graphics & graphics, const core::rc32f & clip) const
    {
        std::lock_guard l(*this);
        _drawBackground(graphics);
        _drawBorder(graphics);
    }
}
