#pragma once

#include "drawing/image/image.h"

namespace drawing::image::formats
{
    //////////////////////////////////////////////////////////////////////////
    // Bitmap �ļ�(ѹ��)��ʽ
    typedef enum tag_bmp_encode
    {
        // δѹ��
        bmp_encode_none = 0,
        // rle8ѹ��
        bmp_encode_rle8,
        // rle4ѹ��
        bmp_encode_rle4,
        // ������(color_mask_abgr_t)����
        bmp_encode_mask,

        // os2 ѹ����ʽ
        bmp_encode_os2_index4,
        // OS/2 �� 4 λ����
        bmp_encode_os2_index8,
        // OS/2 �� 8 λ����
        bmp_encode_os2_24,
        // OS/2 �� 24 λ����
    } bmp_encode_e;

    const byte_t BMP_HEADER[2] = { 0x42, 0x4D };
    const uint16_t BMP_MAGIC = 0x4D42; // 'BM',header.type

#pragma pack(push, 1)
    //////////////////////////////////////////////////////////////////////////
    // �򵥵� Bitmap �ļ�ͷ������ʶ�� Bitmap �ļ����͡�
    struct bmp_simple_header_t
    {
        uint16_t type;
        uint32_t file_size;
        uint32_t reserved;
        uint32_t data_offset;
        uint32_t info_size;
    };

    //////////////////////////////////////////////////////////////////////////
    // Windows Bitmap �ļ�ͷ
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
    // OS/2 Bitmap �ļ�ͷ
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
    * Bmp �ļ��� rle index4 ѹ����ʽ��ʹ������ɫ��
    * rle ѹ�� src_stride �� src_pitch û�����塣
    */
    core::error image_convert_bmp_index4_rle(image_codec_context & icctx, const image_data_t & src, image_data_t & dst);
    /**
    * Bmp �ļ��� rle index8 ѹ����ʽ��ʹ������ɫ��
    * rle ѹ�� src_stride �� src_pitch û�����塣
    */
    core::error image_convert_bmp_index8_rle(image_codec_context & icctx, const image_data_t & src, image_data_t & dst);
}
