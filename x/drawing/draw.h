#pragma once

namespace script {
    struct range;
}

class SkTextBlobBuilder;

namespace drawing
{
    using core::section;

    enum class wrap_mode
    {
        none = 0,
        word,
        charactor,
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
        virtual core::error build(SkTextBlobBuilder & builder, uint32_t index) = 0;

        virtual void setFont(section range, const drawing::font & font) = 0;
        virtual void setColor(section range, uint32_t color) = 0;
    };
}
