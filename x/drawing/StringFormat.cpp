#include "stdafx.h"
#include "StringFormat.h"

#include "SkPaint.h"

namespace drawing
{
    void StringFormat::apply(SkPaint & paint) const
    {
        paint.setAntiAlias(true);
        paint.setLCDRenderText(_lcd);
        paint.setColor(_color);
        paint.setTextSize(_font.size);
        paint.setAutohinted(_hint);
    }
}