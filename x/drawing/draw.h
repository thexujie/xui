#pragma once

namespace script {
    struct range;
}

class SkTextBlobBuilder;

namespace drawing
{
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
}
