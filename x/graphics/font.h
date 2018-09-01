#pragma once
#include "core/string.h"

namespace graphics
{
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

    struct font
    {
        font();
        font(const char * family_, float_t size_= 0, font_weight weight_ = font_weight::normal, font_width width_ = font_width::normal, font_slant slant_ = font_slant::upright);

        font(const font & another) = default;

        font & operator =(const font & another)
        {
            family = another.family;
            size = another.size;
            weight = another.weight;
            width = another.width;
            slant = another.slant;
            return *this;
        }

        bool operator ==(const font & another) const
        {
            return core::equal_ic(family, another.family) &&
                core::equal(size, another.size) &&
                weight == another.weight &&
                width == another.width &&
                slant == another.slant;
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
    };

    struct fontmetrics
    {
        fontmetrics() = default;
        fontmetrics(const font & font);

        float32_t height = 0;
        float32_t top = 0;
        float32_t bottom = 0;
        float32_t ascent = 0;
        float32_t descent = 0;
        float32_t leading = 0;
        float32_t weight = 0;
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
            size_t h3 = std::hash<graphics::font_weight>()(font.weight);
            size_t h4 = std::hash<graphics::font_width>()(font.width);
            size_t h5 = std::hash<graphics::font_slant>()(font.slant);
            return h1 ^ (h2 << 1) ^ (h3 << 1) ^ (h4 << 1) ^ (h5 << 1);
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
                lhs.width < rhs.width ||
                lhs.slant < rhs.slant;
        }
    };
}
