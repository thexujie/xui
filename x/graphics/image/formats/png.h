#pragma once

#include "graphics/image/image.h"

namespace graphics::image::formats
{
    //	���ݿ����	���ݿ�����			�����ݿ�	��ѡ��	λ������
    //	IHDR		�ļ�ͷ���ݿ�			��		��		��һ��
    //	cHRM		��ɫ�Ͱ�ɫ�����ݿ�		��		��		��PLTE��IDAT֮ǰ
    //	gAMA		ͼ������ݿ�			��		��		��PLTE��IDAT֮ǰ
    //	sBIT		������Чλ���ݿ�		��		��		��PLTE��IDAT֮ǰ
    //	PLTE		��ɫ�����ݿ�			��		��		��IDAT֮ǰ
    //	bKGD		������ɫ���ݿ�		��		��		��PLTE֮��IDAT֮ǰ
    //	hIST		ͼ��ֱ��ͼ���ݿ�		��		��		��PLTE֮��IDAT֮ǰ
    //	tRNS		ͼ��͸�����ݿ�		��		��		��PLTE֮��IDAT֮ǰ
    //	oFFs		(ר�ù������ݿ�)		��		��		��IDAT֮ǰ
    //	pHYs		�������سߴ����ݿ�		��		��		��IDAT֮ǰ
    //	sCAL		(ר�ù������ݿ�)		��		��		��IDAT֮ǰ
    //	IDAT		ͼ�����ݿ�			��		��		������IDAT����
    //	tIME		ͼ������޸�ʱ�����ݿ�	��		��		������
    //	tEXt		�ı���Ϣ���ݿ�		��		��		������
    //	zTXt		ѹ���ı����ݿ�		��		��		������
    //	fRAc		(ר�ù������ݿ�)		��		��		������
    //	gIFg		(ר�ù������ݿ�)		��		��		������
    //	gIFt		(ר�ù������ݿ�)		��		��		������
    //	gIFx		(ר�ù������ݿ�)		��		��		������
    //	IEND		ͼ���������			��		��		���һ�����ݿ�

    const uint8_t PNG_HEADER[] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
    const uint8_t PNG_TAIL[12] = { 0x0, 0x0, 0x0, 0x0, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82 };
#pragma pack(push, 1)
    typedef enum tag_png_color_type
    {
        png_color_type_invalid = -1,
        png_color_type_gray = 0,
        //png_color_type_palette,
        png_color_type_rgb = 2,
        png_color_type_index_rgb = 3,
        png_color_type_gray_alpha = 4,
        png_color_type_argb = 6,
    } png_color_type_e;

    typedef struct tag_png_header
    {
        uint32_t width;
        uint32_t height;
        uint8_t bits;
        uint8_t color_type;
        uint8_t format;
        uint8_t filter;
        uint8_t interlace;
    } png_header_t;

    typedef enum tag_png_chunk_type
    {
        png_chunk_type_ihdr = core::make_fourcc('I', 'H', 'D', 'R'),
        // �ļ�ͷ --------
        png_chunk_type_chrm = core::make_fourcc('c', 'H', 'R', 'M'),
        // ��ɫ�Ͱ�ɫ�����ݿ�
        png_chunk_type_gama = core::make_fourcc('g', 'A', 'M', 'A'),
        // ͼ�� gama ���ݿ�
        png_chunk_type_sbits = core::make_fourcc('s', 'B', 'I', 'T'),
        // ������Чλ���ݿ�
        png_chunk_type_plte = core::make_fourcc('P', 'L', 'T', 'E'),
        // ��ɫ�����ݿ� ---------
        png_chunk_type_bkgd = core::make_fourcc('b', 'K', 'G', 'D'),
        // ������ɫ���ݿ�
        png_chunk_type_hist = core::make_fourcc('h', 'I', 'S', 'T'),
        // ͼ��ֱ��ͼ���ݿ�
        png_chunk_type_trns = core::make_fourcc('t', 'R', 'N', 'S'),
        // ͼ��͸�����ݿ�
        png_chunk_type_offs = core::make_fourcc('o', 'F', 'F', 's'),
        // (ר�ù������ݿ�)
        png_chunk_type_phys = core::make_fourcc('p', 'H', 'Y', 's'),
        // �������سߴ����ݿ�
        png_chunk_type_scal = core::make_fourcc('s', 'C', 'A', 'L'),
        // (ר�ù������ݿ�)
        png_chunk_type_idata = core::make_fourcc('I', 'D', 'A', 'T'),
        // ͼ�����ݿ�
        png_chunk_type_time = core::make_fourcc('t', 'I', 'M', 'E'),
        // ͼ������޸�ʱ�����ݿ�
        png_chunk_type_text = core::make_fourcc('t', 'E', 'X', 't'),
        // �ı���Ϣ���ݿ�
        png_chunk_type_ztxt = core::make_fourcc('z', 'T', 'X', 't'),
        // ѹ���ı����ݿ�
        png_chunk_type_frac = core::make_fourcc('f', 'R', 'A', 'c'),
        // (ר�ù������ݿ�)
        png_chunk_type_gift = core::make_fourcc('g', 'I', 'F', 't'),
        // (ר�ù������ݿ�)
        png_chunk_type_gifx = core::make_fourcc('g', 'I', 'F', 'x'),
        // (ר�ù������ݿ�)
        png_chunk_type_iend = core::make_fourcc('I', 'E', 'N', 'D'),
        // ͼ���������
    } png_chunk_type_e;

    typedef struct tag_png_chunk
    {
        union
        {
            uint32_t size;

            struct
            {
                uint8_t size_4;
                uint8_t size_3;
                uint8_t size_2;
                uint8_t size_1;
            };
        };

        union
        {
            png_chunk_type_e type;
            uint8_t type_str[4];
        };
    } png_chunk_t;

    typedef struct tag_png_chunk_sbit
    {
        uint32_t x1;
        uint32_t s_bit;
        uint32_t x2;
    } png_chunk_sbit_t;

    typedef struct tag_png_chunk_phys
    {
        uint32_t ppux;
        uint32_t ppuy;
        uint8_t unit; // 0(δ֪), 1(��)
    } png_chunk_phys_t;

#pragma pack(pop)

    bool is_png_data(const byte_t * pBuffer, int32_t length);
    core::error png_create(image_codec_context & ictx, const byte_t * buffer, int32_t length, image_data_t & image);
}
