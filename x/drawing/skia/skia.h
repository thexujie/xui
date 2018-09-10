#pragma once

#include "core/core.h"

#include <SkSize.h>
#include <SkBitmap.h>
#include <SkStream.h>
#include <SkImage.h>
#include <SkCanvas.h>
#include <SkString.h>
#include <SkTypeface.h>
#include "drawing/image/image.h"
#include "drawing/PathStyle.h"
#include "drawing/font.h"

namespace drawing::skia
{
    template<typename T = SkRefCnt>
    inline void skia_unref(T * ptr)
    {
        if (ptr)
            ptr->unref();
    }

    // from : normal struct convert to Skia struct
    // to : Skia struct convert to normal struct
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

    inline SkIRect from(const core::rc32i & rect)
    {
        return SkIRect::MakeXYWH(rect.x, rect.y, rect.width, rect.height);
    }
    inline SkRect from(const core::rc32f & rect)
    {
        return SkRect::MakeXYWH(rect.x, rect.y, rect.width, rect.height);
    }
    inline core::rc32i to(const SkIRect & rect)
    {
        return core::rc32i(rect.x(), rect.y(), rect.width(), rect.height());
    }
    inline core::rc32f to(const SkRect & rect)
    {
        return core::rc32f(rect.x(), rect.y(), rect.width(), rect.height());
    }

    inline SkEncodedImageFormat from(image::image_type fmt)
    {
        switch (fmt)
        {
        case image::image_type_bmp: return SkEncodedImageFormat::kBMP;
        case image::image_type_png: return SkEncodedImageFormat::kPNG;
        case image::image_type_jpeg: return SkEncodedImageFormat::kJPEG;
        default: return SkEncodedImageFormat::kBMP;
        }
    }

    inline core::si32i & to(SkISize & sksize) { return *(core::si32i *)&sksize; }
    inline const core::si32i & to(const SkISize & sksize) { return *(const core::si32i *)&sksize; }
    inline SkPoint from(const core::pt32f & pos)
    {
        return { pos.x, pos.y };
    }
    inline SkPaint::Style from(PathMode style)
    {
        if (style == PathMode::Stroke)
            return SkPaint::kStroke_Style;
        if (style == PathMode::Fill)
            return SkPaint::kFill_Style;
        //if (style == PathMode::StrokeFill)
        return SkPaint::kStrokeAndFill_Style;
    }

    inline SkMatrix from(const core::float3x2 & matrix)
    {
        return SkMatrix::MakeAll(matrix.sx, matrix.shx, matrix.tx, matrix.shy, matrix.sy, matrix.ty, 0, 0, 1);
    }

    SkFontStyle from(const drawing::font_style & style);
    SkFontStyle from(font_weight weight_ = font_weight::normal, font_width width_ = font_width::normal, font_slant slant_ = font_slant::upright);
    void fontmetrics(const drawing::font & font, drawing::fontmetrics & metrics);

    inline drawing::font_style to(const SkFontStyle & style)
    {
        return drawing::font_style((drawing::font_weight)style.weight(), (drawing::font_width)style.width(), (drawing::font_slant)style.slant());
    }

    inline drawing::font to(const SkTypeface & tf, float32_t size)
    {
        SkString family;
        tf.getFamilyName(&family);
        return drawing::font(family.c_str(), size, to(tf.fontStyle()));
    }

}
