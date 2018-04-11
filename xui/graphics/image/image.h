#pragma once

namespace graphics::image
{
    /**
    * 图像格式
    */
    typedef enum tag_image_format_e
    {
        image_format_invalid = 0,
        image_format_bmp = core::make_fourcc('b', 'm', 'p', 0),
        image_format_tga = core::make_fourcc('t', 'g', 'a', 0),
        image_format_png = core::make_fourcc('p', 'n', 'g', 0),
        image_format_jpg = core::make_fourcc('j', 'p', 'g', 0),
        image_format_dds = core::make_fourcc('d', 'd', 's', 0),
        image_format_user = core::make_fourcc('u', 's', 'e', 'r'),
    } image_format_e;

    //////////////////////////////////////////////////////////////////////////
    /// 颜色模式
    enum cmode_e
    {
        cmode_none = 0,
        cmode_gray1,
        cmode_gray2,
        cmode_gray4,
        cmode_gray8,

        cmode_r3g3b2,

        //---------------------------------16 bits
        cmode_r8g8,
        cmode_r5g6b5,
        cmode_b5g6r5,
        cmode_a1r5g5b5,
        cmode_a1b5g5r5,
        cmode_x1r5g5b5,
        cmode_x1b5g5r5,
        cmode_a4r4g4b4,
        cmode_x4r4g4b4,
        cmode_a8r3g3b2,

        //---------------------------------24 bits
        cmode_r8g8b8,
        cmode_b8g8r8,
        cmode_a8r5g6b5,
        cmode_a8b5g6r5,
        cmode_a8x1r5g5b5,
        cmode_a8x1b5g5r5,

        //---------------------------------32 bits
        cmode_a8r8g8b8,
        cmode_x8r8g8b8,
        cmode_a8b8g8r8,
        cmode_x8b8g8r8,
        cmode_r8g8b8a8,
        cmode_r8g8b8x8,
        cmode_b8g8r8a8,
        cmode_b8g8r8x8,

        cmode_g16r16,

        cmode_a2r10g10b10,
        cmode_a2b10g10r10,

        //---------------------------------
        cmode_a16b16g16r16,

        cmode_r16f,
        cmode_g16r16f,
        cmode_a16b16g16r16f,
        cmode_r32f,
        cmode_g32r32f,
        cmode_b32g32r32f,
        cmode_a32b32g32r32f,

        cmode_x32y32z32f,

        cmode_index4_rle2_x8r8g8b8,
        cmode_index8_rle2_x8r8g8b8,

        cmode_rle_gray8,
        cmode_rle_x1r5g5b5,
        cmode_rle_r8g8b8,
        cmode_rle_a8r8g8b8,

        cmode_index1_x8r8g8b8,
        cmode_index2_x8r8g8b8,

        cmode_index4_r8g8b8,
        cmode_index4_x8r8g8b8,
        cmode_index4_a8r8g8b8,

        cmode_index8_gray8,
        cmode_index8_x1r5g5b5,
        cmode_index8_r8g8b8,
        cmode_index8_x8r8g8b8,
        cmode_index8_a8r8g8b8,

        cmode_yuv,
        cmode_cmyk,
        cmode_ycck,

        cmode_bc1,
        // r5g6b5
        cmode_bc2,
        // a8r5g6b5
        cmode_bc3,
        // a8r5g6b5
        cmode_bc4,
        cmode_bc5,
        cmode_dx10,


        cmode_d24s8,
        //cmode_dx100 = cmode_dx1 + 99, // keep..
    };

    const char * cmode_text(cmode_e cmode);

    /**
    * 只能处理 argb 格式。
    */
    int32_t cmode_bits(cmode_e cmode);

    struct image_data_t
    {
        int32_t width;
        int32_t height;
        int32_t bits;
        int32_t pitch;
        int32_t length;
        cmode_e src_mode;
        cmode_e dst_mode;
        int32_t flags;

        byte_t * buffer;
    };

    bool image_data_malloc(image_data_t * data);
    void image_data_free(image_data_t * data);

#pragma pack(push, 1)

    struct image_frame_t
    {
        int32_t width;
        int32_t height;
        int32_t pitch;
        int32_t bits;
        int32_t length;
        cmode_e mode;
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


    // 转换时是否需要翻转行序
    const int32_t IMAGE_CONVERT_FLIP_Y = 0x1;

    typedef void (*pixel_convert_fun_t)(const void * src_pixel, void * dst_pixel);

    /// 以下用于支持颜色之间的相互转换
    typedef void (*image_convert_fun_t)(int32_t width, int32_t height,
        pixel_convert_fun_t conv_fun, const byte_t * palette, int32_t pal_stride,
        const byte_t * src, int32_t src_strike, int32_t src_pitch,
        byte_t * dst, int32_t dst_strike, int32_t dst_pitch, int32_t flags);

    struct image_convert_rule_t
    {
        image_format_e format;
        //----------------------------------- 输入部分
        int32_t width;
        int32_t height;
        cmode_e src_mode;
        void * user_data;
        int32_t user_format;

        //-----------------------------------输出部分
        cmode_e dst_mode;

        pixel_convert_fun_t pixel_convert_fun;
        image_convert_fun_t image_convert_fun;

        //! 调色板的颜色位数（如果有调色板的话）
        int32_t pal_bits;
        //! 调色板的颜色跨度（如果有调色板的话）
        int32_t pal_stride;

        int32_t src_bits;
        int32_t src_stride;
        int32_t src_pitch;

        int32_t dst_bits;
        int32_t dst_stride;
        int32_t dst_pitch;

        int32_t dst_length;
        byte_t * dst_buffer;
    };

    typedef bool (*image_convert_rule_fun_t)(image_convert_rule_t * rule);

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

    core::error_e image_load(std::string path, image_data_t * img,
        image_format_e image_format = image_format_invalid,
        image_convert_rule_fun_t pfn_rule = nullptr, void * user_data = nullptr);

    image_format_e image_get_format(const byte_t * buffer, int32_t length);


    core::error_e image_create(const byte_t * buffer, int32_t length, image_data_t * img,
        image_convert_rule_fun_t pfn_rule = nullptr, void * user_data = nullptr);

    core::error_e image_create(const byte_t * buffer, int32_t length, image_data_t * img,
        image_format_e image_format,
        image_convert_rule_fun_t pfn_rule = nullptr, void * user_data = nullptr);

    /**
    * 用 src 所指定的颜色填充 dst。
    * src 只提供 1 个像素的颜色信息。
    */
    void image_convert_fill_ex(int32_t width, int32_t height,
        pixel_convert_fun_t conv_fun, const byte_t * palette,
        const byte_t * src, int32_t /*src_stride*/, int32_t /*src_pitch*/,
        byte_t * dst, int32_t dst_stride, int32_t dst_pitch,
        int32_t flags);

    /**
    * 复制图像，dst 和 src 具有相同的颜色格式。
    * 目标图像的 width 和 height 不能小于 源图像。
    */
    void image_convert_copy_ex(int32_t /*width*/, int32_t height,
        pixel_convert_fun_t /*conv_fun*/,
        const byte_t * /*pal*/, int32_t /*pal_stride*/,
        const byte_t * src, int32_t /*src_stride*/, int32_t src_pitch,
        byte_t * dst, int32_t /*dst_stride*/, int32_t dst_pitch,
        int32_t flags);

    /**
    * image_convert_ex 只能将一种 rgb 格式转换成另一种 rgb 格式，
    * 不能进行调色板、压缩等操作。
    */
    void image_convert_ex(int32_t width, int32_t height,
        pixel_convert_fun_t conv_fun,
        const byte_t * /*pal*/, int32_t /*pal_stride*/,
        const byte_t * src, int32_t src_stride, int32_t src_pitch,
        byte_t * dst, int32_t dst_stride, int32_t dst_pitch,
        int32_t flags);

    /**
    * 每读取一个字节，生成 8 个像素。同时，src 前进 src_stride，dst 前进 dst_stride * 8。
    * 每读取一行，src_line 前进 src_pitch，dst_line 前进 dst_pitch。
    * @param width 图像的宽度
    * @param height 图像的高度
    * @param conv_fun 像素的格式转换
    * @param pal 调色板，可以为 nullptr
    * @param src 索引数组
    * @param src_stride 每读取一个字节（生成 8 个像素），src 前进的字节大小。
    * @param src_pitch 每读取一行（生成 width 个像素），src 前进的字节大小。
    * @param dst 输出目标
    * @param dst_stride 每生成一个像素，dst 前进的字节大小
    * @param dst_pitch 每读生成一行（生成 width 个像素），src 前进的字节大小。
    * @param flags 标志位，参见...
    */
    void image_convert_index1_ex(int32_t width, int32_t height,
        pixel_convert_fun_t conv_fun,
        const byte_t * pal, int32_t pal_stride,
        const byte_t * src, int32_t src_stride, int32_t src_pitch,
        byte_t * dst, int32_t dst_stride, int32_t dst_pitch,
        int32_t flags);

    /**
    * 每读取一个字节，生成 4 个像素。同时，src 前进 src_stride，dst 前进 dst_stride * 4。
    * 每读取一行，src_line 前进 src_pitch，dst_line 前进 dst_pitch。
    * @param width 图像的宽度
    * @param height 图像的高度
    * @param conv_fun 像素的格式转换
    * @param pal 调色板，可以为 nullptr
    * @param src 索引数组
    * @param src_stride 每读取一个字节（生成 4 个像素），src 前进的字节大小。
    * @param src_pitch 每读取一行（生成 width 个像素），src 前进的字节大小。
    * @param dst 输出目标
    * @param dst_stride 每生成一个像素，dst 前进的字节大小
    * @param dst_pitch 每读生成一行（生成 width 个像素），src 前进的字节大小。
    * @param flags 标志位，参见...
    */
    void image_convert_index2_ex(int32_t width, int32_t height,
        pixel_convert_fun_t conv_fun,
        const byte_t * pal, int32_t pal_stride,
        const byte_t * src, int32_t src_stride, int32_t src_pitch,
        byte_t * dst, int32_t dst_stride, int32_t dst_pitch,
        int32_t flags);

    /**
    * 每读取一个字节，生成 2 个像素。同时，src 前进 src_stride，dst 前进 dst_stride * 2。
    * 每读取一行，src_line 前进 src_pitch，dst_line 前进 dst_pitch。
    * @param width 图像的宽度
    * @param height 图像的高度
    * @param conv_fun 像素的格式转换
    * @param pal 调色板，可以为 nullptr
    * @param src 索引数组
    * @param src_stride 每读取一个字节（生成两个像素），src 前进的字节大小。
    * @param src_pitch 每读取一行（生成 width 个像素），src 前进的字节大小。
    * @param dst 输出目标
    * @param dst_stride 每生成一个像素，dst 前进的字节大小
    * @param dst_pitch 每读生成一行（生成 width 个像素），src 前进的字节大小。
    * @param flags 标志位，参见...
    */
    void image_convert_index4_ex(int32_t width, int32_t height,
        pixel_convert_fun_t conv_fun,
        const byte_t * pal, int32_t pal_stride,
        const byte_t * src, int32_t src_stride, int32_t src_pitch,
        byte_t * dst, int32_t dst_stride, int32_t dst_pitch,
        int32_t flags);
    /**
    * 每读取一个字节，生成 1 个像素。同时，src 前进 src_stride，dst 前进 dst_stride * 1。
    * 每读取一行，src_line 前进 src_pitch，dst_line 前进 dst_pitch。
    * @param width 图像的宽度
    * @param height 图像的高度
    * @param conv_fun 像素的格式转换
    * @param pal 调色板，可以为 nullptr
    * @param src 索引数组
    * @param src_stride 每读取一个字节（生成 1 个像素），src 前进的字节大小。
    * @param src_pitch 每读取一行（生成 width 个像素），src 前进的字节大小。
    * @param dst 输出目标
    * @param dst_stride 每生成一个像素，dst 前进的字节大小
    * @param dst_pitch 每读生成一行（生成 width 个像素），src 前进的字节大小。
    * @param flags 标志位，参见...
    */
    void image_convert_index8_ex(int32_t width, int32_t height,
        pixel_convert_fun_t conv_fun,
        const byte_t * pal, int32_t pal_stride,
        const byte_t * src, int32_t src_stride, int32_t src_pitch,
        byte_t * dst, int32_t dst_stride, int32_t dst_pitch,
        int32_t flags);


    // 根据颜色掩码计算颜色类型
    cmode_e cmode_from_mask_abgr(const color_mask_abgr_t & mask, int32_t bits);

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
