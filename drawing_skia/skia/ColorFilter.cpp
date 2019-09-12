#include "stdafx.h"
#include "ColorFilter.h"
#include <SkBlendMode.h>
#include <SkColorFilter.h>

namespace skia
{
    std::shared_ptr<ColorFilter> ColorFilter::createBlendMode(core::color color, drawing::blend_mode mode)
    {
        sk_sp<SkColorFilter> filter = SkColorFilter::MakeModeFilter(color, skia::from(mode));
        return std::make_shared<ColorFilter>(skia::skia_shared(filter));
    }

	std::shared_ptr<ColorFilter> ColorFilter::createGrayScale(float32_t brightness, float32_t alpha)
    {
		//redResult = a[0] * srcR + a[1] * srcG + a[2] * srcB + a[3] * srcA + a[4]
		//greenResult = a[5] * srcR + a[6] * srcG + a[7] * srcB + a[8] * srcA + a[9]
		//blueResult = a[10] * srcR + a[11] * srcG + a[12] * srcB + a[13] * srcA + a[14]
		//alphaResult = a[15] * srcR + a[16] * srcG + a[17] * srcB + a[18] * srcA + a[19]
		SkScalar matrix[20];
		memset(matrix, 0, 20 * sizeof(SkScalar));
		matrix[0] = matrix[5] = matrix[10] = 0.2126f * brightness;
		matrix[1] = matrix[6] = matrix[11] = 0.7152f * brightness;
		matrix[2] = matrix[7] = matrix[12] = 0.0722f * brightness;
		matrix[18] = alpha;
		sk_sp<SkColorFilter> filter = SkColorFilter::MakeMatrixFilterRowMajor255(matrix);
		return std::make_shared<ColorFilter>(skia::skia_shared(filter));
    }
}
