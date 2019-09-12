#pragma once

#include "drawing.h"

namespace drawing
{
	class Text;
	
	struct StringFormat
	{
		StringFormat() = default;
		StringFormat(const font & font_) : font(font_) {}

		drawing::font font;
		core::color color = core::colors::Black;
		core::aligns align = core::align::leftTop;
		bool lcd = true;
		bool hint = true;

		StringFormat & _font(const drawing::font & font_)
		{
			font = font_;
			return *this;
		}

		StringFormat & _color(core::color color_)
		{
			color = color_;
			return *this;
		}

		StringFormat & _lcd(bool lcd_)
		{
			lcd = lcd_;
			return *this;
		}

		StringFormat & _align(core::align align_)
		{
			align = align_;
			return *this;
		}

		StringFormat & _hint(bool hint_)
		{
			hint = hint_;
			return *this;
		}
	};

	struct PathFormat
	{
		PathFormat() = default;
		PathFormat(path_mode mode_) :path_mode(mode_) { }

		bool operator == (const PathFormat & another) const
		{
			return path_mode == another.path_mode &&
				stroke_style == another.stroke_style &&
				std::abs(width - another.width) < std::numeric_limits<float32_t>::epsilon() &&
				color == another.color &&
				aa == another.aa;
		}

		bool operator != (const PathFormat & another) const { return !operator==(another); }

		path_mode path_mode = path_mode::stroke;
		blend_mode blend_mode = blend_mode::src_over;
		stroke_style stroke_style = stroke_style::none;
		join_style join_style = join_style::miter;
		float32_t width = 1.0f;
		float32_t miter = std::nanf("0");
		core::color color = core::colors::Transparent;
		bool aa = true;

		PathFormat & _path_mode(drawing::path_mode path_mode_)
		{
			path_mode = path_mode_;
			return *this;
		}

		PathFormat & _blend_mode(drawing::blend_mode blend_mode_)
		{
			blend_mode = blend_mode_;
			return *this;
		}

		PathFormat & _width(float32_t width_)
		{
			width = width_;
			return *this;
		}

		PathFormat & _fill(core::color color_)
		{
			path_mode = drawing::path_mode::fill;
			color = color_;
			return *this;
		}

		PathFormat & _stoke(core::color color_, float32_t width_ = 1.0f, drawing::stroke_style stroke_style_ = stroke_style::solid)
		{
			path_mode = drawing::path_mode::stroke;
			stroke_style = stroke_style_;
			color = color_;
			width = width_;
			return *this;
		}

		PathFormat & _aa(bool aa_)
		{
			aa = aa_;
			return *this;
		}

		PathFormat & _miter(float32_t miter_)
		{
			miter = miter_;
			return *this;
		}

		PathFormat & _join(drawing::join_style join_style_)
		{
			join_style = join_style_;
			return *this;
		}
	};

	struct ImageFormat
	{
		ImageFormat() {}
		ImageFormat(drawing::blend_mode blend_mode_) : blend_mode(blend_mode_){}

		drawing::blend_mode blend_mode = drawing::blend_mode::src_over;

		ImageFormat & _blend_mode(drawing::blend_mode blend_mode_)
		{
			blend_mode = blend_mode_;
			return *this;
		}

	};
	
	class Path
	{
	public:
		virtual ~Path() = default;
		virtual void update(const drawing::path_source & path) = 0;
		virtual void clear() = 0;
		virtual void close() = 0;
		virtual void move(const core::pointf & pos) = 0;
		virtual void line(const core::pointf & pos) = 0;
		virtual void arc(const core::pointf & pos0, const core::pointf & po1, float32_t radius) = 0;
	};

	class ColorFilter
	{
	public:
		virtual ~ColorFilter() = default;
	};

	class Image
	{
	public:
		virtual ~Image() = default;
		virtual void load(const std::u8string_view & path) = 0;
		virtual int32_t width() const = 0;
		virtual int32_t height() const = 0;
		virtual core::sizei size() const = 0;
		virtual float32_t aspect() const = 0;
	};

	class Graphics
	{
	public:
		Graphics() = default;
		virtual ~Graphics() = default;

	public:
		virtual void clear(core::color color) = 0;
		virtual void save() = 0;
		virtual void saveLayer(const core::rectf & bounds, uint8_t alpha) = 0;
		virtual void restore() = 0;
		virtual void setClipRect(const core::rectf & rect, bool aa = true) = 0;
		virtual void setClipPath(const drawing::Path & path, bool aa = true) = 0;
		virtual void setMatrix(const core::float3x2 & matrix) = 0;
		virtual void resetMatrix() = 0;
		virtual void setColorFilter(const drawing::ColorFilter & colorFilter) = 0;

		virtual void drawLine(core::pointf start, core::pointf end, const PathFormat & style) = 0;

		virtual void drawEllipse(const core::rectf & ellipse, const PathFormat & style) = 0;
		virtual void drawRectangle(const core::rectf & rect, const PathFormat & style) = 0;
		virtual void drawRoundRect(const core::rectf & rect, float32_t rx, float32_t ry, const PathFormat & style) = 0;

		virtual void drawPoints(const core::pointf points[], size_t count, point_mode mode, const PathFormat & style) = 0;
		virtual void drawPath(const Path & path, const PathFormat & style) = 0;
		virtual void drawPath(const std::shared_ptr<drawing::Path> & path, const PathFormat & style) = 0;

		virtual void drawString(const std::u8string & str, const core::pointf & pos, const StringFormat & format) = 0;
		virtual void drawString(const std::u8string & str, const core::rectf & rect, const StringFormat & format) = 0;
		virtual void drawText(const drawing::Text & text, core::pointf point, const StringFormat & format) = 0;

		virtual void drawImage(const Image & image, core::pointf point, core::aligns align, const ImageFormat & format = {}) = 0;
		virtual void drawImage(const Image & image, core::rectf rect, core::aligns align, const ImageFormat & format = {}) = 0;
		virtual void drawImage(const Image & image, core::pointf point, core::recti region, core::aligns align, const ImageFormat & format = {}) = 0;
		virtual void drawImage(const Image & image, core::pointf point, core::rectf region, core::aligns align, const ImageFormat & format = {}) = 0;
		virtual void drawImage(const Image & image, core::rectf rect, core::recti region, core::aligns align, const ImageFormat & format = {}) = 0;
		virtual void drawImage(const Image & image, core::rectf rect, core::rectf region, core::aligns align, const ImageFormat & format = {}) = 0;

		virtual void drawImage(const Image & image, core::pointf point, const ImageFormat & format = {}) = 0;
		virtual void drawImage(const Image & image, core::rectf rect, const ImageFormat & format = {}) = 0;
		virtual void drawImage(const Image & image, core::rectf rect, core::recti region, const ImageFormat & format = {}) = 0;
		virtual void drawImage(const Image & image, core::rectf rect, core::rectf region, const ImageFormat & format = {}) = 0;
	};
}
