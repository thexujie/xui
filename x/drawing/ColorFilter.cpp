#include "stdafx.h"
#include "ColorFilter.h"
#include <SkBlendMode.h>
#include <SkColorFilter.h>
#include "skia/skia.h"

namespace drawing
{
    std::shared_ptr<ColorFilter> ColorFilter::createBlendMode(core::color color, blend_mode mode)
    {
        sk_sp<SkColorFilter> filter = SkColorFilter::MakeModeFilter(color, skia::from(mode));
        return std::make_shared<ColorFilter>(skia::skia_shared(filter));
    }
}
