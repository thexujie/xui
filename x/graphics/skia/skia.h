#pragma once

#include "core/core.h"

#include <SkSize.h>
#include <SkBitmap.h>
#include <SkStream.h>
#include <SkImage.h>
#include <SkCanvas.h>
#include <SkString.h>
#include <SkTypeface.h>

namespace graphics::skia
{
    inline image::image_type to(SkEncodedImageFormat fmt)
    {
        switch(fmt)
        {
        case SkEncodedImageFormat::kBMP: return image::image_type_bmp;
        case SkEncodedImageFormat::kPNG: return image::image_type_png;
        case SkEncodedImageFormat::kJPEG: return image::image_type_jpeg;
        default: return image::image_type_none;
        }
    }

    inline SkRect from(const core::rc32f & rect)
    {
        return SkRect::MakeXYWH(rect.x, rect.y, rect.width, rect.height);
    }
    inline SkIRect from(const core::rc32i & rect)
    {
        return SkIRect::MakeXYWH(rect.x, rect.y, rect.width, rect.height);
    }

    inline SkEncodedImageFormat to(image::image_type fmt)
    {
        switch (fmt)
        {
        case image::image_type_bmp: return SkEncodedImageFormat::kBMP;
        case image::image_type_png: return SkEncodedImageFormat::kPNG;
        case image::image_type_jpeg: return SkEncodedImageFormat::kJPEG;
        default: return SkEncodedImageFormat::kBMP;
        }
    }

    inline core::si32i & size(SkISize & sksize) { return *(core::si32i *)&sksize; }
    inline const core::si32i & size(const SkISize & sksize) { return *(const core::si32i *)&sksize; }
}
