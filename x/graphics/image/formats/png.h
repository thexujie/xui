#pragma once

#include "graphics/image/image.h"

namespace graphics::image::formats
{
    //	数据块符号	数据块名称			多数据块	可选否	位置限制
    //	IHDR		文件头数据块			否		否		第一块
    //	cHRM		基色和白色点数据块		否		是		在PLTE和IDAT之前
    //	gAMA		图像γ数据块			否		是		在PLTE和IDAT之前
    //	sBIT		样本有效位数据块		否		是		在PLTE和IDAT之前
    //	PLTE		调色板数据块			否		是		在IDAT之前
    //	bKGD		背景颜色数据块		否		是		在PLTE之后IDAT之前
    //	hIST		图像直方图数据块		否		是		在PLTE之后IDAT之前
    //	tRNS		图像透明数据块		否		是		在PLTE之后IDAT之前
    //	oFFs		(专用公共数据块)		否		是		在IDAT之前
    //	pHYs		物理像素尺寸数据块		否		是		在IDAT之前
    //	sCAL		(专用公共数据块)		否		是		在IDAT之前
    //	IDAT		图像数据块			是		否		与其他IDAT连续
    //	tIME		图像最后修改时间数据块	否		是		无限制
    //	tEXt		文本信息数据块		是		是		无限制
    //	zTXt		压缩文本数据块		是		是		无限制
    //	fRAc		(专用公共数据块)		是		是		无限制
    //	gIFg		(专用公共数据块)		是		是		无限制
    //	gIFt		(专用公共数据块)		是		是		无限制
    //	gIFx		(专用公共数据块)		是		是		无限制
    //	IEND		图像结束数据			否		否		最后一个数据块

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
        // 文件头 --------
        png_chunk_type_chrm = core::make_fourcc('c', 'H', 'R', 'M'),
        // 基色和白色点数据块
        png_chunk_type_gama = core::make_fourcc('g', 'A', 'M', 'A'),
        // 图像 gama 数据块
        png_chunk_type_sbits = core::make_fourcc('s', 'B', 'I', 'T'),
        // 样本有效位数据块
        png_chunk_type_plte = core::make_fourcc('P', 'L', 'T', 'E'),
        // 调色板数据块 ---------
        png_chunk_type_bkgd = core::make_fourcc('b', 'K', 'G', 'D'),
        // 背景颜色数据块
        png_chunk_type_hist = core::make_fourcc('h', 'I', 'S', 'T'),
        // 图像直方图数据块
        png_chunk_type_trns = core::make_fourcc('t', 'R', 'N', 'S'),
        // 图像透明数据块
        png_chunk_type_offs = core::make_fourcc('o', 'F', 'F', 's'),
        // (专用公共数据块)
        png_chunk_type_phys = core::make_fourcc('p', 'H', 'Y', 's'),
        // 物理像素尺寸数据块
        png_chunk_type_scal = core::make_fourcc('s', 'C', 'A', 'L'),
        // (专用公共数据块)
        png_chunk_type_idata = core::make_fourcc('I', 'D', 'A', 'T'),
        // 图像数据块
        png_chunk_type_time = core::make_fourcc('t', 'I', 'M', 'E'),
        // 图像最后修改时间数据块
        png_chunk_type_text = core::make_fourcc('t', 'E', 'X', 't'),
        // 文本信息数据块
        png_chunk_type_ztxt = core::make_fourcc('z', 'T', 'X', 't'),
        // 压缩文本数据块
        png_chunk_type_frac = core::make_fourcc('f', 'R', 'A', 'c'),
        // (专用公共数据块)
        png_chunk_type_gift = core::make_fourcc('g', 'I', 'F', 't'),
        // (专用公共数据块)
        png_chunk_type_gifx = core::make_fourcc('g', 'I', 'F', 'x'),
        // (专用公共数据块)
        png_chunk_type_iend = core::make_fourcc('I', 'E', 'N', 'D'),
        // 图像结束数据
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
        uint8_t unit; // 0(未知), 1(米)
    } png_chunk_phys_t;

#pragma pack(pop)

    bool is_png_data(const byte_t * pBuffer, int32_t length);
    core::error png_create(image_codec_context & ictx, const byte_t * buffer, int32_t length, image_data_t & image);
}
