#pragma once

#include "../drawing.h"

#include <SkSize.h>
#include <SkBitmap.h>
#include <SkStream.h>
#include <SkImage.h>
#include <SkCanvas.h>
#include <SkSurface.h>
#include <SkString.h>
#include <SkTypeface.h>
#include <SkPaint.h>
#include "image/image.h"
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

    template<typename T = SkRefCnt>
    inline std::shared_ptr<T> skia_shared(sk_sp<T> & ss)
    {
        if (!ss)
            return nullptr;
        auto ptr = ss.get();
        SkSafeRef(ptr);
        return std::shared_ptr<T>(ptr, skia_unref<T>);
    }

    // from : normal struct convert to Skia struct
    // to : Skia struct convert to normal struct
    inline core::image_type to(SkEncodedImageFormat fmt)
    {
        switch(fmt)
        {
        case SkEncodedImageFormat::kBMP: return core::image_type_bmp;
        case SkEncodedImageFormat::kPNG: return core::image_type_png;
        case SkEncodedImageFormat::kJPEG: return core::image_type_jpeg;
        default: return core::image_type_none;
        }
    }

    inline SkIRect from(const core::recti & rect)
    {
        return SkIRect::MakeXYWH(rect.x, rect.y, rect.width, rect.height);
    }
    inline SkRect from(const core::rectf & rect)
    {
        return SkRect::MakeXYWH(rect.x, rect.y, rect.width, rect.height);
    }
    inline core::recti to(const SkIRect & rect)
    {
        return core::recti(rect.x(), rect.y(), rect.width(), rect.height());
    }
    inline core::rectf to(const SkRect & rect)
    {
        return core::rectf(rect.x(), rect.y(), rect.width(), rect.height());
    }

    inline SkEncodedImageFormat from(core::image_type type)
    {
        switch (type)
        {
        case core::image_type_bmp: return SkEncodedImageFormat::kBMP;
        case core::image_type_png: return SkEncodedImageFormat::kPNG;
        case core::image_type_jpeg: return SkEncodedImageFormat::kJPEG;
        default: return SkEncodedImageFormat::kBMP;
        }
    }

    inline core::sizei & to(SkISize & sksize) { return *(core::sizei *)&sksize; }
    inline const core::sizei & to(const SkISize & sksize) { return *(const core::sizei *)&sksize; }
    inline SkPoint from(const core::pointf & pos)
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
        return drawing::font(reinterpret_cast<const char8_t *>(family.c_str()), size, to(tf.fontStyle()));
    }

    inline SkBlendMode from(blend_mode mode)
    {
        switch(mode)
        {
        case blend_mode::clear: return SkBlendMode::kClear;
        case blend_mode::src: return SkBlendMode::kSrc;
        case blend_mode::dst: return SkBlendMode::kDst;
        case blend_mode::src_over: return SkBlendMode::kSrcOver;
        case blend_mode::dst_over: return SkBlendMode::kDstOver;
        case blend_mode::src_in: return SkBlendMode::kSrcIn;
        case blend_mode::dst_in: return SkBlendMode::kDstIn;
        case blend_mode::src_out: return SkBlendMode::kSrcOut;
        case blend_mode::dst_out: return SkBlendMode::kDstOut;
        case blend_mode::src_atop: return SkBlendMode::kSrcATop;
        case blend_mode::dst_atop: return SkBlendMode::kDstATop;
        case blend_mode::xor: return SkBlendMode::kXor;
        case blend_mode::plus: return SkBlendMode::kPlus;
		case blend_mode::overlay: return SkBlendMode::kOverlay;
		case blend_mode::darken: return SkBlendMode::kDarken;
		case blend_mode::lighten: return SkBlendMode::kLighten;
		case blend_mode::dodge: return SkBlendMode::kColorDodge;
		case blend_mode::burn: return SkBlendMode::kColorBurn;
		case blend_mode::hard_light: return SkBlendMode::kHardLight;
		case blend_mode::soft_light: return SkBlendMode::kSoftLight;
		case blend_mode::difference: return SkBlendMode::kDifference;
		case blend_mode::exclusion: return SkBlendMode::kExclusion;
		case blend_mode::multiply: return SkBlendMode::kMultiply;
		case blend_mode::hue: return SkBlendMode::kHue;
		case blend_mode::saturation: return SkBlendMode::kSaturation;
		case blend_mode::color: return SkBlendMode::kColor;
		case blend_mode::luminosity: return SkBlendMode::kLuminosity;
		default: return SkBlendMode::kSrc;
		}
	}

    inline blend_mode to(SkBlendMode mode)
    {
        switch (mode)
        {
        case SkBlendMode::kClear: return blend_mode::clear;
        case SkBlendMode::kSrc: return blend_mode::src;
        case SkBlendMode::kDst: return blend_mode::dst;
        case SkBlendMode::kSrcOver: return blend_mode::src_over;
        case SkBlendMode::kDstOver: return blend_mode::dst_over;
        case SkBlendMode::kSrcIn: return blend_mode::src_in;
        case SkBlendMode::kDstIn: return blend_mode::dst_in;
        case SkBlendMode::kSrcOut: return blend_mode::src_out;
        case SkBlendMode::kDstOut: return blend_mode::dst_out;
        case SkBlendMode::kSrcATop: return blend_mode::src_atop;
        case SkBlendMode::kDstATop: return blend_mode::dst_atop;
        case SkBlendMode::kXor: return blend_mode::xor;
        case SkBlendMode::kPlus: return blend_mode::plus;
		case SkBlendMode::kOverlay: return blend_mode::overlay;
		case SkBlendMode::kDarken: return blend_mode::darken;
		case SkBlendMode::kLighten: return blend_mode::lighten;
		case SkBlendMode::kColorDodge: return blend_mode::dodge;
		case SkBlendMode::kColorBurn: return blend_mode::burn;
		case SkBlendMode::kHardLight: return blend_mode::hard_light;
		case SkBlendMode::kSoftLight: return blend_mode::soft_light;
		case SkBlendMode::kDifference: return blend_mode::difference;
		case SkBlendMode::kExclusion: return blend_mode::exclusion;
		case SkBlendMode::kMultiply: return blend_mode::multiply;
		case SkBlendMode::kHue: return blend_mode::hue;
		case SkBlendMode::kSaturation: return blend_mode::saturation;
		case SkBlendMode::kColor: return blend_mode::color;
		case SkBlendMode::kLuminosity: return blend_mode::luminosity;
        default: return blend_mode::src;
        }
    }

    inline SkCanvas::PointMode from(point_mode mode)
    {
        switch (mode)
        {
        case point_mode::points: return SkCanvas::PointMode::kPoints_PointMode;
        case point_mode::lines: return SkCanvas::PointMode::kLines_PointMode;
        case point_mode::polygon: return SkCanvas::PointMode::kPolygon_PointMode;
        default: return SkCanvas::PointMode::kPoints_PointMode;
        }
    }

    inline SkPaint::Join from(join_style style)
    {
        switch (style)
        {
        case join_style::miter: return SkPaint::Join::kMiter_Join;
        case join_style::round: return SkPaint::Join::kRound_Join;
        case join_style::bevel: return SkPaint::Join::kBevel_Join;
        default: return SkPaint::Join::kMiter_Join;
        }
    }
}
