#pragma once

#include "drawing/image/image.h"

namespace drawing::image::formats
{
    //////////////////////////////////////////////////////////////////////////
    // Tga 文件(压缩)格式

    enum tga_color_type_e : uint8_t
    {
        tga_color_type_rgb = 0,
        tga_color_type_index,
    };

    enum tga_format_e : uint8_t
    {
        tga_format_invalid = 0,
        // 文件中没有图像数据
        tga_format_index,
        // 未压缩的，索引 颜色
        tga_format_rgb,
        // 未压缩的，RGB 颜色
        tga_format_gray,
        // 未压缩的，灰度 颜色

        tga_format_rle_index = 9,
        // 游程编码的，索引 颜色
        tga_format_rle_rgb,
        // 游程编码的，RGB 颜色
        tga_format_rle_gray,
        // 游程编码的，灰度 颜色

        // 以下还未提供支持
        tga_format_hf_rle_index = 32,
        // 哈夫曼 游程编码的，索引 颜色
        tga_format_hf_rle_rgb,
        // 哈夫曼 游程编码的，RGB 颜色
    };

#pragma pack(push, 1)
    //////////////////////////////////////////////////////////////////////////
    /// Tga 文件头
    typedef struct tag_tga_header_coord
    {
        uint8_t low;
        uint8_t high;
    } tga_header_short_t;

    typedef struct tag_tga_header
    {
        uint8_t info_length;
        tga_color_type_e color_type;
        tga_format_e commpress;
        tga_header_short_t color_entry;
        uint16_t color_count;
        uint8_t color_bit;

        union
        {
            tga_header_short_t x_orgin;
            uint16_t x;
        };

        union
        {
            tga_header_short_t y_orgin;
            uint16_t y;
        };

        uint16_t width;
        uint16_t height;
        uint8_t bit_count;
        uint8_t flags;
    } tga_header_t;

    // 标记文件是按照从上到下存储的。
    const uint32_t TGAF_TOP_TO_BOTTOM = 0x20;
    // 文件尾
    const uint8_t TGA_TAIL[] = "TRUEVISION-XFILE";

    typedef struct tag_tga_tailer
    {
        uint32_t extension_offset;
        uint32_t developer_offset;
        uint8_t signature[18];
    } tga_tailer_t;

#pragma pack(pop)

    typedef struct tag_tga_image_data : public image_data_t
    {
        tga_format_e format;
    } tga_image_data_t;


    bool is_tga_data(const byte_t * buffer, size_t length);
    core::error tga_create(image_codec_context & ictx, const byte_t * buffer, size_t length, image_data_t & image);
    core::error tga_save(const image_data_t & data, std::string path);

    core::error image_convert_tga_rle8(image_codec_context & icctx, const image_data_t & src, image_data_t & dst);

}
