#pragma once
#include "core/string.h"

namespace graphics::text
{
    const int32_t FONT_WEIGHT_BOLD = 800;
    const int32_t FONT_WEIGHT_NORMAL = 400;

    enum font_level
    {
        system = 0,
        gray,
        cleartype,
    };

    struct font
    {
        enum flag_e
        {
            italic = 0x0001,
            underline = 0x0002,

            gray = 0x0010,
            anti = 0x0020,
            cleartype = 0x0040,
        };

        font() = default;

        font(const char * _family, int32_t _size = 0, int32_t _weight = FONT_WEIGHT_NORMAL, int32_t _flags = 0)
        {
            family = _family;
            size = _size;
            weight = _weight > 0 ? _weight : FONT_WEIGHT_NORMAL;
            flags = _flags;
        }

        font(const font & another) = default;

        font & operator =(const font & another)
        {
            family = another.family;
            size = another.size;
            weight = another.weight;
            flags = another.flags;
            return *this;
        }

        bool operator ==(const font & another) const
        {
            return core::string::equal_ic()(family, another.family) &&
                size == another.size &&
                weight == another.weight &&
                flags == another.flags;
        }

        bool operator !=(const font & another) const
        {
            return !operator==(another);
        }

        std::string family;
        int32_t size = 0;
        int32_t weight = FONT_WEIGHT_NORMAL;
        int32_t flags = 0;
    };

    struct fontmetrics
    {
        int32_t size = 0;
        int32_t height = 0;
        int32_t ascent = 0;
        int32_t descent = 0;
        int32_t linespace = 0;
        int32_t weight = 0;
    };
}

namespace std
{
    template<>
    struct hash<graphics::text::font>
    {
    public:
        size_t operator()(const graphics::text::font & font) const
        {
            size_t h1 = std::hash<std::string>()(font.family);
            size_t h2 = std::hash<int32_t>()(font.size);
            size_t h3 = std::hash<int32_t>()(font.weight);
            size_t h4 = std::hash<int32_t>()(font.flags);
            return h1 ^ (h2 << 1) ^ (h3 << 1) ^ (h4 << 1);
        }
    };

    template<>
    struct less<graphics::text::font>
    {
        constexpr bool operator()(const graphics::text::font & lhs, const graphics::text::font & rhs) const
        {
            return lhs.family < rhs.family ||
                lhs.size < rhs.size ||
                lhs.weight < rhs.weight ||
                lhs.flags < rhs.flags;
        }
    };
}
