#include "stdafx.h"
#include "PathStyle.h"
#include <SkPaint.h>
#include "skia/skia.h"
#include <SkDashPathEffect.h>

namespace graphics
{
    void PathStyle::apply(SkPaint & paint) const
    {
        paint.setColor(_color);
        paint.setStrokeWidth(_width);
        paint.setStyle(skia::from(_mode));
        if(_mode == PathMode::Stroke)
        {
            if (_stroke_style == stroke_style::dashed)
            {
                SkScalar intervals[] = { _width, _width };
                paint.setPathEffect(SkDashPathEffect::Make(intervals, SK_ARRAY_COUNT(intervals), 0));
            }
        }
        paint.setAntiAlias(_aa);
    }
}
