#include "stdafx.h"
#include "Spacer.h"

namespace ui::controls
{
    Spacer::Spacer()
    {
		_mouse_through = true;
    }

    Spacer::~Spacer()
    {
        
    }

    void Spacer::paint(drawing::Graphics & graphics, const core::rectf & clip) const
    {
    }
}