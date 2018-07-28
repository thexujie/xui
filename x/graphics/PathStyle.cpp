#include "stdafx.h"
#include "PathStyle.h"
#include <SkPaint.h>
#include "skia/skia.h"

namespace graphics
{
    void PathStyle::apply(SkPaint & paint) const
    {
        paint.setColor(_color);
        paint.setStrokeWidth(_width);
        paint.setStyle(skia::from(_mode));
        paint.setAntiAlias(_aa);
    }
}
