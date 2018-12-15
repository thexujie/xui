#pragma once

namespace script {
    struct range;
}

class SkTextBlobBuilder;

namespace drawing
{
    enum class blend_mode
    {
        clear,
        src,
        dst,
        src_over,
        dst_over,
        src_in,
        dst_in,
        src_out,
        dst_out,
        src_atop,
        dst_atop,
        xor,
        plus,
        overlay,

		darken,
		lighten,

		dodge,
		burn,

		hard_light,
		soft_light,

		difference,
		exclusion,
		multiply,

		hue,
		saturation,
		color,
		luminosity,
    };

    enum class wrap_mode
    {
        none = 0,
        word,
        charactor,
    };

    struct bitmap_buffer
    {
        byte_t * data = nullptr;
        core::sizei size;
        int32_t strike = 0;
        int32_t pitch = 0;
        bool flip_y = false;
    };
}
