#pragma once
#include "core/string.h"

namespace graphics
{
    struct fontmetrics
    {
        int32_t size = 0;
        int32_t height = 0;
        int32_t ascent = 0;
        int32_t descent = 0;
        int32_t linespace = 0;
        int32_t weight = 0;
    };

    enum font_weight : uint16_t
    {
        Weight_Invisible = 0,
        Weight_Thin = 100,
        Weight_ExtraLight = 200,
        Weight_Light = 300,
        Weight_Normal = 400,
        Weight_Medium = 500,
        Weight_SemiBold = 600,
        Weight_Bold = 700,
        Weight_ExtraBold = 800,
        Weight_BlackBold = 900,
        Weight_ExtraBlackBold = 1000,
    };

    enum font_width : uint8_t
    {
        Width_UltraCondensed = 1,
        Width_ExtraCondensed = 2,
        Width_Condensed = 3,
        Width_SemiCondensed = 4,
        Width_Normal = 5,
        Width_SemiExpanded = 6,
        Width_Expanded = 7,
        Width_ExtraExpanded = 8,
        Width_UltraExpanded = 9,
    };

    enum font_slant : uint8_t
    {
        Slant_Upright = 0,
        Slant_Italic,
        Slant_Oblique,
    };

    struct font
    {
        font();
        font(const char * _family, float_t _size = 0, int32_t _weight = Weight_Normal, int32_t _width = Width_Normal, int32_t _slant = Slant_Upright, int32_t flags_ = 0);

        font(const font & another) = default;

        font & operator =(const font & another)
        {
            family = another.family;
            size = another.size;
            weight = another.weight;
            width = another.width;
            slant = another.slant;
            flags = another.flags;
            return *this;
        }

        bool operator ==(const font & another) const
        {
            return core::string::equal_ic()(family, another.family) &&
                std::fabs(size - another.size) <  std::numeric_limits<float32_t>::epsilon() &&
                weight == another.weight &&
                width == another.width &&
                slant == another.slant &&
                flags == another.flags;
        }

        bool operator !=(const font & another) const
        {
            return !operator==(another);
        }

        std::string family;
        float_t size = 0;
        font_weight weight : 16;
        font_width width : 4;
        font_slant slant : 4;
        int32_t flags = 0;
    };
}

namespace std
{
    template<>
    struct hash<graphics::font>
    {
    public:
        size_t operator()(const graphics::font & font) const
        {
            size_t h1 = std::hash<std::string>()(font.family);
            size_t h2 = std::hash<float32_t>()(font.size);
            size_t h3 = std::hash<int32_t>()(font.weight);
            size_t h4 = std::hash<int32_t>()(font.flags);
            return h1 ^ (h2 << 1) ^ (h3 << 1) ^ (h4 << 1);
        }
    };

    template<>
    struct less<graphics::font>
    {
        constexpr bool operator()(const graphics::font & lhs, const graphics::font & rhs) const
        {
            return lhs.family < rhs.family ||
                lhs.size < rhs.size ||
                lhs.weight < rhs.weight ||
                lhs.flags < rhs.flags;
        }
    };
}
