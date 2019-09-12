#pragma once

#include "core/core.h"

namespace drawing
{
	enum class point_mode
	{
		points,
		lines,
		polygon,
	};

	enum class blend_mode
	{
		clear,
		src,
		dst,
		src_over,
		dst_over,
		src_in,
		dst_in,
		src_out,
		dst_out,
		src_atop,
		dst_atop,
		xor,
		plus,
		overlay,

		darken,
		lighten,

		dodge,
		burn,

		hard_light,
		soft_light,

		difference,
		exclusion,
		multiply,

		hue,
		saturation,
		color,
		luminosity,
	};

	enum class path_mode
	{
		stroke = 0,
		fill,
		strokeFill,
	};

	enum class stroke_style
	{
		none = 0,
		hidden,
		dotted,
		dashed,
		solid,
		double_,
		groove,
		ridge,
		inset,
		outset,
		inherit,
	};

	enum class join_style
	{
		miter = 0,
		round,
		bevel,
	};

	enum class wrap_mode
	{
		none = 0,
		word,
		charactor,
	};

	enum class font_weight : uint16_t
	{
		invisible = 0,
		thin = 100,
		extralight = 200,
		light = 300,
		normal = 400,
		medium = 500,
		semibold = 600,
		bold = 700,
		extrabold = 800,
		blackbold = 900,
		extrablackbold = 1000,
	};

	enum class font_width : uint8_t
	{
		ultracondensed = 1,
		extracondensed = 2,
		condensed = 3,
		semicondensed = 4,
		normal = 5,
		semiexpanded = 6,
		expanded = 7,
		extraexpanded = 8,
		ultraexpanded = 9,
	};

	enum class font_slant : uint8_t
	{
		upright = 0,
		italic,
		oblique,
	};
	
	struct bitmap_buffer
	{
		byte_t * data = nullptr;
		core::sizei size;
		int32_t strike = 0;
		int32_t pitch = 0;
		bool flip_y = false;
	};

	struct font_style
	{
		font_style() = default;
		font_style(font_weight weight_, font_width width_, font_slant slant_) :weight(weight_), width(width_), slant(slant_) {}
		font_style(const font_style & another) = default;

		font_style & operator =(const font_style & another)
		{
			weight = another.weight;
			width = another.width;
			slant = another.slant;
			return *this;
		}

		bool operator ==(const font_style & another) const
		{
			return weight == another.weight &&
				width == another.width &&
				slant == another.slant;
		}

		bool operator !=(const font_style & another) const
		{
			return !operator==(another);
		}

		font_weight weight = font_weight::normal;
		font_width width = font_width::normal;
		font_slant slant = font_slant::upright;
	};

	struct font
	{
		font();
		font(const char8_t * family_, float_t size_ = 0, font_style style_ = font_style());
		font(const std::u8string & family_, float_t size_ = 0, font_style style_ = font_style());
		font(const font & another) = default;

		font & operator =(const font & another)
		{
			family = another.family;
			size = another.size;
			style = another.style;
			return *this;
		}

		bool operator ==(const font & another) const
		{
			return core::equal_ic(family, another.family) &&
				core::equal(size, another.size) &&
				style == another.style;
		}

		bool operator !=(const font & another) const
		{
			return !operator==(another);
		}

		std::u8string family;
		float_t size = 0;
		font_style style;
	};

	struct fontmetrics
	{
		float32_t height = 0;
		float32_t top = 0;
		float32_t bottom = 0;
		float32_t ascent = 0;
		float32_t descent = 0;
		float32_t leading = 0;
		float32_t weight = 0;
		float32_t width = 0;

		fontmetrics() = default;
	};

	const font & default_font();

	struct stroke_style_name
	{
		stroke_style style;
		const char8_t * name = nullptr;
	};

	const stroke_style_name stroke_style_names[] =
	{
		{ stroke_style::none, u8"none" },
	{ stroke_style::hidden, u8"hidden" },
	{ stroke_style::dotted, u8"dotted" },
	{ stroke_style::dashed, u8"dashed" },
	{ stroke_style::solid, u8"solid" },
	{ stroke_style::double_, u8"double" },
	{ stroke_style::groove, u8"groove" },
	{ stroke_style::ridge, u8"ridge" },
	{ stroke_style::inset, u8"inset" },
	{ stroke_style::outset, u8"outset" },
	{ stroke_style::inherit, u8"inherit" },
	};

	stroke_style parseStrokeStyle(const std::u8string_view & str);
	core::vec2<stroke_style> parseStrokeStyle2D(const std::u8string_view & str);
	core::vec4<stroke_style> parseStrokeStyle4D(const std::u8string_view & str);
}


namespace std
{
	template<>
	struct hash<drawing::font>
	{
	public:
		size_t operator()(const drawing::font & font) const
		{
			size_t h1 = std::hash<std::u8string>()(font.family);
			size_t h2 = std::hash<float32_t>()(font.size);
			size_t h3 = std::hash<drawing::font_weight>()(font.style.weight);
			size_t h4 = std::hash<drawing::font_width>()(font.style.width);
			size_t h5 = std::hash<drawing::font_slant>()(font.style.slant);
			return h1 ^ (h2 << 1) ^ (h3 << 1) ^ (h4 << 1) ^ (h5 << 1);
		}
	};

	template<>
	struct less<drawing::font>
	{
		bool operator()(const drawing::font & lhs, const drawing::font & rhs) const
		{
			return lhs.family < rhs.family ||
				lhs.size < rhs.size ||
				lhs.style.weight < rhs.style.weight ||
				lhs.style.width < rhs.style.width ||
				lhs.style.slant < rhs.style.slant;
		}
	};
}
