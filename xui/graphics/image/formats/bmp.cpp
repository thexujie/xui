#include "stdafx.h"
#include "bmp.h"

namespace graphics { namespace image { namespace formats
{
    using namespace core;

    // 做 2 个字节的测试
    bool is_bmp_data(const byte_t * buffer, int32_t length)
    {
        if (length < 3)
            return false;

        for (int cnt = 0; cnt < sizeof(BMP_HEADER); ++cnt)
        {
            if ((uint8_t)buffer[cnt] != BMP_HEADER[cnt])
                return false;
        }
        return true;
    }

    core::error_e bmp_create(const byte_t * buffer, int32_t length, image_data_t * bmp, image_convert_rule_fun_t pfn_match,
                       void * user_data)
    {
        if (!pfn_match)
            pfn_match = bmp_rule_default;

        image_clear(bmp, sizeof(image_data_t));

        const bmp_simple_header_t * header = (const bmp_simple_header_t *)buffer;
        if (header->type != BMP_MAGIC)
            return error_bad_format;

        int32_t width = 0;
        int32_t height = 0;
        cmode_e src_mode = cmode_none;
        const byte_t * conv_palette = nullptr;
        const byte_t * conv_buffer = nullptr;
        int32_t flags = 0;

        // OS2 格式
        if (header->info_size == sizeof(bmp_info_os2_t))
        {
            const bmp_header_os2_t * info = (const bmp_header_os2_t *)buffer;
            if (info->type != BMP_MAGIC)
                return error_bad_format;
            if (info->info_size != sizeof(bmp_info_os2_t))
                return error_bad_data;

            buffer += sizeof(bmp_header_os2_t);

            width = (int32_t)info->width;
            height = (int32_t)info->height;
            if (height < 0)
                height = -height;
            else
                flags |= IMAGE_CONVERT_FLIP_Y;

            conv_palette = nullptr;
            conv_buffer = buffer;

            bmp_encode_e encode = bmp_encode_none;
            switch (info->bits)
            {
            case 4:
                encode = bmp_encode_os2_index4;
                src_mode = cmode_index4_r8g8b8;
                conv_palette = buffer;
                conv_buffer = buffer + 16 * 3;
                break;
            case 8:
                encode = bmp_encode_os2_index8;
                src_mode = cmode_index8_r8g8b8;
                conv_palette = buffer;
                conv_buffer = buffer + 256 * 3;
                break;
            case 24:
                encode = bmp_encode_os2_24;
                src_mode = cmode_r8g8b8;
                conv_palette = nullptr;
                conv_buffer = buffer;
                break;
            default:
                conv_palette = nullptr;
                conv_buffer = buffer;
                break;
            }
        }
            // Windows 格式
        else
        {
            const bmp_header_windows_t * header = (const bmp_header_windows_t *)buffer;
            buffer += sizeof(bmp_header_windows_t);
            if (header->type != BMP_MAGIC)
                return error_bad_format;

            const bmp_info_windows_t * info = (const bmp_info_windows_t *)buffer;
            buffer += sizeof(bmp_info_windows_t);

            if (info->size != sizeof(bmp_info_windows_t) && info->size != sizeof(bmp_info_windows_t) + sizeof(
                color_mask_abgr_t))
                return error_bad_data;

            width = (int32_t)info->width;
            height = (int32_t)info->height;

            if (height < 0)
                height = -height;
            else
                flags |= IMAGE_CONVERT_FLIP_Y;

            // 调色板、颜色掩码等额外信息
            int32_t extern_size = header->data_offset - sizeof(bmp_info_windows_t) - sizeof(bmp_header_windows_t);

            conv_palette = nullptr;
            conv_buffer = buffer;
            // 颜色掩码确定颜色类型
            if (info->encode == bmp_encode_mask)
            {
                assert(extern_size == sizeof(color_mask_abgr_t));
                color_mask_abgr_t * mask = (color_mask_abgr_t *)buffer;
                src_mode = cmode_from_mask_abgr(*mask, info->bit_count);
                conv_buffer = buffer + extern_size;
                conv_palette = nullptr;
            }
                // rle8 编码
            else if (info->encode == bmp_encode_rle8)
            {
                src_mode = cmode_index8_rle2_x8r8g8b8;
                conv_buffer = buffer + extern_size;
                conv_palette = buffer;
            }
                // rle4 编码
            else if (info->encode == bmp_encode_rle4)
            {
                src_mode = cmode_index4_rle2_x8r8g8b8;
                conv_buffer = buffer + extern_size;
                conv_palette = buffer;
            }
                // 有调色板
            else if (extern_size)
            {
                src_mode = cmode_none;
                switch (info->bit_count)
                {
                case 1:
                    src_mode = cmode_index1_x8r8g8b8;
                    break;
                case 2:
                    src_mode = cmode_index2_x8r8g8b8;
                    break;
                case 4:
                    src_mode = cmode_index4_x8r8g8b8;
                    break;
                case 8:
                    src_mode = cmode_index8_x8r8g8b8;
                    break;
                default:
                    break;
                }
                conv_buffer = buffer + extern_size;
                conv_palette = buffer;
            }
            else // 没有调色板
            {
                src_mode = cmode_none;
                switch (info->bit_count)
                {
                case 2:
                    src_mode = cmode_gray2;
                    break;
                case 4:
                    src_mode = cmode_gray4;
                    break;
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
                conv_buffer = buffer;
                conv_palette = nullptr;
            }
        }

        image_convert_rule_t rule = {image_format_bmp, width, height, src_mode, user_data};
        if (!pfn_match(&rule))
            return error_bad_format;
        bmp->width = rule.width;
        bmp->height = rule.height;
        bmp->bits = rule.dst_bits;
        bmp->pitch = rule.dst_pitch;
        bmp->length = rule.dst_length;
        bmp->buffer = rule.dst_buffer;
        bmp->src_mode = rule.src_mode;
        bmp->dst_mode = rule.dst_mode;
        bmp->flags = flags;
        bmp->buffer = image_malloc(bmp->length);

        rule.image_convert_fun(rule.width, rule.height,
                               rule.pixel_convert_fun,
                               conv_palette, rule.pal_stride,
                               conv_buffer, rule.src_stride, rule.src_pitch,
                               bmp->buffer, rule.dst_stride, rule.dst_pitch, flags);
        return error_ok;
    }

    bool bmp_rule_default(image_convert_rule_t * rule)
    {
        if (!rule)
            return false;

        switch (rule->src_mode)
        {
        case cmode_x4r4g4b4:
            rule->dst_mode = cmode_x1r5g5b5;
            rule->pixel_convert_fun = color_x4r4g4b4_to_x1r5g5b5;
            rule->image_convert_fun = image_convert_ex;

            rule->pal_bits = 0;
            rule->pal_stride = 0;

            rule->src_bits = 16;
            rule->src_stride = 2;
            rule->src_pitch = align_to_4(rule->width * 2);

            rule->dst_bits = 16;
            rule->dst_stride = 2;
            rule->dst_pitch = align_to_4(rule->width * 2);
            rule->dst_length = rule->dst_pitch * rule->height;
            break;
        case cmode_a4r4g4b4:
            rule->dst_mode = cmode_a8b8g8r8;
            rule->pixel_convert_fun = color_a4r4g4b4_to_a8b8g8r8;
            rule->image_convert_fun = image_convert_ex;

            rule->pal_bits = 0;
            rule->pal_stride = 0;

            rule->src_bits = 16;
            rule->src_stride = 2;
            rule->src_pitch = align_to_4(rule->width * 2);

            rule->dst_bits = 32;
            rule->dst_stride = 4;
            rule->dst_pitch = rule->width * 4;
            rule->dst_length = rule->dst_pitch * rule->height;
            break;
        case cmode_x1r5g5b5:
            rule->dst_mode = cmode_x1r5g5b5;
            rule->pixel_convert_fun = nullptr;
            rule->image_convert_fun = image_convert_copy_ex;

            rule->pal_bits = 0;
            rule->pal_stride = 0;

            rule->src_bits = 16;
            rule->src_stride = 2;
            rule->src_pitch = align_to_4(rule->width * 2);

            rule->dst_bits = 16;
            rule->dst_stride = 2;
            rule->dst_pitch = align_to_4(rule->width * 2);
            rule->dst_length = rule->dst_pitch * rule->height;
            break;
        case cmode_r5g6b5:
            rule->dst_mode = cmode_r8g8b8;
            rule->pixel_convert_fun = color_r5g6b5_to_r8g8b8;
            rule->image_convert_fun = image_convert_ex;

            rule->pal_bits = 0;
            rule->pal_stride = 0;

            rule->src_bits = 16;
            rule->src_stride = 2;
            rule->src_pitch = align_to_4(rule->width * 2);

            rule->dst_bits = 24;
            rule->dst_stride = 3;
            rule->dst_pitch = align_to_4(rule->width * 3);
            rule->dst_length = rule->dst_pitch * rule->height;
            break;
        case cmode_r8g8b8:
            rule->dst_mode = cmode_r8g8b8;
            rule->pixel_convert_fun = nullptr;
            rule->image_convert_fun = image_convert_copy_ex;

            rule->pal_bits = 0;
            rule->pal_stride = 0;

            rule->src_bits = 24;
            rule->src_stride = 3;
            rule->src_pitch = align_to_4(rule->width * 3);

            rule->dst_bits = 24;
            rule->dst_stride = 3;
            rule->dst_pitch = align_to_4(rule->width * 3);
            rule->dst_length = rule->dst_pitch * rule->height;
            break;
        case cmode_x8r8g8b8:
            rule->dst_mode = cmode_r8g8b8;
            rule->pixel_convert_fun = color_x8r8g8b8_to_r8g8b8;
            rule->image_convert_fun = image_convert_ex;

            rule->pal_bits = 0;
            rule->pal_stride = 0;

            rule->src_bits = 32;
            rule->src_stride = 4;
            rule->src_pitch = rule->width * 4;

            rule->dst_bits = 24;
            rule->dst_stride = 3;
            rule->dst_pitch = align_to_4(rule->width * 3);
            rule->dst_length = rule->dst_pitch * rule->height;
            break;
        case cmode_a8r8g8b8:
            rule->dst_mode = cmode_a8r8g8b8;
            rule->pixel_convert_fun = nullptr;
            rule->image_convert_fun = image_convert_copy_ex;

            rule->pal_bits = 0;
            rule->pal_stride = 0;

            rule->src_bits = 32;
            rule->src_stride = 4;
            rule->src_pitch = rule->width * 4;

            rule->dst_bits = 32;
            rule->dst_stride = 4;
            rule->dst_pitch = rule->width * 4;
            rule->dst_length = rule->dst_pitch * rule->height;
            break;
        case cmode_r8g8b8x8:
            rule->dst_mode = cmode_r8g8b8;
            rule->pixel_convert_fun = color_r8g8b8x8_to_r8g8b8;
            rule->image_convert_fun = image_convert_ex;

            rule->pal_bits = 0;
            rule->pal_stride = 0;

            rule->src_bits = 32;
            rule->src_stride = 4;
            rule->src_pitch = rule->width * 4;

            rule->dst_bits = 24;
            rule->dst_stride = 3;
            rule->dst_pitch = align_to_4(rule->width * 3);
            rule->dst_length = rule->dst_pitch * rule->height;
            break;

        case cmode_index1_x8r8g8b8:
            rule->dst_mode = cmode_r8g8b8;
            rule->pixel_convert_fun = color_x8r8g8b8_to_r8g8b8;
            rule->image_convert_fun = image_convert_index1_ex;

            rule->pal_bits = 32;
            rule->pal_stride = 4;

            rule->src_bits = 1;
            rule->src_stride = 1;
            rule->src_pitch = align_to_4((rule->width + 7) / 8);

            rule->dst_bits = 24;
            rule->dst_stride = 3;
            rule->dst_pitch = align_to_4(rule->width * 3);

            rule->dst_length = rule->dst_pitch * rule->height;
            break;
        case cmode_index4_r8g8b8:
            rule->dst_mode = cmode_r8g8b8;
            rule->pixel_convert_fun = color_24_to_24;
            rule->image_convert_fun = image_convert_index4_ex;

            rule->pal_bits = 24;
            rule->pal_stride = 3;

            rule->src_bits = 4;
            rule->src_stride = 1;
            rule->src_pitch = align_to_4((rule->width * 4 + 7) / 8);

            rule->dst_bits = 24;
            rule->dst_stride = 3;
            rule->dst_pitch = align_to_4(rule->width * 3);

            rule->dst_length = rule->dst_pitch * rule->height;
            break;
        case cmode_index4_x8r8g8b8:
            rule->dst_mode = cmode_r8g8b8;
            rule->pixel_convert_fun = color_x8r8g8b8_to_r8g8b8;
            rule->image_convert_fun = image_convert_index4_ex;

            rule->pal_bits = 32;
            rule->pal_stride = 4;

            rule->src_bits = 4;
            rule->src_stride = 1;
            rule->src_pitch = align_to_4((rule->width * 4 + 7) / 8);

            rule->dst_bits = 24;
            rule->dst_stride = 3;
            rule->dst_pitch = align_to_4(rule->width * 3);

            rule->dst_length = rule->dst_pitch * rule->height;
            break;
        case cmode_index8_r8g8b8:
            rule->dst_mode = cmode_r8g8b8;
            rule->pixel_convert_fun = color_24_to_24;
            rule->image_convert_fun = image_convert_index8_ex;

            rule->pal_bits = 24;
            rule->pal_stride = 3;

            rule->src_bits = 8;
            rule->src_stride = 1;
            rule->src_pitch = align_to_4(rule->width);

            rule->dst_bits = 24;
            rule->dst_stride = 3;
            rule->dst_pitch = align_to_4(rule->width * 3);

            rule->dst_length = rule->dst_pitch * rule->height;
            break;
        case cmode_index8_x8r8g8b8:
            rule->dst_mode = cmode_r8g8b8;
            rule->pixel_convert_fun = color_x8r8g8b8_to_r8g8b8;
            rule->image_convert_fun = image_convert_index8_ex;

            rule->pal_bits = 32;
            rule->pal_stride = 4;

            rule->src_bits = 8;
            rule->src_stride = 1;
            rule->src_pitch = align_to_4(rule->width);

            rule->dst_bits = 24;
            rule->dst_stride = 3;
            rule->dst_pitch = align_to_4(rule->width * 3);

            rule->dst_length = rule->dst_pitch * rule->height;
            break;
        case cmode_index4_rle2_x8r8g8b8:
            rule->dst_mode = cmode_r8g8b8;
            rule->pixel_convert_fun = color_x8r8g8b8_to_r8g8b8;
            rule->image_convert_fun = image_convert_bmp_index4_rle;

            rule->pal_bits = 32;
            rule->pal_stride = 4;

            rule->src_bits = 0;
            rule->src_stride = 0;
            rule->src_pitch = 0;

            rule->dst_bits = 24;
            rule->dst_stride = 3;
            rule->dst_pitch = align_to_4(rule->width * 3);

            rule->dst_length = rule->dst_pitch * rule->height;
            break;
        case cmode_index8_rle2_x8r8g8b8:
            rule->dst_mode = cmode_r8g8b8;
            rule->pixel_convert_fun = color_x8r8g8b8_to_r8g8b8;
            rule->image_convert_fun = image_convert_bmp_index8_rle;

            rule->pal_bits = 32;
            rule->pal_stride = 4;

            rule->src_bits = 0;
            rule->src_stride = 0;
            rule->src_pitch = 0;

            rule->dst_bits = 24;
            rule->dst_stride = 3;
            rule->dst_pitch = align_to_4(rule->width * 3);

            rule->dst_length = rule->dst_pitch * rule->height;
            break;
        default:
            rule->image_convert_fun = nullptr;
            break;
        }
        return rule->image_convert_fun != nullptr;
    }

    void image_convert_bmp_index4_rle(int32_t width, int32_t height,
                                      pixel_convert_fun_t conv_fun,
                                      const byte_t * pal, int32_t pal_stride,
                                      const byte_t * src, int32_t /*src_stride*/, int32_t /*src_pitch*/,
        byte_t * dst, int32_t dst_stride, int32_t dst_pitch,
                                      int32_t flags)
    {
        byte_t * dst_line = dst;
        int32_t pitch = dst_pitch;
        if (flags & IMAGE_CONVERT_FLIP_Y)
        {
            dst_line += (height - 1) * pitch;
            pitch = -pitch;
        }

        const byte_t * src_color1 = nullptr;
        const byte_t * src_color2 = nullptr;
        const byte_t * src_pixel = src;
        byte_t * dst_pixel = dst_line;
        byte_t * dst_end = dst_pixel + dst_pitch;
        int32_t count = 0;
        bool working = true;
        while (working)
        {
            // 连续的、重复的
            if (*src_pixel)
            {
                count = *src_pixel++;
                src_color1 = pal + (*src_pixel >> 4) * pal_stride;
                src_color2 = pal + (*src_pixel & 0xF) * pal_stride;
                ++src_pixel;
                // dst_pixel < dst_end 这个条件本来是不需要的，
                // 但是，每行的末尾后面的一个像素都被计算进去，
                // （特别是最后一行）导致内存访问冲突。。。
                for (int32_t cnt = 0; (cnt < count) && (dst_pixel < dst_end); ++cnt, dst_pixel += dst_stride)
                {
                    if (cnt & 1)
                        conv_fun(src_color2, dst_pixel);
                    else
                        conv_fun(src_color1, dst_pixel);
                }
            }
                // 连续的、不重复的
            else
            {
                ++src_pixel;
                switch (*src_pixel)
                {
                case 0: // 换行
                    while (dst_pixel < dst_end)
                    {
                        conv_fun(pal, dst_pixel);
                        dst_pixel += dst_stride;
                    }
                    ++src_pixel;
                    dst_line += pitch;
                    dst_pixel = dst_line;
                    dst_end = dst_line + dst_pitch;
                    break;
                case 1: // 解码结束
                    working = false;
                    break;
                case 2: // 偏移
                    ++src_pixel;
                    count = *src_pixel++;
                    while (count--)
                    {
                        conv_fun(pal, dst_pixel);
                        dst_pixel += dst_stride;
                    }

                    if (count = *src_pixel++) // 跳过 count 行
                    {
                        byte_t * skip_end = dst_pixel + count * pitch;
                        while (dst_pixel < skip_end)
                        {
                            // 换行
                            if (dst_pixel >= dst_end)
                            {
                                dst_line += pitch;
                                dst_pixel = dst_line;
                                dst_end = dst_line + dst_stride;
                            }
                            else
                            {
                                conv_fun(pal, dst_pixel);
                                dst_pixel += dst_stride;
                            }
                        }
                    }
                    break;
                default: // 连续的、不重复的（Absolute Mode）
                    count = *src_pixel++;
                    for (int32_t cnt = 0; cnt < count; ++cnt, dst_pixel += dst_stride)
                    {
                        if (cnt & 1)
                        {
                            src_color1 = pal + (*src_pixel & 0xF) * pal_stride;
                            conv_fun(src_color1, dst_pixel);
                            ++src_pixel;
                        }
                        else
                        {
                            src_color2 = pal + (*src_pixel >> 4) * pal_stride;
                            conv_fun(src_color2, dst_pixel);
                        }
                    }
                    if ((count >> 1) & 1)
                        ++src_pixel;
                    break;
                }
            }
        }
    }

    void image_convert_bmp_index8_rle(int32_t width, int32_t height,
                                      pixel_convert_fun_t conv_fun,
                                      const byte_t * pal, int32_t pal_stride,
                                      const byte_t * src, int32_t /*src_stride*/, int32_t /*src_pitch*/,
                                      byte_t * dst, int32_t dst_stride, int32_t dst_pitch,
                                      int32_t flags)
    {
        byte_t * dst_line = dst;
        int32_t pitch = dst_pitch;
        if (flags & IMAGE_CONVERT_FLIP_Y)
        {
            dst_line += (height - 1) * pitch;
            pitch = -pitch;
        }

        const byte_t * src_color = nullptr;
        const byte_t * src_pixel = src;
        byte_t * dst_pixel = dst_line;
        byte_t * dst_end = dst_line + dst_pitch;
        int32_t count = 0;
        bool working = true;
        while (working)
        {
            // 连续的、重复的
            if (*src_pixel)
            {
                count = *src_pixel++;
                src_color = pal + (*src_pixel++) * pal_stride;
                while (count--)
                {
                    conv_fun(src_color, dst_pixel);
                    dst_pixel += dst_stride;
                }
            }
            else
            {
                ++src_pixel;
                switch (*src_pixel)
                {
                case 0: // 换行，该行剩余dst_end - dst_pixel个像素，使用第0个颜色填充。
                    while (dst_pixel < dst_end)
                    {
                        conv_fun(pal, dst_pixel);
                        dst_pixel += dst_stride;
                    }
                    ++src_pixel;
                    dst_line += pitch;
                    dst_pixel = dst_line;
                    dst_end = dst_line + dst_pitch;
                    break;
                case 1: // 解码结束
                    working = false;
                    break;
                case 2: // 偏移n个像素，该行前*src_index个像素，使用第0个颜色填充
                    ++src_pixel;
                    count = *src_pixel++;
                    while (count--)
                    {
                        conv_fun(pal, dst_pixel);
                        dst_pixel += dst_stride;
                    }

                    // 再次重复count个像素。
                    if (count = *src_pixel++)
                    {
                        byte_t * end = dst_pixel + count * pitch;
                        while (dst_pixel < end)
                        {
                            // 换行
                            if (dst_pixel >= dst_end)
                            {
                                dst_line += pitch;
                                dst_pixel = dst_line;
                                dst_end = dst_line + dst_pitch;
                            }
                            else
                            {
                                conv_fun(pal, dst_pixel);
                                dst_pixel += dst_stride;
                            }
                        }
                    }
                    break;
                default: // 连续的、不重复的
                    count = *src_pixel++;

                    for (int32_t cnt = 0; cnt < count; ++cnt)
                    {
                        src_color = pal + (*src_pixel++) * pal_stride;
                        conv_fun(src_color, dst_pixel);
                        dst_pixel += dst_stride;
                    }

                    if (count & 1)
                        ++src_pixel;
                    break;
                }
            }
        }
    }
}}}
