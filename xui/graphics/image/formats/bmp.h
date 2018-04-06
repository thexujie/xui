#pragma once

#include "graphics/image/image.h"

namespace graphics { namespace image { namespace formats
{
    //////////////////////////////////////////////////////////////////////////
    // Bitmap �ļ�(ѹ��)��ʽ
    typedef enum tag_bmp_encode
    {
        bmp_encode_none = 0,
        // δѹ��
        bmp_encode_rle8,
        // rle8ѹ��
        bmp_encode_rle4,
        // rle4ѹ��
        bmp_encode_mask,
        // ������(color_mask_abgr_t)����

        // os2 ѹ����ʽ
        bmp_encode_os2_index4,
        // OS/2 �� 4 λ����
        bmp_encode_os2_index8,
        // OS/2 �� 8 λ����
        bmp_encode_os2_24,
        // OS/2 �� 24 λ����
    } bmp_encode_e;

    const byte_t BMP_HEADER[2] = {0x42, 0x4D};
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

    bool is_bmp_data(const byte_t * buffer, int32_t length);
    core::error_e bmp_create(const byte_t * buffer, int32_t length, image_data_t * img,
                       image_convert_rule_fun_t pfn_match = nullptr, void * user_data = nullptr);

    /**
    * @brief Ĭ�ϵ� bmp ��ʽת������Ĭ�Ͻ������ڲ���ʧ���ȵ�ǰ����ת��Ϊ GDI ���ݸ�ʽ
    * ���ܵ������ʽ�� r5g6b5 r8g8b8 a8r8g8b8 ����
    */
    bool bmp_rule_default(image_convert_rule_t * rule);

    /**
    * Bmp �ļ��� rle index4 ѹ����ʽ��ʹ������ɫ��
    * rle ѹ�� src_stride �� src_pitch û�����塣
    */
    void image_convert_bmp_index4_rle(int32_t width, int32_t height,
                                      pixel_convert_fun_t conv_fun,
                                      const byte_t * pal, int32_t pal_stride,
                                      const byte_t * src, int32_t /*src_stride*/, int32_t /*src_pitch*/,
                                      byte_t * dst, int32_t dst_stride, int32_t dst_pitch,
                                      int32_t flags);
    /**
    * Bmp �ļ��� rle index8 ѹ����ʽ��ʹ������ɫ��
    * rle ѹ�� src_stride �� src_pitch û�����塣
    */
    void image_convert_bmp_index8_rle(int32_t width, int32_t height,
                                      pixel_convert_fun_t conv_fun,
                                      const byte_t * pal, int32_t pal_stride,
                                      const byte_t * src, int32_t /*src_stride*/, int32_t /*src_pitch*/,
                                      byte_t * dst, int32_t dst_stride, int32_t dst_pitch,
                                      int32_t flags);
}}}
