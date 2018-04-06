#include "stdafx.h"
#include "png.h"
#include "../deps/libpng/png.h"
#include "../deps/libpng/pngstruct.h"
#pragma comment(lib, "libpng.lib")
#pragma comment(lib, "zlib.lib")

namespace graphics { namespace image { namespace formats
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
            if (pBuffer[cnt] != PNG_HEADER[cnt])
                return false;

        // 有时只是想读取前若干个字节测试，所以校验文件尾作用不大。
        //for(int cnt = 0; cnt < sizeof(PNG_TAIL); ++cnt)
        //	if(pBuffer[cnt + length - sizeof(PNG_TAIL)] != PNG_TAIL[cnt])
        //		return false;

        return true;
    }

    core::error_e png_create(const byte_t * buffer, int32_t length, image_data_t * img,
                       image_convert_rule_fun_t pfn_match, void * user_data)
    {
        if (!pfn_match)
            pfn_match = png_rule_default;

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
        cmode_e src_mode = cmode_invalid;

        png_get_IHDR(png, pinfo, &width, &height, &bit_depth, &color_type, 0, 0, 0);

        png_set_strip_16(png);

        if (png_get_valid(png, pinfo, PNG_INFO_tRNS))
            png_set_tRNS_to_alpha(png);

        int32_t src_bits = 0;
        png_color_type_e type = (png_color_type_e)color_type;
        switch (type)
        {
        case png_color_type_index_rgb:
            png_set_palette_to_rgb(png);
            src_mode = cmode_b8g8r8;
            src_bits = 24;
            break;
        case png_color_type_gray:
            switch (bit_depth)
            {
            case 1:
                src_mode = cmode_gray1;
                break;
            case 2:
                src_mode = cmode_gray2;
                break;
            case 4:
                src_mode = cmode_gray4;
                break;
            case 8:
                src_mode = cmode_gray8;
                break;
            default:
                break;
            }
            src_bits = bit_depth;
            ///*png_set_gray_1_2_4_to_8*/png_set_expand_gray_1_2_4_to_8(png);
            break;
        case png_color_type_gray_alpha:
            png_set_gray_to_rgb(png);
            src_mode = cmode_b8g8r8;
            src_bits = 24;
            break;
        case png_color_type_rgb:
            src_mode = cmode_b8g8r8;
            src_bits = 24;
            break;
        case png_color_type_argb:
            src_mode = cmode_a8b8g8r8;
            src_bits = 32;
            break;
        default:
            break;
        }

        image_convert_rule_t rule = {image_format_png, (int32_t)width, (int32_t)height, src_mode, user_data};
        if (!pfn_match(&rule))
        {
            png_free(png, row_ptr);
            png_destroy_read_struct(&png, &pinfo, 0);
            return error_ok;
        }

        //png_set_bgr(png);
        int32_t src_pitch = align_to_4((width * src_bits + 7) / 8);
        int32_t src_length = src_pitch * height;

        row_ptr = (byte_t **)png_malloc(png, height * sizeof(png_bytep));
        byte_t * src_buffer = image_malloc(src_length);

        byte_t * line = src_buffer;
        for (png_uint_32 cnt = 0; cnt < height; ++cnt)
        {
            row_ptr[cnt] = line;
            line += src_pitch;
        }
        png_read_image(png, (png_byte **)row_ptr);
        png_read_end(png, 0);
        png_free(png, row_ptr);
        png_destroy_read_struct(&png, &pinfo, 0);

        if (rule.image_convert_fun == image_convert_png_use_src)
        {
            img->width = rule.width;
            img->height = rule.height;
            img->bits = rule.dst_bits;
            img->pitch = rule.dst_pitch;
            img->length = rule.dst_length;
            img->buffer = rule.dst_buffer;
            img->src_mode = rule.src_mode;
            img->dst_mode = rule.dst_mode;
            img->flags = 0;
            img->buffer = src_buffer;
        }
        else
        {
            img->width = rule.width;
            img->height = rule.height;
            img->bits = rule.dst_bits;
            img->pitch = rule.dst_pitch;
            img->length = rule.dst_length;
            img->buffer = rule.dst_buffer;
            img->src_mode = rule.src_mode;
            img->dst_mode = rule.dst_mode;
            img->flags = 0;
            img->buffer = image_malloc(rule.dst_length);

            rule.image_convert_fun(rule.width, rule.height,
                                   rule.pixel_convert_fun,
                                   nullptr, rule.pal_stride,
                                   src_buffer, rule.src_stride, rule.src_pitch,
                                   img->buffer, rule.dst_stride, rule.dst_pitch, 0);
            image_free(src_buffer);
        }
        return error_ok;
    }

    bool png_rule_default(image_convert_rule_t * rule)
    {
        if (!rule)
            return false;

        switch (rule->src_mode)
        {
        case cmode_gray8:
            rule->dst_mode = cmode_x8r8g8b8;
            rule->pixel_convert_fun = color_gray8_to_x8r8g8b8;
            rule->image_convert_fun = image_convert_ex;

            rule->src_bits = 8;
            rule->src_stride = 1;
            rule->src_pitch = align_to_4(rule->width * 1);

            rule->dst_bits = 32;
            rule->dst_stride = 4;
            rule->dst_pitch = rule->width * 4;

            rule->dst_length = rule->dst_pitch * rule->height;
            break;
        case cmode_x1r5g5b5:
            rule->dst_mode = cmode_x8r8g8b8;
            rule->pixel_convert_fun = color_x1r5g5b5_to_x8r8g8b8;
            rule->image_convert_fun = image_convert_ex;

            rule->src_bits = 16;
            rule->src_stride = 2;
            rule->src_pitch = align_to_4(rule->width * 2);

            rule->dst_bits = 32;
            rule->dst_stride = 4;
            rule->dst_pitch = rule->width * 4;

            rule->dst_length = rule->dst_pitch * rule->height;
            break;
        case cmode_a1r5g5b5:
            rule->dst_mode = cmode_a8r8g8b8;
            rule->pixel_convert_fun = color_a1r5g5b5_to_a8r8g8b8;
            rule->image_convert_fun = image_convert_ex;

            rule->src_bits = 16;
            rule->src_stride = 2;
            rule->src_pitch = align_to_4(rule->width * 2);

            rule->dst_bits = 32;
            rule->dst_stride = 4;
            rule->dst_pitch = align_to_4(rule->width * 4);

            rule->dst_length = rule->dst_pitch * rule->height;
            break;
        case cmode_r5g6b5:
            rule->dst_mode = cmode_x8r8g8b8;
            rule->pixel_convert_fun = color_r5g6b5_to_x8r8g8b8;
            rule->image_convert_fun = image_convert_ex;

            rule->src_bits = 16;
            rule->src_stride = 2;
            rule->src_pitch = align_to_4(rule->width * 2);

            rule->dst_bits = 32;
            rule->dst_stride = 4;
            rule->dst_pitch = rule->width * 4;

            rule->dst_length = rule->dst_pitch * rule->height;
            break;
        case cmode_r8g8b8:
            rule->dst_mode = cmode_x8r8g8b8;
            rule->pixel_convert_fun = color_r8g8b8_to_x8r8g8b8;
            rule->image_convert_fun = image_convert_ex;

            rule->src_bits = 24;
            rule->src_stride = 3;
            rule->src_pitch = align_to_4(rule->width * 3);

            rule->dst_bits = 32;
            rule->dst_stride = 4;
            rule->dst_pitch = rule->width * 4;

            rule->dst_length = rule->dst_pitch * rule->height;
            break;
        case cmode_b8g8r8:
            rule->dst_mode = cmode_x8r8g8b8;
            rule->pixel_convert_fun = color_b8g8r8_to_x8r8g8b8;
            rule->image_convert_fun = image_convert_ex;

            rule->src_bits = 24;
            rule->src_stride = 3;
            rule->src_pitch = align_to_4(rule->width * 3);

            rule->dst_bits = 32;
            rule->dst_stride = 4;
            rule->dst_pitch = rule->width * 4;

            rule->dst_length = rule->dst_pitch * rule->height;
            break;
        case cmode_x8r8g8b8:
            rule->dst_mode = cmode_x8r8g8b8;
            rule->pixel_convert_fun = nullptr;
            rule->image_convert_fun = image_convert_copy_ex;

            rule->src_bits = 32;
            rule->src_stride = 4;
            rule->src_pitch = rule->width * 4;

            rule->dst_bits = 32;
            rule->dst_stride = 4;
            rule->dst_pitch = rule->width * 4;

            rule->dst_length = rule->dst_pitch * rule->height;
            break;
        case cmode_x8b8g8r8:
            rule->dst_mode = cmode_x8r8g8b8;
            rule->pixel_convert_fun = color_x8b8g8r8_to_x8r8g8b8;
            rule->image_convert_fun = image_convert_ex;

            rule->src_bits = 32;
            rule->src_stride = 4;
            rule->src_pitch = rule->width * 4;

            rule->dst_bits = 32;
            rule->dst_stride = 4;
            rule->dst_pitch = rule->width * 4;

            rule->dst_length = rule->dst_pitch * rule->height;
            break;
        case cmode_a8r8g8b8:
            rule->dst_mode = cmode_a8r8g8b8;
            rule->pixel_convert_fun = nullptr;
            rule->image_convert_fun = image_convert_copy_ex;

            rule->src_bits = 32;
            rule->src_stride = 4;
            rule->src_pitch = rule->width * 4;

            rule->dst_bits = 32;
            rule->dst_stride = 4;
            rule->dst_pitch = rule->width * 4;

            rule->dst_length = rule->dst_pitch * rule->height;
            break;
        case cmode_a8b8g8r8:
            rule->dst_mode = cmode_a8r8g8b8;
            rule->pixel_convert_fun = color_a8b8g8r8_to_a8r8g8b8;
            rule->image_convert_fun = image_convert_ex;

            rule->src_bits = 32;
            rule->src_stride = 4;
            rule->src_pitch = rule->width * 4;

            rule->dst_bits = 32;
            rule->dst_stride = 4;
            rule->dst_pitch = rule->width * 4;

            rule->dst_length = rule->dst_pitch * rule->height;
            break;
        default:
            rule->image_convert_fun = nullptr;
            break;
        }
        return rule->image_convert_fun != nullptr;
    }
}}}
