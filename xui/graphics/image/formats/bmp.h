#pragma once

#include "graphics/image/image.h"

namespace graphics { namespace image { namespace formats
{
    //////////////////////////////////////////////////////////////////////////
    // Bitmap 文件(压缩)格式
    typedef enum tag_bmp_encode
    {
        bmp_encode_none = 0,
        // 未压缩
        bmp_encode_rle8,
        // rle8压缩
        bmp_encode_rle4,
        // rle4压缩
        bmp_encode_mask,
        // 由掩码(color_mask_abgr_t)决定

        // os2 压缩方式
        bmp_encode_os2_index4,
        // OS/2 的 4 位编码
        bmp_encode_os2_index8,
        // OS/2 的 8 位编码
        bmp_encode_os2_24,
        // OS/2 的 24 位编码
    } bmp_encode_e;

    const byte_t BMP_HEADER[2] = {0x42, 0x4D};
    const uint16_t BMP_MAGIC = 0x4D42; // 'BM',header.type

#pragma pack(push, 1)
    //////////////////////////////////////////////////////////////////////////
    // 简单的 Bitmap 文件头，用于识别 Bitmap 文件类型。
    struct bmp_simple_header_t
    {
        uint16_t type;
        uint32_t file_size;
        uint32_t reserved;
        uint32_t data_offset;
        uint32_t info_size;
    };

    //////////////////////////////////////////////////////////////////////////
    // Windows Bitmap 文件头
    struct bmp_info_windows_t
    {
        uint32_t size;
        int32_t width;
        int32_t height;
        uint16_t plane_count;
        uint16_t bit_count;
        bmp_encode_e encode;
        uint32_t image_size;
        int32_t ppm_x; // pels per meter
        int32_t ppm_y;
        uint32_t color_used;
        uint32_t color_important;
    };

    struct bmp_header_windows_t
    {
        uint16_t type;
        uint32_t file_size;
        uint32_t reserved;
        uint32_t data_offset;
    };

    //////////////////////////////////////////////////////////////////////////
    // OS/2 Bitmap 文件头
    struct bmp_info_os2_t
    {
        uint32_t info_size;
        uint16_t width;
        uint16_t height;
        uint16_t planes;
        uint16_t bits;
    };

    struct bmp_header_os2_t
    {
        uint16_t type;
        uint32_t file_size;
        uint32_t reserved;
        uint32_t data_offset;

        union
        {
            bmp_info_os2_t info;

            struct
            {
                uint32_t info_size;
                uint16_t width;
                uint16_t height;
                uint16_t planes;
                uint16_t bits;
            };
        };
    };

#pragma pack(pop)

    bool is_bmp_data(const byte_t * buffer, int32_t length);
    core::error_e bmp_create(const byte_t * buffer, int32_t length, image_data_t * img,
                       image_convert_rule_fun_t pfn_match = nullptr, void * user_data = nullptr);

    /**
    * @brief 默认的 bmp 格式转换规则，默认将尽量在不损失精度的前提下转换为 GDI 兼容格式
    * 可能的输出格式有 r5g6b5 r8g8b8 a8r8g8b8 三种
    */
    bool bmp_rule_default(image_convert_rule_t * rule);

    /**
    * Bmp 文件的 rle index4 压缩方式，使用索引色。
    * rle 压缩 src_stride 和 src_pitch 没有意义。
    */
    void image_convert_bmp_index4_rle(int32_t width, int32_t height,
                                      pixel_convert_fun_t conv_fun,
                                      const byte_t * pal, int32_t pal_stride,
                                      const byte_t * src, int32_t /*src_stride*/, int32_t /*src_pitch*/,
                                      byte_t * dst, int32_t dst_stride, int32_t dst_pitch,
                                      int32_t flags);
    /**
    * Bmp 文件的 rle index8 压缩方式，使用索引色。
    * rle 压缩 src_stride 和 src_pitch 没有意义。
    */
    void image_convert_bmp_index8_rle(int32_t width, int32_t height,
                                      pixel_convert_fun_t conv_fun,
                                      const byte_t * pal, int32_t pal_stride,
                                      const byte_t * src, int32_t /*src_stride*/, int32_t /*src_pitch*/,
                                      byte_t * dst, int32_t dst_stride, int32_t dst_pitch,
                                      int32_t flags);
}}}
