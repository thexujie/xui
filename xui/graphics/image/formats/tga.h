#pragma once

#include "graphics/image/image.h"

namespace graphics { namespace image { namespace formats
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

    // 标记文件是按照从下到上存储的。
    const uint32_t TGAF_FLIPY = 0x20;
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


    bool is_tga_data(const byte_t * buffer, int32_t length);
    core::error_e tga_create(const byte_t * buffer, int32_t length, image_data_t * img,
                       image_convert_rule_fun_t pfn_match = nullptr, void * user_data = nullptr);

    bool tga_rule_default(image_convert_rule_t * rule);

    void image_convert_tga_rle8(int32_t width, int32_t height,
                                pixel_convert_fun_t conv_fun,
                                const byte_t * pal, int32_t pal_stride,
                                const byte_t * src, int32_t src_stride, int32_t src_pitch,
        byte_t * dst, int32_t dst_stride, int32_t dst_pitch,
                                int32_t flags);

    core::error_e tga_save(const image_data_t * data, image_save_write_fun_t pfn_write, void * userdata);
    core::error_e tga_save_ex(const image_data_t * data, image_save_write_fun_t pfn_write, void * userdata,
                      tga_format_e format);
}}}
