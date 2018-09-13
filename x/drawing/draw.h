#pragma once

namespace script {
    struct range;
}

class SkTextBlobBuilder;

namespace drawing
{
    using core::section32;
    using core::section8;

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
        virtual core::error itermize(std::string text, const drawing::font & font, core::color32 color) = 0;
        virtual core::error wrap(float32_t end, wrap_mode mode) = 0;
        virtual core::error build(SkTextBlobBuilder & builder, uint32_t index) = 0;

        virtual void setFont(section32 range, const drawing::font & font) = 0;
        virtual void setColor(section32 range, uint32_t color) = 0;
    };
}
