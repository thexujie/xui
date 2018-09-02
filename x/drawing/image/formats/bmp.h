#pragma once

#include "drawing/image/image.h"

namespace drawing::image::formats
{
    //////////////////////////////////////////////////////////////////////////
    // Bitmap 文件(压缩)格式
    typedef enum tag_bmp_encode
    {
        // 未压缩
        bmp_encode_none = 0,
        // rle8压缩
        bmp_encode_rle8,
        // rle4压缩
        bmp_encode_rle4,
        // 由掩码(color_mask_abgr_t)决定
        bmp_encode_mask,

        // os2 压缩方式
        bmp_encode_os2_index4,
        // OS/2 的 4 位编码
        bmp_encode_os2_index8,
        // OS/2 的 8 位编码
        bmp_encode_os2_24,
        // OS/2 的 24 位编码
    } bmp_encode_e;

    const byte_t BMP_HEADER[2] = { 0x42, 0x4D };
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

    bool is_bmp_data(const byte_t * buffer, size_t length);
    core::error bmp_create(image_codec_context & ictx, const byte_t * buffer, size_t length, image_data_t & image);
    core::error bmp_save(const image_data_t & data, std::string path);
    /**
    * Bmp 文件的 rle index4 压缩方式，使用索引色。
    * rle 压缩 src_stride 和 src_pitch 没有意义。
    */
    core::error image_convert_bmp_index4_rle(image_codec_context & icctx, const image_data_t & src, image_data_t & dst);
    /**
    * Bmp 文件的 rle index8 压缩方式，使用索引色。
    * rle 压缩 src_stride 和 src_pitch 没有意义。
    */
    core::error image_convert_bmp_index8_rle(image_codec_context & icctx, const image_data_t & src, image_data_t & dst);
}
