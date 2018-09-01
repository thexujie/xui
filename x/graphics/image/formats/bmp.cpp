#include "stdafx.h"
#include "bmp.h"

//namespace graphics { namespace image { namespace formats
namespace graphics::image::formats
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

    core::error bmp_create(image_codec_context & ictx, const byte_t * buffer, int32_t length, image_data_t & image)
    {
        const bmp_simple_header_t * header = reinterpret_cast<const bmp_simple_header_t *>(buffer);
        if (header->type != BMP_MAGIC)
            return error_bad_format;

        image_format format = {};
        image_convert_fun pfn_convert = nullptr;
        const byte_t * conv_palette = nullptr;
        const byte_t * conv_buffer = nullptr;
        bool flip_y = false;

        // OS2 格式
        if (header->info_size == sizeof(bmp_info_os2_t))
        {
            const bmp_header_os2_t * info = reinterpret_cast<const bmp_header_os2_t *>(buffer);
            if (info->type != BMP_MAGIC)
                return error_bad_format;
            if (info->info_size != sizeof(bmp_info_os2_t))
                return error_bad_data;

            buffer += sizeof(bmp_header_os2_t);

            format.width = (int32_t)info->width;
            format.height = std::abs(info->height);
            flip_y = info->height > 0;
            if (format.height < 0)
                format.height = -format.height;

            conv_palette = nullptr;
            conv_buffer = buffer;

            bmp_encode_e encode = bmp_encode_none;
            switch (info->bits)
            {
            case 4:
                encode = bmp_encode_os2_index4;
                format.format = format_r8g8b8;
                pfn_convert = image_convert_index4_ex;
                conv_palette = buffer;
                conv_buffer = buffer + 16 * 3;
                break;
            case 8:
                encode = bmp_encode_os2_index8;
                format.format = format_r8g8b8;
                pfn_convert = image_convert_index8_ex;
                conv_palette = buffer;
                conv_buffer = buffer + 256 * 3;
                break;
            case 24:
                encode = bmp_encode_os2_24;
                format.format = format_r8g8b8;
                pfn_convert = image_convert_ex;
                conv_palette = nullptr;
                conv_buffer = buffer;
                break;
            default:
                pfn_convert = nullptr;
                conv_palette = nullptr;
                conv_buffer = buffer;
                break;
            }
        }
            // Windows 格式
        else
        {
            const bmp_header_windows_t * bmp_header = (const bmp_header_windows_t *)buffer;
            buffer += sizeof(bmp_header_windows_t);
            if (header->type != BMP_MAGIC)
                return error_bad_format;

            const bmp_info_windows_t * info = (const bmp_info_windows_t *)buffer;
            buffer += sizeof(bmp_info_windows_t);

            if (info->size != sizeof(bmp_info_windows_t) && info->size != sizeof(bmp_info_windows_t) + sizeof(
                color_mask_abgr_t))
                return error_bad_data;

            format.width = info->width;
            format.height = std::abs(info->height);
            flip_y = info->height > 0;

            // 调色板、颜色掩码等额外信息
            int32_t extern_size = header->data_offset - sizeof(bmp_info_windows_t) - sizeof(bmp_header_windows_t);

            conv_palette = nullptr;
            conv_buffer = nullptr;
            // 颜色掩码确定颜色类型
            if (info->encode == bmp_encode_mask)
            {
                assert(extern_size == sizeof(color_mask_abgr_t));
                color_mask_abgr_t * mask = (color_mask_abgr_t *)buffer;
                format.format = format_from_mask_abgr(*mask, info->bit_count);
                pfn_convert = image_convert_ex;
                conv_buffer = buffer + extern_size;
                conv_palette = nullptr;
            }
                // rle8 编码
            else if (info->encode == bmp_encode_rle8)
            {
                format.format = format_x8r8g8b8;
                pfn_convert = image_convert_bmp_index8_rle;
                conv_buffer = buffer + extern_size;
                conv_palette = buffer;
            }
                // rle4 编码
            else if (info->encode == bmp_encode_rle4)
            {
                format.format = format_x8r8g8b8;
                pfn_convert = image_convert_bmp_index4_rle;
                conv_buffer = buffer + extern_size;
                conv_palette = buffer;
            }
                // 有调色板
            else if (extern_size)
            {
                switch (info->bit_count)
                {
                case 1:
                    format.format = format_x8r8g8b8;
                    pfn_convert = image_convert_index1_ex;
                    break;
                case 2:
                    format.format = format_x8r8g8b8;
                    pfn_convert = image_convert_index2_ex;
                    break;
                case 4:
                    format.format = format_x8r8g8b8;
                    pfn_convert = image_convert_index4_ex;
                    break;
                case 8:
                    format.format = format_x8r8g8b8;
                    pfn_convert = image_convert_index8_ex;
                    break;
                default:
                    pfn_convert = nullptr;
                    break;
                }
                conv_buffer = buffer + extern_size;
                conv_palette = buffer;
            }
            else // 没有调色板
            {
                switch (info->bit_count)
                {
                case 2:
                    format.format = format_gray2;
                    pfn_convert = image_convert_ex;
                    break;
                case 4:
                    format.format = format_gray4;
                    pfn_convert = image_convert_ex;
                    break;
                case 8:
                    format.format = format_gray8;
                    pfn_convert = image_convert_ex;
                    break;
                case 16:
                    format.format = format_x1r5g5b5;
                    pfn_convert = image_convert_ex;
                    break;
                case 24:
                    format.format = format_r8g8b8;
                    pfn_convert = image_convert_ex;
                    break;
                case 32:
                    format.format = format_a8r8g8b8;
                    pfn_convert = image_convert_ex;
                    break;
                default:
                    pfn_convert = nullptr;
                    break;
                }
                conv_buffer = buffer;
                conv_palette = nullptr;
            }
        }

        if (!pfn_convert)
            return error_not_supported;

        image.format = format;
        if (ictx.get_format)
            image.format = ictx.get_format(image_type_bmp, format);

        ictx.pfn_alloc(image, 4);
        image.pfn_free = ictx.pfn_free;

        image_data_t src_data = {};
        src_data.format = format;
        src_data.data = const_cast<byte_t *>(conv_buffer);
        src_data.pitch = align_to<int32_t>(format.width * (format_bits(format.format) / 8), 4);
        if (flip_y)
            src_data.pitch = -src_data.pitch;
        src_data.palette = const_cast<byte_t *>(conv_palette);
        src_data.palette_size = 0;

        error err = pfn_convert(ictx, src_data, image);
        if(err < 0)
        {
            image.pfn_free(image);
            return err;
        }
        return error_ok;
    }

    core::error bmp_save(const image_data_t & data, std::string path)
    {
        color_mask_abgr_t mask = mask_from_format_abgr(data.format.format);
        if (!mask)
            return error_not_supported;

        std::fstream fs;
        fs.open(core::u8str_wstr(path), std::ios::out | std::ios::binary | std::ios::trunc);
        if (!fs.good())
            return error_io;

        image::formats::bmp_header_windows_t bmfHdr = {};
        bmfHdr.type = 0x4D42;
        bmfHdr.data_offset = sizeof(image::formats::bmp_header_windows_t) + sizeof(image::formats::bmp_info_windows_t) + sizeof(color_mask_abgr_t);
        bmfHdr.file_size = bmfHdr.data_offset + data.pitch * data.format.height;

        image::formats::bmp_info_windows_t bi = {};
        bi.size = sizeof(image::formats::bmp_info_windows_t);
        bi.width = data.format.width;
        bi.height = -data.format.height;
        bi.plane_count = 1;
        bi.bit_count = image::format_bits(data.format.format);
        bi.encode = image::formats::bmp_encode_mask;
        bi.image_size = 0;
        bi.ppm_x = 0;
        bi.ppm_y = 0;
        bi.color_used = 0;
        bi.color_important = 0;

        fs.write((const char *)&bmfHdr, sizeof(image::formats::bmp_header_windows_t));
        fs.write((const char *)&bi, sizeof(image::formats::bmp_info_windows_t));
        fs.write((const char *)&mask, sizeof(color_mask_abgr_t));
        fs.write((const char *)data.data, data.pitch * data.format.height);

        fs.close();
        return error_ok;
    }

    core::error image_convert_bmp_index4_rle(image_codec_context & icctx, const image_data_t & src, image_data_t & dst)
    {
        assert(dst.pitch > 0);
        pixel_convert_fun pfn_resampler = icctx.get_sampler ? icctx.get_sampler(src.format.format, dst.format.format) : image_get_samapler(src.format.format, dst.format.format);
        if (!pfn_resampler)
            return error_not_supported;

        byte_t * dst_line = dst.data;
        int32_t dst_pitch = dst.pitch;
        if (src.pitch < 0)
        {
            dst_line = dst.data + (dst.format.height - 1) * dst.pitch;
            dst_pitch = -dst_pitch;
        }

        int32_t src_stride = format_bits(src.format.format) / 8;
        int32_t dst_stride = format_bits(dst.format.format) / 8;

        const uint8_t * src_pixel = (const uint8_t *)src.data;
        byte_t * dst_pixel = dst_line;
        byte_t * dst_pixel_end = dst_pixel + dst.pitch;

        const byte_t * src_color1 = nullptr;
        const byte_t * src_color2 = nullptr;
        int32_t count = 0;
        bool working = true;
        while (working)
        {
            // 连续的、重复的
            if (*src_pixel)
            {
                count = *src_pixel++;
                src_color1 = src.palette + (*src_pixel >> 4) * src_stride;
                src_color2 = src.palette + (*src_pixel & 0xF) * src_stride;
                ++src_pixel;
                // dst_pixel < dst_pixel_end 这个条件本来是不需要的，
                // 但是，每行的末尾后面的一个像素都被计算进去，
                // （特别是最后一行）导致内存访问冲突。。。
                for (int32_t cnt = 0; (cnt < count) && (dst_pixel < dst_pixel_end); ++cnt, dst_pixel += dst_stride)
                {
                    if (cnt & 1)
                        pfn_resampler(src_color2, dst_pixel);
                    else
                        pfn_resampler(src_color1, dst_pixel);
                }
            }
            // 连续的、不重复的
            else
            {
                ++src_pixel;
                switch (*src_pixel)
                {
                case 0: // 换行
                    while (dst_pixel < dst_pixel_end)
                    {
                        pfn_resampler(src.palette, dst_pixel);
                        dst_pixel += dst_stride;
                    }
                    ++src_pixel;
                    dst_line += dst_pitch;
                    dst_pixel = dst_line;
                    dst_pixel_end = dst_line + dst.pitch;
                    break;
                case 1: // 解码结束
                    working = false;
                    break;
                case 2: // 偏移
                    ++src_pixel;
                    count = *src_pixel++;
                    while (count--)
                    {
                        pfn_resampler(src.palette, dst_pixel);
                        dst_pixel += dst_stride;
                    }

                    if (count = *src_pixel++) // 跳过 count 行
                    {
                        byte_t * skip_end = dst_pixel + count * dst_pitch;
                        while (dst_pixel < skip_end)
                        {
                            // 换行
                            if (dst_pixel == dst_pixel_end)
                            {
                                dst_line += dst_pitch;
                                dst_pixel = dst_line;
                                dst_pixel_end = dst_line + dst_stride;
                            }

                            pfn_resampler(src.palette, dst_pixel);
                            dst_pixel += dst_stride;
                        }
                    }
                    break;
                default: // 连续的、不重复的（Absolute PathMode）
                    count = *src_pixel++;
                    for (int32_t cnt = 0; cnt < count; ++cnt, dst_pixel += dst_stride)
                    {
                        if (cnt & 1)
                        {
                            src_color1 = src.palette + (*src_pixel & 0xF) * src_stride;
                            pfn_resampler(src_color1, dst_pixel);
                            ++src_pixel;
                        }
                        else
                        {
                            src_color2 = src.palette + (*src_pixel >> 4) * src_stride;
                            pfn_resampler(src_color2, dst_pixel);
                        }
                    }
                    if ((count >> 1) & 1)
                        ++src_pixel;
                    break;
                }
            }
        }

        return error_ok;
    }

    core::error image_convert_bmp_index8_rle(image_codec_context & icctx, const image_data_t & src, image_data_t & dst)
    {
        assert(dst.pitch > 0);
        pixel_convert_fun pfn_resampler = icctx.get_sampler ? icctx.get_sampler(src.format.format, dst.format.format) : image_get_samapler(src.format.format, dst.format.format);
        if (!pfn_resampler)
            return error_not_supported;

        byte_t * dst_line = dst.data;
        int32_t dst_pitch = dst.pitch;
        if (src.pitch < 0)
        {
            dst_line = dst.data + (dst.format.height - 1) * dst.pitch;
            dst_pitch = -dst_pitch;
        }

        int32_t src_stride = format_bits(src.format.format) / 8;
        int32_t dst_stride = format_bits(dst.format.format) / 8;

        const uint8_t * src_pixel = (const uint8_t *)src.data;

        byte_t * dst_pixel = dst_line;
        const byte_t * src_color = nullptr;
        byte_t * dst_pixel_end = dst_line + dst_pitch;
        int32_t count = 0;
        bool working = true;
        while (working)
        {
            // 连续的、重复的
            if (*src_pixel)
            {
                count = *src_pixel++;
                src_color = src.palette + (*src_pixel++) * src_stride;
                while (count--)
                {
                    pfn_resampler(src_color, dst_pixel);
                    dst_pixel += dst_stride;
                }
            }
            else
            {
                ++src_pixel;
                switch (*src_pixel)
                {
                case 0: // 换行，该行剩余dst_end - dst_pixel个像素，使用第0个颜色填充。
                    while (dst_pixel < dst_pixel_end)
                    {
                        pfn_resampler(src.palette, dst_pixel);
                        dst_pixel += dst_stride;
                    }
                    ++src_pixel;
                    dst_line += dst_pitch;
                    dst_pixel = dst_line;
                    dst_pixel_end = dst_line + dst.pitch;
                    break;
                case 1: // 解码结束
                    working = false;
                    break;
                case 2: // 偏移n个像素，该行前*src_index个像素，使用第0个颜色填充
                    ++src_pixel;
                    count = *src_pixel++;
                    while (count--)
                    {
                        pfn_resampler(src.palette, dst_pixel);
                        dst_pixel += dst_stride;
                    }

                    if (count = *src_pixel++) // 跳过 count 行
                    {
                        byte_t * dst_pixel_repeat_to = dst_pixel + count * dst_pitch;
                        while (dst_pixel < dst_pixel_repeat_to)
                        {
                            // 换行
                            if (dst_pixel == dst_pixel_end)
                            {
                                dst_line += dst_pitch;
                                dst_pixel = dst_line;
                                dst_pixel_end = dst_line + dst_pitch;
                            }

                            pfn_resampler(src.palette, dst_pixel);
                            dst_pixel += dst_stride;
                        }
                    }
                    break;
                default: // 连续的、不重复的
                    count = *src_pixel++;

                    for (int32_t cnt = 0; cnt < count; ++cnt)
                    {
                        src_color = src.palette + (*src_pixel++) * src_stride;
                        pfn_resampler(src_color, dst_pixel);
                        dst_pixel += dst_stride;
                    }

                    if (count & 1)
                        ++src_pixel;
                    break;
                }
            }
        }
        return error_ok;
    }
}
