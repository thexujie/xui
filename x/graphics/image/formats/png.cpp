#include "stdafx.h"
#include "png.h"
#include "../deps/libpng/png.h"
#include "../deps/libpng/pngstruct.h"
#include "dds.h"
#pragma comment(lib, "libpng.lib")
#pragma comment(lib, "zlib.lib")

namespace graphics::image::formats
{
    using namespace core;

    struct png_source
    {
        const byte_t * buffer;
        int32_t length;
        const byte_t * curr;
    };

    void png_cpexcept_error(png_struct * png, png_const_charp msg)
    {
        longjmp(png->/*jmpbuf*/jmp_buf_local, 1);
    }

    void user_read_data_fcn(png_struct * png, png_byte * data, png_size_t length)
    {
        png_source * pBuff = (png_source *)png->io_ptr;
        memcpy((void *)data, (void *)pBuff->curr, length);
        pBuff->curr += length;
    }

    // 做 8 + 12 个字节的测试
    bool is_png_data(const byte_t * pBuffer, int32_t length)
    {
        if (length < 21)
            return false;
        for (int cnt = 0; cnt < sizeof(PNG_HEADER); ++cnt)
            if ((uint8_t)pBuffer[cnt] != PNG_HEADER[cnt])
                return false;

        // 有时只是想读取前若干个字节测试，所以校验文件尾作用不大。
        //for(int cnt = 0; cnt < sizeof(PNG_TAIL); ++cnt)
        //	if(pBuffer[cnt + length - sizeof(PNG_TAIL)] != PNG_TAIL[cnt])
        //		return false;

        return true;
    }

    core::error_e png_create(image_codec_context & ictx, const byte_t * buffer, int32_t length, image_data_t & image)
    {
        png_source buff;
        buff.buffer = buffer;
        buff.length = length;
        buff.curr = buffer;

        byte_t ** row_ptr = nullptr;

        png_struct * png = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, png_cpexcept_error, 0);
        png_info * pinfo = png_create_info_struct(png);

        if (setjmp(png_jmpbuf(png)))
        {
            png_free(png, row_ptr);
            png_destroy_read_struct(&png, &pinfo, 0);
            return error_inner;
        }

        png_set_read_fn(png, &buff, user_read_data_fcn);
        png_read_info(png, pinfo);

        int32_t bit_depth = 0;
        int32_t color_type = -1;
        png_uint_32 width = 0;
        png_uint_32 height = 0;

        png_get_IHDR(png, pinfo, &width, &height, &bit_depth, &color_type, 0, 0, 0);

        png_set_strip_16(png);

        if (png_get_valid(png, pinfo, PNG_INFO_tRNS))
            png_set_tRNS_to_alpha(png);

        image_convert_fun pfn_convert = nullptr;
        image_format format = {};
        format.width = (int32_t)width;
        format.height = (int32_t)height;

        int32_t src_bits = 0;
        png_color_type_e type = (png_color_type_e)color_type;
        switch (type)
        {
        case png_color_type_index_rgb:
            png_set_palette_to_rgb(png);
            format.format = format_b8g8r8;
            src_bits = 24;
            pfn_convert = image_convert_ex;
            break;
        case png_color_type_gray:
            switch (bit_depth)
            {
            case 1:
                format.format = format_gray1;
                pfn_convert = nullptr;
                break;
            case 2:
                format.format = format_gray2;
                pfn_convert = nullptr;
                break;
            case 4:
                format.format = format_gray4;
                pfn_convert = nullptr;
                break;
            case 8:
                format.format = format_gray8;
                pfn_convert = image_convert_ex;
                break;
            default:
                pfn_convert = nullptr;
                break;
            }
            src_bits = bit_depth;
            ///*png_set_gray_1_2_4_to_8*/png_set_expand_gray_1_2_4_to_8(png);
            break;
        case png_color_type_gray_alpha:
            png_set_gray_to_rgb(png);
            format.format = format_b8g8r8;
            pfn_convert = image_convert_ex;
            src_bits = 24;
            break;
        case png_color_type_rgb:
            format.format = format_b8g8r8;
            pfn_convert = image_convert_ex;
            src_bits = 24;
            break;
        case png_color_type_argb:
            format.format = format_a8b8g8r8;
            pfn_convert = image_convert_ex;
            src_bits = 32;
            break;
        default:
            pfn_convert = nullptr;
            break;
        }

        if (!pfn_convert)
        {
            png_free(png, row_ptr);
            png_destroy_read_struct(&png, &pinfo, 0);
            return error_not_supported;
        }

        //png_set_bgr(png);
        int32_t src_pitch = align_to_4((width * src_bits + 7) / 8);
        int32_t src_length = src_pitch * height;

        row_ptr = (byte_t **)png_malloc(png, height * sizeof(png_bytep));
        byte_t * src_buffer = image_malloc(src_length);

        for (png_uint_32 cnt = 0; cnt < height; ++cnt)
            row_ptr[cnt] = src_buffer + cnt * src_pitch;

        png_read_image(png, (png_byte **)row_ptr);
        png_read_end(png, 0);
        png_free(png, row_ptr);
        png_destroy_read_struct(&png, &pinfo, 0);

        image.format = format;
        if (ictx.get_format)
            image.format = ictx.get_format(image_type_bmp, format);

        ictx.pfn_alloc(image, 4);
        image.pfn_free = ictx.pfn_free;

        image_data_t src_data = {};
        src_data.format = format;
        src_data.data = src_buffer;
        src_data.pitch = src_pitch;

        error_e err = pfn_convert(ictx, src_data, image);
        if (err < 0)
        {
            image.pfn_free(image);
            return err;
        }
        return error_ok;
    }
}
