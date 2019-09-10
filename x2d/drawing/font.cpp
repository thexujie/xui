#include "stdafx.h"
#include "font.h"

static drawing::font __default_font;
static drawing::fontmetrics __default_fontmetrics;
static std::once_flag __default_font_once_flag;

void generate_default_font()
{
	__default_font = platform::default_font();
    __default_fontmetrics.from(__default_font);
}

namespace drawing
{
    font::font()
    {
        std::call_once(__default_font_once_flag, generate_default_font);
        *this = platform::default_font();
    }

    font::font(const char8_t * family_, float_t size_, font_style style_)
        : style(style_)
    {
        if (!family_ || !family_[0] || size_ <= 0)
        {
            std::call_once(__default_font_once_flag, generate_default_font);
            family = (family_ && family_[0]) ? family_ : __default_font.family;
            size = size_ > 0 ? size_ : __default_font.size;
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
            std::call_once(__default_font_once_flag, generate_default_font);
            family = family_.empty() ? __default_font.family : family_;
            size = size_ > 0 ? size_ : __default_font.size;
        }
        else
        {
            family = family_;
            size = size_;
        }
    }

    fontmetrics::fontmetrics(const font & font)
    {
        platform::font_metrics(font, *this);
    }

    void fontmetrics::from(const font & font)
    {
		platform::font_metrics(font, *this);
    }

    const font & default_font()
    {
        std::call_once(__default_font_once_flag, generate_default_font);
        return __default_font;
    }

    const fontmetrics & default_fontmetrics()
    {
        std::call_once(__default_font_once_flag, generate_default_font);
        return __default_fontmetrics;
    }
}
