#pragma once

#include <SkSize.h>
#include <SkBitmap.h>
#include <SkStream.h>
#include <SkImage.h>
#include <SkCanvas.h>
#include <SkSurface.h>
#include <SkString.h>
#include <SkTypeface.h>
#include <SkPaint.h>
#include <SkDashPathEffect.h>
#include <SkTextBlob.h>
#include <SkFontMgr.h>
#include <SkPath.h>

namespace script {
	struct range;
}

namespace skia
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
		switch (fmt)
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

	inline core::sizei & to(SkISize & sksize) { return *(core::sizei *) & sksize; }
	inline const core::sizei & to(const SkISize & sksize) { return *(const core::sizei *) & sksize; }
	inline SkPoint from(const core::pointf & pos)
	{
		return { pos.x, pos.y };
	}
	inline SkPaint::Style from(drawing::path_mode style)
	{
		if (style == drawing::path_mode::stroke)
			return SkPaint::kStroke_Style;
		if (style == drawing::path_mode::fill)
			return SkPaint::kFill_Style;
		//if (style == drawing::path_mode::strokeFill)
		return SkPaint::kStrokeAndFill_Style;
	}

	inline SkMatrix from(const core::float3x2 & matrix)
	{
		return SkMatrix::MakeAll(matrix.sx, matrix.shx, matrix.tx, matrix.shy, matrix.sy, matrix.ty, 0, 0, 1);
	}

	SkFontStyle from(const drawing::font_style & style);
	SkFontStyle from(drawing::font_weight weight_ = drawing::font_weight::normal, drawing::font_width width_ = drawing::font_width::normal, drawing::font_slant slant_ = drawing::font_slant::upright);
	void fontmetrics(const drawing::font & font, drawing::fontmetrics & metrics);
	drawing::fontmetrics fontmetrics(const drawing::font & font);

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

	inline SkBlendMode from(drawing::blend_mode mode)
	{
		switch (mode)
		{
		case drawing::blend_mode::clear: return SkBlendMode::kClear;
		case drawing::blend_mode::src: return SkBlendMode::kSrc;
		case drawing::blend_mode::dst: return SkBlendMode::kDst;
		case drawing::blend_mode::src_over: return SkBlendMode::kSrcOver;
		case drawing::blend_mode::dst_over: return SkBlendMode::kDstOver;
		case drawing::blend_mode::src_in: return SkBlendMode::kSrcIn;
		case drawing::blend_mode::dst_in: return SkBlendMode::kDstIn;
		case drawing::blend_mode::src_out: return SkBlendMode::kSrcOut;
		case drawing::blend_mode::dst_out: return SkBlendMode::kDstOut;
		case drawing::blend_mode::src_atop: return SkBlendMode::kSrcATop;
		case drawing::blend_mode::dst_atop: return SkBlendMode::kDstATop;
		case drawing::blend_mode:: xor : return SkBlendMode::kXor;
		case drawing::blend_mode::plus: return SkBlendMode::kPlus;
		case drawing::blend_mode::overlay: return SkBlendMode::kOverlay;
		case drawing::blend_mode::darken: return SkBlendMode::kDarken;
		case drawing::blend_mode::lighten: return SkBlendMode::kLighten;
		case drawing::blend_mode::dodge: return SkBlendMode::kColorDodge;
		case drawing::blend_mode::burn: return SkBlendMode::kColorBurn;
		case drawing::blend_mode::hard_light: return SkBlendMode::kHardLight;
		case drawing::blend_mode::soft_light: return SkBlendMode::kSoftLight;
		case drawing::blend_mode::difference: return SkBlendMode::kDifference;
		case drawing::blend_mode::exclusion: return SkBlendMode::kExclusion;
		case drawing::blend_mode::multiply: return SkBlendMode::kMultiply;
		case drawing::blend_mode::hue: return SkBlendMode::kHue;
		case drawing::blend_mode::saturation: return SkBlendMode::kSaturation;
		case drawing::blend_mode::color: return SkBlendMode::kColor;
		case drawing::blend_mode::luminosity: return SkBlendMode::kLuminosity;
		default: return SkBlendMode::kSrc;
		}
	}

	inline drawing::blend_mode to(SkBlendMode mode)
	{
		switch (mode)
		{
		case SkBlendMode::kClear: return drawing::blend_mode::clear;
		case SkBlendMode::kSrc: return drawing::blend_mode::src;
		case SkBlendMode::kDst: return drawing::blend_mode::dst;
		case SkBlendMode::kSrcOver: return drawing::blend_mode::src_over;
		case SkBlendMode::kDstOver: return drawing::blend_mode::dst_over;
		case SkBlendMode::kSrcIn: return drawing::blend_mode::src_in;
		case SkBlendMode::kDstIn: return drawing::blend_mode::dst_in;
		case SkBlendMode::kSrcOut: return drawing::blend_mode::src_out;
		case SkBlendMode::kDstOut: return drawing::blend_mode::dst_out;
		case SkBlendMode::kSrcATop: return drawing::blend_mode::src_atop;
		case SkBlendMode::kDstATop: return drawing::blend_mode::dst_atop;
		case SkBlendMode::kXor: return drawing::blend_mode:: xor;
		case SkBlendMode::kPlus: return drawing::blend_mode::plus;
		case SkBlendMode::kOverlay: return drawing::blend_mode::overlay;
		case SkBlendMode::kDarken: return drawing::blend_mode::darken;
		case SkBlendMode::kLighten: return drawing::blend_mode::lighten;
		case SkBlendMode::kColorDodge: return drawing::blend_mode::dodge;
		case SkBlendMode::kColorBurn: return drawing::blend_mode::burn;
		case SkBlendMode::kHardLight: return drawing::blend_mode::hard_light;
		case SkBlendMode::kSoftLight: return drawing::blend_mode::soft_light;
		case SkBlendMode::kDifference: return drawing::blend_mode::difference;
		case SkBlendMode::kExclusion: return drawing::blend_mode::exclusion;
		case SkBlendMode::kMultiply: return drawing::blend_mode::multiply;
		case SkBlendMode::kHue: return drawing::blend_mode::hue;
		case SkBlendMode::kSaturation: return drawing::blend_mode::saturation;
		case SkBlendMode::kColor: return drawing::blend_mode::color;
		case SkBlendMode::kLuminosity: return drawing::blend_mode::luminosity;
		default: return drawing::blend_mode::src;
		}
	}

	inline SkCanvas::PointMode from(drawing::point_mode mode)
	{
		switch (mode)
		{
		case drawing::point_mode::points: return SkCanvas::PointMode::kPoints_PointMode;
		case drawing::point_mode::lines: return SkCanvas::PointMode::kLines_PointMode;
		case drawing::point_mode::polygon: return SkCanvas::PointMode::kPolygon_PointMode;
		default: return SkCanvas::PointMode::kPoints_PointMode;
		}
	}

	inline SkPaint::Join from(drawing::join_style style)
	{
		switch (style)
		{
		case drawing::join_style::miter: return SkPaint::Join::kMiter_Join;
		case drawing::join_style::round: return SkPaint::Join::kRound_Join;
		case drawing::join_style::bevel: return SkPaint::Join::kBevel_Join;
		default: return SkPaint::Join::kMiter_Join;
		}
	}

	inline void apply_imageformat(SkPaint & paint, const drawing::ImageFormat & format)
	{
		paint.setAntiAlias(true);
		paint.setBlendMode(skia::from(format.blend_mode));
	}


	inline void apply_pathformat(SkPaint & paint, const drawing::PathFormat & format)
	{
		paint.setColor(format.color);
		paint.setStrokeWidth(format.width);
		paint.setStyle(skia::from(format.path_mode));
		paint.setBlendMode(skia::from(format.blend_mode));
		paint.setStrokeJoin(skia::from(format.join_style));
		if (!std::isnan(format.miter))
			paint.setStrokeMiter(format.miter);
		if (format.path_mode == drawing::path_mode::stroke)
		{
			if (format.stroke_style == drawing::stroke_style::dashed)
			{
				SkScalar intervals[] = { format.width, format.width };
				paint.setPathEffect(SkDashPathEffect::Make(intervals, SK_ARRAY_COUNT(intervals), 0));
			}
		}
		paint.setAntiAlias(format.aa);
	}

	inline void apply_stringformat(SkPaint & paint, const drawing::StringFormat & format)
	{
		paint.setAntiAlias(true);
		paint.setLCDRenderText(format.lcd);
		paint.setColor(format.color);
		paint.setTextSize(format.font.size);
		paint.setAutohinted(format.hint);
	}
}

