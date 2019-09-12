#include "stdafx.h"
#include "drawing.h"

namespace drawing
{
	void generate_default_font();
	
    font::font()
    {
        *this = default_font();
    }

    font::font(const char8_t * family_, float_t size_, font_style style_)
        : style(style_)
    {
        if (!family_ || !family_[0] || size_ <= 0)
        {
            family = (family_ && family_[0]) ? family_ : default_font().family;
            size = size_ > 0 ? size_ : default_font().size;
        }
        else
        {
            family = family_;
            size = size_;
        }
    }

    font::font(const std::u8string & family_, float_t size_, font_style style_)
        : style(style_)
    {
        if (family_.empty() || size_ <= 0)
        {
            family = family_.empty() ? default_font().family : family_;
            size = size_ > 0 ? size_ : default_font().size;
        }
        else
        {
            family = family_;
            size = size_;
        }
    }

	stroke_style parseStrokeStyle(const std::u8string_view & str)
	{
		for (const auto & name : stroke_style_names)
		{
			if (core::equal_ic(str, name.name, -1))
				return name.style;
		}
		return stroke_style::none;
	}

	core::vec2<stroke_style> parseStrokeStyle2D(const std::u8string_view & str)
	{
		std::vector<std::u8string_view> strs = core::split(str, u8' ');
		if (strs.size() == 1)
			return core::vec2<stroke_style>{ parseStrokeStyle(strs[0]) };
		if (strs.size() == 2)
			return { parseStrokeStyle(strs[0]), parseStrokeStyle(strs[1]) };
		return {};
	}

	core::vec4<stroke_style> parseStrokeStyle4D(const std::u8string_view & str)
	{
		std::vector<std::u8string_view> strs = core::split(str, u8' ');
		if (strs.size() == 1)
			return core::vec4<stroke_style>{ parseStrokeStyle(strs[0]) };
		if (strs.size() == 2)
			return { parseStrokeStyle(strs[0]), parseStrokeStyle(strs[1]) };
		if (strs.size() == 4)
			return { parseStrokeStyle(strs[0]), parseStrokeStyle(strs[1]), parseStrokeStyle(strs[2]), parseStrokeStyle(strs[3]) };
		return {};
	}
}
