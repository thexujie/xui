#pragma once
#include "core/string.h"

namespace graphics
{
    namespace text
    {
        const int32_t FONT_WEIGHT_BOLD = 800;
        const int32_t FONT_WEIGHT_NORMAL = 400;

        struct font
        {
            font() = default;
            font(const char * _family, int32_t _size = 0, int32_t _weight = FONT_WEIGHT_NORMAL)
            {
                family = _family;
                size = _size;
                weight = _weight;
            }
            font(const font & another) = default;

            font & operator = (const font & another)
            {
                family = another.family;
                size = another.size;
                weight = another.weight;
                italic = another.italic;
                underline = another.underline;
                strikeout = another.strikeout;
                return *this;
            }

            bool operator == (const font & another) const
            {
                return core::string::equal_ic()(family, another.family) &&
                size == another.size &&
                weight == another.weight &&
                italic == another.italic &&
                underline == another.underline &&
                strikeout == another.strikeout;
            }

            bool operator != (const font & another) const
            {
                return !operator==(another);
            }

            std::string family;
            int32_t size = 0;
            int32_t weight = 0;
            bool italic = false;
            bool underline = false;
            bool strikeout = false;
        };
    }
}

