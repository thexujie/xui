#pragma once

#include "drawing/image/image.h"

namespace drawing::image::formats
{
    //////////////////////////////////////////////////////////////////////////
    // Tga �ļ�(ѹ��)��ʽ

    enum tga_color_type_e : uint8_t
    {
        tga_color_type_rgb = 0,
        tga_color_type_index,
    };

    enum tga_format_e : uint8_t
    {
        tga_format_invalid = 0,
        // �ļ���û��ͼ������
        tga_format_index,
        // δѹ���ģ����� ��ɫ
        tga_format_rgb,
        // δѹ���ģ�RGB ��ɫ
        tga_format_gray,
        // δѹ���ģ��Ҷ� ��ɫ

        tga_format_rle_index = 9,
        // �γ̱���ģ����� ��ɫ
        tga_format_rle_rgb,
        // �γ̱���ģ�RGB ��ɫ
        tga_format_rle_gray,
        // �γ̱���ģ��Ҷ� ��ɫ

        // ���»�δ�ṩ֧��
        tga_format_hf_rle_index = 32,
        // ������ �γ̱���ģ����� ��ɫ
        tga_format_hf_rle_rgb,
        // ������ �γ̱���ģ�RGB ��ɫ
    };

#pragma pack(push, 1)
    //////////////////////////////////////////////////////////////////////////
    /// Tga �ļ�ͷ
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

    // ����ļ��ǰ��մ��ϵ��´洢�ġ�
    const uint32_t TGAF_TOP_TO_BOTTOM = 0x20;
    // �ļ�β
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
