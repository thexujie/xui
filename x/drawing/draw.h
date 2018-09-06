#pragma once

class SkTextBlobBuilder;

namespace drawing
{
    enum class wrap_mode
    {
        none = 0,
        word,
        charactor,
    };

    // range
    struct range
    {
        size_t index = 0;
        size_t length = 0;

        size_t end() const { return index + length; }
        range operator + (const range & rhs)
        {
            if (!length)
                return rhs;

            if (!rhs.length)
                return *this;

            if (index + length == rhs.index)
                return { index, rhs.index + rhs.length };

            if (rhs.index + rhs.length == index)
                return { rhs.index, index + length };

            return { 0, 0 };
        }

        range & operator += (const range & rhs)
        {
            if (!length)
                *this = rhs;
            else if (!rhs.length)
                ;
            else if (index + length == rhs.index)
                *this = { index, length + rhs.length };
            else if (rhs.index + rhs.length == index)
                *this = { rhs.index, length + rhs.length };
            else
                *this = { 0, 0 };
            return *this;
        }
    };

    struct bitmap_buffer
    {
        byte_t * data = nullptr;
        core::si32i size;
        int32_t strike = 0;
        int32_t pitch = 0;
        bool flip_y = false;
    };

    class IShaper
    {
    public:
        virtual ~IShaper() = default;
        virtual core::error reset(std::string text) = 0;
        virtual core::error itermize() = 0;
        virtual core::error wrap(float32_t end, wrap_mode mode) = 0;
        virtual core::error shape(SkTextBlobBuilder & builder, uint32_t index) = 0;

        virtual void setFont(range range, const drawing::font & font) = 0;
        virtual void setColor(range range, uint32_t color) = 0;
    };
}
