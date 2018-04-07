#include "stdafx.h"
#include "tga.h"

namespace graphics { namespace image { namespace formats
{
    using namespace core;
    using namespace core;

    bool is_tga_data(const byte_t * buffer, int32_t length)
    {
        if (length < sizeof(tag_tga_header))
            return false;

        const tga_header_t * header = (const tga_header_t *)buffer;
        switch (header->color_type)
        {
        case tga_color_type_rgb:
        case tga_color_type_index:
            break;
        default:
            return false;
        }
        switch (header->commpress)
        {
        case tga_format_index:
        case tga_format_rgb:
        case tga_format_gray:
        case tga_format_rle_index:
        case tga_format_rle_rgb:
        case tga_format_rle_gray:
        case tga_format_hf_rle_index:
        case tga_format_hf_rle_rgb:
            break;
        default:
            return false;
        }
        return true;
    }


    core::error_e tga_create(const byte_t * buffer, int32_t length, image_data_t * img,
                       image_convert_rule_fun_t pfn_rule, void * user_data)
    {
        image_clear(img, sizeof(image_data_t));
        if (!pfn_rule)
            pfn_rule = tga_rule_default;

        const tga_header_t * header = (const tga_header_t *)buffer;
        buffer += sizeof(tga_header_t) + header->info_length/* 跳过图像信息字段 */;
        tga_format_e tga_format = (tga_format_e)header->commpress;

        int32_t width = (int32_t)header->width;
        int32_t height = (int32_t)header->height;
        int32_t flags = 0;
        // 从上到下存储
        if (!(header->flags & TGAF_FLIPY))
            flags |= IMAGE_CONVERT_FLIP_Y;

        const byte_t * conv_buffer = buffer;
        const byte_t * conv_palette = nullptr;
        cmode_e src_mode = cmode_none;
        switch (tga_format)
        {
        case tga_format_rgb:
            switch (header->bit_count)
            {
            case 8:
                src_mode = cmode_gray8;
                break;
            case 16:
                src_mode = cmode_x1r5g5b5;
                break;
            case 24:
                src_mode = cmode_r8g8b8;
                break;
            case 32:
                src_mode = cmode_a8r8g8b8;
                break;
            default:
                break;
            }
            break;
        case tga_format_gray:
            src_mode = cmode_gray8;
            break;
        case tga_format_index:
            conv_palette = buffer;
            switch (header->color_bit)
            {
            case 8:
                src_mode = cmode_index8_gray8;
                conv_buffer = buffer + header->color_count * 1;
                break;
            case 16:
                src_mode = cmode_index8_x1r5g5b5;
                conv_buffer = buffer + header->color_count * 2;
                break;
            case 24:
                src_mode = cmode_index8_r8g8b8;
                conv_buffer = buffer + header->color_count * 3;
                break;
            case 32:
                src_mode = cmode_index8_a8r8g8b8;
                conv_buffer = buffer + header->color_count * 4;
                break;
            default:
                break;
            }
            break;

        case tga_format_rle_gray:
            src_mode = cmode_rle_gray8;
            break;
        case tga_format_rle_rgb:
            switch (header->bit_count)
            {
            case 8:
                src_mode = cmode_rle_gray8;
                break;
            case 16:
                src_mode = cmode_rle_x1r5g5b5;
                break;
            case 24:
                src_mode = cmode_rle_r8g8b8;
                break;
            case 32:
                src_mode = cmode_rle_a8r8g8b8;
                break;
            default:
                break;
            }
            break;
        case tga_format_rle_index:
            //tga_rle_index(header, buffer, img, src_strike, dst_strike, conv_func);
            break;
        default:
            break;
        }

        image_convert_rule_t rule = {image_format_tga, width, height, src_mode, user_data};
        if (!pfn_rule(&rule))
            return error_bad_format;

        img->width = rule.width;
        img->height = rule.height;
        img->bits = rule.dst_bits;
        img->pitch = rule.dst_pitch;
        img->length = rule.dst_length;
        img->buffer = rule.dst_buffer;
        img->src_mode = rule.src_mode;
        img->dst_mode = rule.dst_mode;
        img->flags = flags;

        img->buffer = image_malloc(rule.dst_length);

        rule.image_convert_fun(rule.width, rule.height,
                               rule.pixel_convert_fun,
                               conv_palette, rule.pal_stride,
                               conv_buffer, rule.src_stride, rule.src_pitch,
                               img->buffer, rule.dst_stride, rule.dst_pitch, flags);
        return error_ok;
    }

    bool tga_rule_default(image_convert_rule_t * rule)
    {
        if (!rule)
            return false;

        switch (rule->src_mode)
        {
        case cmode_rle_a8r8g8b8:
            rule->dst_mode = cmode_a8r8g8b8;
            rule->pixel_convert_fun = color_32_to_32;
            rule->image_convert_fun = image_convert_tga_rle8;

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
            rule->pixel_convert_fun = color_32_to_32;
            rule->image_convert_fun = image_convert_copy_ex;

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

    void image_convert_tga_rle8(int32_t width, int32_t height,
                                pixel_convert_fun_t conv_fun,
                                const byte_t * pal, int32_t pal_stride,
                                const byte_t * src, int32_t src_stride, int32_t src_pitch,
        byte_t * dst, int32_t dst_stride, int32_t dst_pitch,
                                int32_t flags)
    {
        byte_t * dst_line = dst;
        const uint8_t * src_pixel = (const uint8_t *)src;
        byte_t * dst_pixel = nullptr;

        int32_t pitch = dst_pitch;
        if (flags & IMAGE_CONVERT_FLIP_Y)
        {
            dst_line += (height - 1) * pitch;
            pitch = -pitch;
        }

        int32_t state = 0;
        for (int32_t row = 0, col = 0; row != height; ++row)
        {
            dst_pixel = dst_line;
            for (col = 0; col != width; ++col)
            {
                if (state == 0)
                {
                    state = *src_pixel++;
                    if (state >= 0)
                        ++state;
                    else
                        state = -(0x81 + state);
                }
                else
                {
                }

                // 连续的、不重复的
                if (state > 0)
                {
                    conv_fun(src_pixel, dst_pixel);
                    dst_pixel += dst_stride;
                    src_pixel += src_stride;
                    --state;
                }
                    // 连续、重复的
                else
                {
                    conv_fun(src_pixel, dst_pixel);
                    dst_pixel += dst_stride;
                    ++state;
                    if (state == 0)
                        src_pixel += src_stride;
                }
            }
            dst_line += pitch;
        }
    }

    error_e tga_save(const image_data_t * data, image_save_write_fun_t pfn_write, void * userdata)
    {
        return tga_save_ex(data, pfn_write, userdata, tga_format_rgb);
    }

    error_e tga_save_ex(const image_data_t * data, image_save_write_fun_t pfn_write, void * userdata,
                      tga_format_e format)
    {
        if (!data || !pfn_write)
            return error_args;

        if (format == tga_format_rgb)
        {
            bool support = false;
            switch (data->bits)
            {
            case 8:
            case 16:
            case 24:
            case 32:
                support = true;
                break;
            default:
                break;
            }
            if (!support)
                return error_bad_format;

            tga_header_t header = {};
            header.color_type = tga_color_type_rgb;
            header.commpress = tga_format_rgb;
            header.width = (uint16_t)data->width;
            header.height = (uint16_t)data->height;
            header.bit_count = (uint8_t)data->bits;
            header.flags = TGAF_FLIPY;

            pfn_write(&header, sizeof(tga_header_t), userdata);
            pfn_write(data->buffer, data->length, userdata);
            pfn_write(TGA_TAIL, sizeof(TGA_TAIL), userdata); // 包括 null
            return error_ok;
        }
        return error_bad_format;
    }
}}}
