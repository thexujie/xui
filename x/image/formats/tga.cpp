#include "stdafx.h"
#include "tga.h"

namespace core::image::formats
{
    using namespace core;

    bool is_tga_data(const byte_t * buffer, size_t length)
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


    core::error tga_create(image_codec_context & ictx, const byte_t * buffer, size_t length, image_data_t & image)
    {
        const tga_header_t * header = reinterpret_cast<const tga_header_t *>(buffer);
        buffer += sizeof(tga_header_t) + header->info_length/* 跳过图像信息字段 */;
        tga_format_e tga_format = (tga_format_e)header->commpress;

        image_convert_fun pfn_convert = nullptr;
        image_format format = {};
        format.width = header->width;
        format.height = header->height;

        const byte_t * conv_buffer = buffer;
        const byte_t * conv_palette = nullptr;
        switch (tga_format)
        {
        case tga_format_rgb:
            switch (header->bit_count)
            {
            case 8:
                format.format = format::gray8;
                pfn_convert = image_convert_ex;
                break;
            case 16:
                format.format = format::x1r5g5b5;
                pfn_convert = image_convert_ex;
                break;
            case 24:
                format.format = format::r8g8b8;
                pfn_convert = image_convert_ex;
                break;
            case 32:
                format.format = format::a8r8g8b8;
                pfn_convert = image_convert_ex;
                break;
            default:
                pfn_convert = nullptr;
                break;
            }
            break;
        case tga_format_gray:
            format.format = format::gray8;
            pfn_convert = image_convert_ex;
            break;
        case tga_format_index:
            conv_palette = buffer;
            switch (header->color_bit)
            {
            case 8:
                format.format = format::gray8;
                pfn_convert = image_convert_index8_ex;
                conv_buffer = buffer + header->color_count * 1;
                break;
            case 16:
                format.format = format::x1r5g5b5;
                pfn_convert = image_convert_index8_ex;
                conv_buffer = buffer + header->color_count * 2;
                break;
            case 24:
                format.format = format::r8g8b8;
                pfn_convert = image_convert_index8_ex;
                conv_buffer = buffer + header->color_count * 3;
                break;
            case 32:
                format.format = format::a8r8g8b8;
                pfn_convert = image_convert_index8_ex;
                conv_buffer = buffer + header->color_count * 4;
                break;
            default:
                pfn_convert = nullptr;
                break;
            }
            break;

        case tga_format_rle_gray:
            pfn_convert = nullptr;
            break;
        case tga_format_rle_rgb:
            switch (header->bit_count)
            {
            case 8:
                format.format = format::gray8;
                pfn_convert = image_convert_tga_rle8;
                break;
            case 16:
                format.format = format::x1r5g5b5;
                pfn_convert = image_convert_tga_rle8;
                break;
            case 24:
                format.format = format::r8g8b8;
                pfn_convert = image_convert_tga_rle8;
                break;
            case 32:
                format.format = format::a8r8g8b8;
                pfn_convert = image_convert_tga_rle8;
                break;
            default:
                pfn_convert = nullptr;
                break;
            }
            break;
        case tga_format_rle_index:
            //tga_rle_index(header, buffer, img, src_strike, dst_strike, conv_func);
            break;
        default:
            pfn_convert = nullptr;
            break;
        }

        if (!pfn_convert)
            return e_not_supported;

        image.format = format;
        if (ictx.get_format)
            image.format = ictx.get_format(image_type_bmp, format);

        ictx.pfn_alloc(image, 4);
        image.pfn_free = ictx.pfn_free;

        image_data_t src_data = {};
        src_data.format = format;
        src_data.pitch = align_to<int32_t>(format.width * (format_bits(format.format) / 8), 4);
        src_data.data = (byte_t *)conv_buffer;
        src_data.palette = (byte_t *)conv_palette;

        if (!(header->flags & TGAF_TOP_TO_BOTTOM))
            src_data.pitch = -src_data.pitch;

        error err = pfn_convert(ictx, src_data, image);
        if (err < 0)
        {
            image.pfn_free(image);
            return err;
        }
        return ok;
    }

    core::error tga_save(const image_data_t & data, const std::u8string & path)
    {
        assert(data.pitch > 0);
        switch (format_bits(data.format.format))
        {
        case 8:
        case 16:
        case 24:
        case 32:
            break;
        default:
            return e_not_supported;
        }

        std::fstream fs;
        fs.open(core::u8str_wstr(path), std::ios::out | std::ios::binary | std::ios::trunc);
        if (!fs.good())
            return e_io;

        tga_header_t header = {};
        header.color_type = tga_color_type_rgb;
        header.commpress = data.format.format == format::gray8 ? tga_format_gray : tga_format_rgb;
        header.width = (uint16_t)data.format.width;
        header.height = (uint16_t)data.format.height;
        header.bit_count = (uint8_t)format_bits(data.format.format);
        header.flags = TGAF_TOP_TO_BOTTOM;

        fs.write((const char *)&header, sizeof(tga_header_t));
        int32_t row_pitch = format_bits(data.format.format) * data.format.width / 8;
        for(int32_t row = 0; row < data.format.height; ++row)
            fs.write((const char *)data.data + data.pitch * row, row_pitch);
        fs.write((const char *)TGA_TAIL, sizeof(TGA_TAIL)); // 包括 null
        return ok;
    }

    core::error image_convert_tga_rle8(image_codec_context & icctx, const image_data_t & src, image_data_t & dst)
    {
        pixel_convert_fun pfn_resampler = icctx.get_sampler ? icctx.get_sampler(src.format.format, dst.format.format) : image_get_samapler(src.format.format, dst.format.format);
        if (!pfn_resampler)
            return e_not_supported;

        int32_t src_stride = format_bits(src.format.format) / 8;
        int32_t dst_stride = format_bits(dst.format.format) / 8;
        const byte_t * src_pixel = src.data;
        byte_t * dst_line = dst.data;
        byte_t * dst_pixel = nullptr;

        int32_t dst_pitch = dst.pitch;
        if (src.pitch < 0)
        {
            dst_line = dst.data + (src.format.height - 1) * dst.pitch;
            dst_pitch = -dst_pitch;
        }



        int32_t state = 0;
        for (int32_t row = 0, col = 0; row != src.format.height; ++row)
        {
            dst_pixel = dst_line;
            for (col = 0; col != src.format.width; ++col)
            {
                if (state == 0)
                {
                    state = *src_pixel++;
                    if (state >= 0)
                        ++state;
                    else
                        state = -(0x81 + state);
                }
                else { }

                // 连续的、不重复的
                if (state > 0)
                {
                    pfn_resampler(src_pixel, dst_pixel);
                    dst_pixel += dst_stride;
                    src_pixel += src_stride;
                    --state;
                }
                    // 连续、重复的
                else
                {
                    pfn_resampler(src_pixel, dst_pixel);
                    dst_pixel += dst_stride;
                    ++state;
                    if (state == 0)
                        src_pixel += src_stride;
                }
            }
            dst_line += dst_pitch;
        }
        return ok;
    }
}
