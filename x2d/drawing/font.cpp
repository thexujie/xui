#include "stdafx.h"
#include "font.h"


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

    fontmetrics::fontmetrics(const font & font)
    {
		font_metrics(font, *this);
    }

    void fontmetrics::from(const font & font)
    {
		font_metrics(font, *this);
    }
}
