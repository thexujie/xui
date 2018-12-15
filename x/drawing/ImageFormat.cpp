#include "stdafx.h"
#include "ImageFormat.h"

#include "SkPaint.h"
#include "skia/skia.h"

namespace drawing
{
    void ImageFormat::apply(SkPaint & paint) const
    {
        paint.setAntiAlias(true);
        paint.setBlendMode(drawing::skia::from(_blend_mode));
    }
}
