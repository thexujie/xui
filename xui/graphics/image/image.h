#pragma once

namespace graphics::image
{
    /**
    * 图像格式
    */
    enum image_type
    {
        image_type_none = 0,
        image_type_bmp = core::make_fourcc('b', 'm', 'p', 0),
        image_type_tga = core::make_fourcc('t', 'g', 'a', 0),
        image_type_png = core::make_fourcc('p', 'n', 'g', 0),
        image_type_jpeg = core::make_fourcc('j', 'p', 'g', 0),
        image_type_dds = core::make_fourcc('d', 'd', 's', 0),
        image_type_user = core::make_fourcc('u', 's', 'e', 'r'),
    };

    //////////////////////////////////////////////////////////////////////////
    /// 颜色模式
    enum format
    {
        format_none = 0,
        format_gray1,
        format_gray2,
        format_gray4,
        format_gray8,

        format_r3g3b2,

        //---------------------------------16 bits
        format_r8g8,
        format_r5g6b5,
        format_b5g6r5,
        format_a1r5g5b5,
        format_a1b5g5r5,
        format_x1r5g5b5,
        format_x1b5g5r5,
        format_a4r4g4b4,
        format_x4r4g4b4,
        format_a8r3g3b2,

        //---------------------------------24 bits
        format_r8g8b8,
        format_b8g8r8,
        format_a8r5g6b5,
        format_a8b5g6r5,
        format_a8x1r5g5b5,
        format_a8x1b5g5r5,

        //---------------------------------32 bits
        format_a8r8g8b8,
        format_x8r8g8b8,
        format_a8b8g8r8,
        format_x8b8g8r8,
        format_r8g8b8a8,
        format_r8g8b8x8,
        format_b8g8r8a8,
        format_b8g8r8x8,

        format_g16r16,

        format_a2r10g10b10,
        format_a2b10g10r10,

        //---------------------------------
        format_a16b16g16r16,

        format_r16f,
        format_g16r16f,
        format_a16b16g16r16f,
        format_r32f,
        format_g32r32f,
        format_b32g32r32f,
        format_a32b32g32r32f,

        format_x32y32z32f,

        format_yuv,
        format_cmyk,
        format_ycck,

        //format_dx100 = format_dx1 + 99, // keep..
        format_count,
    };

    const char * format_text(format cmode);

    /**
    * 只能处理 argb 格式。
    */
    int32_t format_bits(format cmode);

#pragma pack(push, 1)

    struct image_frame_t
    {
        int32_t width;
        int32_t height;
        int32_t pitch;
        int32_t bits;
        int32_t length;
        format mode;
        const byte_t * buffer;
    };

    struct color_yuv_t
    {
        uint8_t y;
        uint8_t u;
        uint8_t v;
    };


    struct color_ycck_t
    {
        union
        {
            uint32_t color;

            struct
            {
                uint8_t y;
                uint8_t b;
                uint8_t r;
                uint8_t k;
            };
        };
    };


    struct color_r8g8b8_t
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
    };


    struct color_b8g8r8_t
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };


    struct color_a8b8g8r8_t
    {
        union
        {
            uint32_t color;

            struct
            {
                uint8_t r;
                uint8_t g;
                uint8_t b;
                uint8_t a;
            };
        };
    };


    struct color_a8r8g8b8_t
    {
        union
        {
            uint32_t color;

            struct
            {
                union
                {
                    color_r8g8b8_t rgb;

                    struct
                    {
                        uint8_t b;
                        uint8_t g;
                        uint8_t r;
                    };
                };

                uint8_t a;
            };
        };
    };


    struct color_r5g6b5
    {
        union
        {
            uint16_t color;

            struct
            {
                uint16_t b : 5;
                uint16_t g : 6;
                uint16_t r : 5;
            };
        };
    };


    struct color_b8g8r8a8_t
    {
        union
        {
            uint32_t color;

            struct
            {
                uint8_t a;

                union
                {
                    color_b8g8r8_t bgr;

                    struct
                    {
                        uint8_t r;
                        uint8_t g;
                        uint8_t b;
                    };
                };
            };
        };
    };


    struct color_r8g8b8a8_t
    {
        union
        {
            uint32_t color;

            struct
            {
                union
                {
                    uint8_t a;
                    uint8_t x;
                };

                union
                {
                    color_r8g8b8_t rgb;

                    struct
                    {
                        uint8_t b;
                        uint8_t g;
                        uint8_t r;
                    };
                };
            };
        };
    };


    struct color_cmyk_t
    {
        uint8_t c;
        uint8_t m;
        uint8_t y;
        uint8_t k;
    };


    struct color_a16b16g16r16_t
    {
        uint16_t r;
        uint16_t g;
        uint16_t b;
        uint16_t a;
    };


    struct color_g16r16_t
    {
        uint16_t r;
        uint16_t g;
    };


    struct color_g32r32f_t
    {
        float32_t r;
        float32_t g;
    };


    struct color_a32b32g32r32f_t
    {
        float32_t r;
        float32_t g;
        float32_t b;
        float32_t a;
    };
#pragma pack(pop)

    template<typename T>
    inline T align_to(T val, T align)
    {
        return (val + align - 1) / align * align;
    }

    int32_t align_to_2(int32_t val);
    int32_t align_to_4(int32_t val);


    struct color_x32y32z32f_t
    {
        float32_t x;
        float32_t y;
        float32_t z;
    };

    //////////////////////////////////////////////////////////////////////////
    /// 颜色掩码


    struct color_mask_abgr_t
    {
        uint32_t r = 0;
        uint32_t g = 0;
        uint32_t b = 0;
        uint32_t a = 0;
    };

    struct image_format
    {
        int32_t width = 0;
        int32_t height = 0;
        format format = format_none;
    };

    inline bool operator == (const image_format & lhs, const image_format & rhs)
    {
        return lhs.width == rhs.width && lhs.height == rhs.height && lhs.format == rhs.format;
    }

    inline bool operator != (const image_format & lhs, const image_format & rhs)
    {
        return !operator==(lhs, rhs);
    }

    // 转换时是否需要翻转行序
    const int32_t IMAGE_CONVERT_FLIP_Y = 0x1;

    typedef byte_t *(*image_buffer_alloc_fun)(image_format format);
    typedef void(*image_buffer_free_fun)(byte_t * ptr);

    typedef void(*pixel_convert_fun)(const void * src_pixel, void * dst_pixel);
    typedef core::error_e(*image_convert_fun)(struct image_codec_context & ctx, const struct image_data_t & src, struct image_data_t & dst);

    struct image_data_t
    {
        image_format format;
        int32_t pitch = 0;
        byte_t * data = nullptr;
        byte_t * palette = nullptr;
        int32_t palette_size = 0;
    };
    void image_buffer_alloc_default(image_data_t & data);
    void image_buffer_free(image_data_t & data);
    pixel_convert_fun image_get_samapler(format src, format dst);
    image_format image_get_format(image_type type, image_format format);

    struct image_t
    {
        image_data_t data;
        decltype(image_buffer_free) * pfn_free = nullptr;
    };

    struct image_codec_context
    {
        image_type type = image_type_none;

        decltype(image_get_format) * get_format = nullptr;
        decltype(image_get_samapler) * get_sampler = nullptr;
        decltype(image_buffer_alloc_default) * pfn_alloc = nullptr;
        decltype(image_buffer_free) * pfn_free = nullptr;
    };

    struct image_converter
    {
        format format = format_none;
        image_convert_fun image_convert_fun = nullptr;
        pixel_convert_fun pixel_convert_fun = nullptr;
    };


    typedef bool (*image_convert_rule_fun_t)(image_codec_context * rule);

    typedef core::error_e (*image_save_write_fun_t)(const void * buffer, int32_t size, void * userdata);
    typedef core::error_e (*image_save_fun_t)(const image_data_t * data, image_save_write_fun_t pfn_write, void * userdata);

    const color_mask_abgr_t MASK_R3G3B2 = { 0x00E0, 0x001C, 0x0003, 0x0000 };

    const color_mask_abgr_t MASK_A8R3G3B2 = { 0x00E0, 0x001C, 0x0003, 0xFF00 };

    const color_mask_abgr_t MASK_A4R4G4B4 = { 0x0F00, 0x00F0, 0x000F, 0xF000 };
    const color_mask_abgr_t MASK_X4R4G4B4 = { 0x0F00, 0x00F0, 0x000F, 0x0000 };

    const color_mask_abgr_t MASK_A0R5G6B5 = { 0xF800, 0x07E0, 0x001F, 0x0000 };

    const color_mask_abgr_t MASK_X1R5G5B5 = { 0x7C00, 0x03E0, 0x001F, 0x0000 };
    const color_mask_abgr_t MASK_A1R5G5B5 = { 0x7C00, 0x03E0, 0x001F, 0x8000 };

    const color_mask_abgr_t MASK_R8G8B8 = { 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 };
    const color_mask_abgr_t MASK_B8G8R8 = { 0x000000FF, 0x0000FF00, 0x00FF0000, 0x00000000 };

    const color_mask_abgr_t MASK_X8R8G8B8 = MASK_R8G8B8;
    const color_mask_abgr_t MASK_A8R8G8B8 = { 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000 };

    const color_mask_abgr_t MASK_X8B8G8R8 = MASK_B8G8R8;
    const color_mask_abgr_t MASK_A8B8G8R8 = { 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000 };

    const color_mask_abgr_t MASK_R8G8B8X8 = { 0xFF000000, 0x00FF0000, 0x0000FF00, 0x00000000 };
    const color_mask_abgr_t MASK_R8G8B8A8 = { 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF };

    const color_mask_abgr_t MASK_G16R16 = { 0x0000FFFF, 0xFFFF0000, 0x00000000, 0x00000000 };

    const color_mask_abgr_t MASK_A2R10G10B10 = { 0x3FF00000, 0x000FFC00, 0x000003FF, 0xC0000000 };
    const color_mask_abgr_t MASK_A2B10G10R10 = { 0x000003FF, 0x000FFC00, 0x3FF00000, 0xC0000000 };

    inline byte_t * image_malloc(int32_t size)
    {
        return (byte_t *)malloc(size);
    }

    inline void image_free(void * dst)
    {
        if (dst)
            free(dst);
    }

    inline void image_clear(void * dst, int32_t length)
    {
        memset(dst, 0, length);
    }

    inline void image_memcpy(void * dst, int32_t dst_size, const void * src, int32_t length)
    {
        memcpy_s(dst, dst_size, src, length);
    }

    core::error_e image_load(std::string path, image_t & img);

    image_type image_get_type(const byte_t * buffer, int32_t length);

    core::error_e image_create(const byte_t * buffer, int32_t length, image_t & img);
    core::error_e image_create(image_codec_context & ictx, const byte_t * buffer, int32_t length, image_t & img);

    /**
    * 复制图像，dst 和 src 具有相同的颜色格式。
    * 目标图像的 width 和 height 不能小于 源图像。
    */
    core::error_e image_convert_copy_ex(image_codec_context & icctx, const image_data_t & src, const image_data_t & dst);

    /**
    * image_convert_ex 只能将一种 rgb 格式转换成另一种 rgb 格式，
    * 不能进行调色板、压缩等操作。
    */
    core::error_e image_convert_ex(image_codec_context & icctx, const image_data_t & src, image_data_t & dst);
    core::error_e image_convert_index1_ex(image_codec_context & icctx, const image_data_t & src, image_data_t & dst);
    core::error_e image_convert_index2_ex(image_codec_context & icctx, const image_data_t & src, image_data_t & dst);
    core::error_e image_convert_index4_ex(image_codec_context & icctx, const image_data_t & src, image_data_t & dst);
    core::error_e image_convert_index8_ex(image_codec_context & icctx, const image_data_t & src, image_data_t & dst);


    // 根据颜色掩码计算颜色类型
    format format_from_mask_abgr(const color_mask_abgr_t & mask, int32_t bits);
    color_mask_abgr_t mask_from_format_abgr(format format);


    //////////////////////////////////////////////////////////////////////////
    /// 以下用于支持颜色之间的相互转换

    // - 复制
    void color_8_to_8(const void * src_pixel, void * dst_pixel);
    void color_16_to_16(const void * src_pixel, void * dst_pixel);
    void color_24_to_24(const void * src_pixel, void * dst_pixel);
    void color_32_to_32(const void * src_pixel, void * dst_pixel);
    void color_32_to_32_ex0(const void * src_pixel, void * dst_pixel);

    void color_r3g3b2_to_x1r5g5b5(const void * src_pixel, void * dst_pixel);
    void color_r3g3b2_to_a1r5g5b5(const void * src_pixel, void * dst_pixel);
    void color_r3g3b2_to_x8r8g8b8(const void * src_pixel, void * dst_pixel);

    void color_a8r3g3b2_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_a8r3g3b2_to_a8b8g8r8(const void * src_pixel, void * dst_pixel);

    void color_r8g8b8_to_b8g8r8(const void * src_pixel, void * dst_pixel);
    void color_r8g8b8_to_x8r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_r8g8b8_to_r8g8b8a8(const void * src_pixel, void * dst_pixel);
    void color_a8r8g8b8_to_r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_a8b8g8r8_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_x8b8g8r8_to_r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_x8r8g8b8_to_r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_x8r8g8b8_to_x1r5g5b5(const void * src_pixel, void * dst_pixel);
    void color_x8r8g8b8_to_r5g6b5(const void * src_pixel, void * dst_pixel);

    void color_r5g6b5_to_x1r5g5b5(const void * src_pixel, void * dst_pixel);
    void color_r5g6b5_to_r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_r5g6b5_to_x8r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_r5g6b5_to_x8b8g8r8(const void * src_pixel, void * dst_pixel);
    void color_b5g6r5_to_r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_b5g6r5_to_x8r8g8b8(const void * src_pixel, void * dst_pixel);


    void color_a1r5g5b5_to_x1r5g5b5(const void * src_pixel, void * dst_pixel);
    void color_a1r5g5b5_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_a1r5g6b5_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_a4r5g6b5_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);

    void color_x4r4g4b4_to_x1r5g5b5(const void * src_pixel, void * dst_pixel);
    void color_x4r4g4b4_to_x8r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_x4r4g4b4_to_x8b8g8r8(const void * src_pixel, void * dst_pixel);
    void color_a4r4g4b4_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_a4r4g4b4_to_a8b8g8r8(const void * src_pixel, void * dst_pixel);
    void color_a4r4g4b4_to_x8b8g8r8(const void * src_pixel, void * dst_pixel);

    void color_a16b16g16r16_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_a16b16g16r16f_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_g16r16_to_r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_g16r16f_to_r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_r16f_to_r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_r32f_to_r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_g32r32f_to_r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_a32b32g32r32f_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);

    void color_gray8_to_x8r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_gray8_to_r8g8b8(const void * src_pixel, void * dst_pixel);

    void color_x1r5g5b5_to_a1r5g5b5(const void * src_pixel, void * dst_pixel);
    void color_x1r5g5b5_to_r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_x1r5g5b5_to_x8r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_x1r5g5b5_to_x8b8g8r8(const void * src_pixel, void * dst_pixel);

    void color_a8r8g8b8_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);

    void color_yuv_to_r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_r8g8b8_to_yuv(const void * src_pixel, void * dst_pixel);
    void color_yuv_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);

    void color_ycck_to_r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_ycck_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);

    void color_gray_to_r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_gray_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);

    void color_cmyk_to_r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_cmyk_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);

    void color_r8g8b8_to_x32y32z32f(const void * src_pixel, void * dst_pixel);
    void color_x32y32z32f_tor_r8g8b8(const void * src_pixel, void * dst_pixel);

    void color_r8g8b8x8_to_r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_r8g8b8x8_to_x8b8g8r8(const void * src_pixel, void * dst_pixel);
    void color_r8g8b8a8_to_b8g8r8a8(const void * src_pixel, void * dst_pixel);
    void color_a8r8g8b8_to_a8b8g8r8(const void * src_pixel, void * dst_pixel);


    void color_r8g8b8_to_a8b8g8r8(const void * src_pixel, void * dst_pixel);
    void color_r8g8b8_to_x8b8g8r8(const void * src_pixel, void * dst_pixel);
    void color_b8g8r8_to_a8b8g8r8(const void * src_pixel, void * dst_pixel);
    void color_b8g8r8_to_x8r8g8b8(const void * src_pixel, void * dst_pixel);

    void color_x8r8g8b8_to_x8b8g8r8(const void * src_pixel, void * dst_pixel);
    void color_x8b8g8r8_to_x8r8g8b8(const void * src_pixel, void * dst_pixel);


    void color_a2r10g10b10_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);

    void color_a2b10g10r10_to_a8r8g8b8(const void * src_pixel, void * dst_pixel);
    void color_a2b10g10r10_to_a2b10g10r10(const void * src_pixel, void * dst_pixel);
    void color_a2b10g10r10_to_a2r10g10b10(const void * src_pixel, void * dst_pixel);
}
