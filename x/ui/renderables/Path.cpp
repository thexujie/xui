#include "stdafx.h"
#include "Path.h"

namespace ui::renderables
{
    void Path::render(drawing::Graphics & graphics) const
    {
        Renderable::render(graphics);
        graphics.setClipPath(_pathClip);
        graphics.drawPath(_path, _style);
    }
}
