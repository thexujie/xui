#include "stdafx.h"
#include "dds.h"

namespace graphics::image::formats
{
    using namespace core;

    bool is_dds_data(const byte_t * buffer, int32_t length)
    {
        if (length < sizeof(dds_header_t))
            return false;
        const dds_header_t * header = (const dds_header_t *)buffer;
        if (header->magic != DDS_MAGIC)
            return false;

        if (sizeof(header->magic) + header->size != sizeof(dds_header_t))
            return false;

        if (header->pixel_format.size != sizeof(dds_pixel_format_t))
            return false;

        return true;
    }

    void dxt_make_color(uint16_t color0, uint16_t color1, uint16_t colors[4])
    {
        colors[0] = color0;
        colors[1] = color1;

        colors[2] =
            ((((color0 & 0xF800) * 2 + (color1 & 0xF800) * 1 + 1) / 3) & 0xF800) |
            ((((color0 & 0x07E0) * 2 + (color1 & 0x07E0) * 1 + 1) / 3) & 0x07E0) |
            ((((color0 & 0x001F) * 2 + (color1 & 0x001F) * 1 + 1) / 3) & 0x001F);
        colors[3] =
            ((((color0 & 0xF800) * 1 + (color1 & 0xF800) * 2 + 1) / 3) & 0xF800) |
            ((((color0 & 0x07E0) * 1 + (color1 & 0x07E0) * 2 + 1) / 3) & 0x07E0) |
            ((((color0 & 0x001F) * 1 + (color1 & 0x001F) * 2 + 1) / 3) & 0x001F);
    }

    static void bc3_calc_alpha(byte_t alpha0, byte_t alpha1, byte_t alphas[8])
    {
        alphas[0] = alpha0;
        alphas[1] = alpha1;

        if (alpha0 > alpha1)
        {
            alphas[2] = (alpha0 * 6 + alpha1 * 1) / 7;
            alphas[3] = (alpha0 * 5 + alpha1 * 2) / 7;
            alphas[4] = (alpha0 * 4 + alpha1 * 3) / 7;
            alphas[5] = (alpha0 * 3 + alpha1 * 4) / 7;
            alphas[6] = (alpha0 * 2 + alpha1 * 5) / 7;
            alphas[7] = (alpha0 * 1 + alpha1 * 6) / 7;
        }
        else
        {
            alphas[2] = (alpha0 * 4 + alpha1 * 1) / 5;
            alphas[3] = (alpha0 * 3 + alpha1 * 2) / 5;
            alphas[4] = (alpha0 * 2 + alpha1 * 3) / 5;
            alphas[5] = (alpha0 * 1 + alpha1 * 4) / 5;

            alphas[6] = 0;
            alphas[7] = 0xFF;
        }
    }

    static void bc3_calc_alpha_indices(uint16_t index0, uint16_t index1, uint16_t index2, byte_t indices[4][4])
    {
        indices[0][0] = index0 & 0x7;
        indices[0][1] = (index0 >> 3) & 0x7;
        indices[0][2] = (index0 >> 6) & 0x7;
        indices[0][3] = (index0 >> 9) & 0x7;
        indices[1][0] = (index0 >> 12) & 0x7;

        indices[1][1] = (index0 >> 15) & 0x7;
        indices[1][1] |= (index1 << 1) & 0x7;
        indices[1][2] = (index1 >> 2) & 0x7;
        indices[1][3] = (index1 >> 5) & 0x7;

        indices[2][0] = (index1 >> 8) & 0x7;
        indices[2][1] = (index1 >> 11) & 0x7;
        indices[2][2] = (index1 >> 14) & 0x7;
        indices[2][2] |= (index2 << 2) & 0x7;
        indices[2][3] = (index2 >> 1) & 0x7;

        indices[3][0] = (index2 >> 4) & 0x7;
        indices[3][1] = (index2 >> 7) & 0x7;
        indices[3][2] = (index2 >> 10) & 0x7;
        indices[3][3] = (index2 >> 13) & 0x7;
    }

    core::error_e dds_convert_bc1(image_codec_context & icctx, const image_data_t & src, image_data_t & dst)
    {
        pixel_convert_fun pfn_resampler = icctx.get_sampler ? icctx.get_sampler(src.format.format, dst.format.format) : image_get_samapler(src.format.format, dst.format.format);
        if (!pfn_resampler)
            return error_not_supported;

        int32_t texel_w = src.format.width >> 2;
        int32_t texel_h = src.format.height >> 2;

        uint16_t colors[4];

        int32_t dst_stride = format_bits(dst.format.format) / 8;
        byte_t * dst_line = dst.data;
        byte_t * dst_pixel = nullptr;
        const dds_texel_bc1_t * texel = reinterpret_cast<const dds_texel_bc1_t *>(src.data);
        for (int32_t texel_row = 0, texel_col = 0; texel_row != texel_h; ++texel_row)
        {
            for (texel_col = 0; texel_col != texel_w; ++texel_col, ++texel)
            {
                dxt_make_color(texel->color0, texel->color1, colors);
                uint32_t rgb_indices = texel->indices;
                for (int32_t row = 0, col = 0; row != 4; ++row)
                {
                    dst_pixel = dst_line + texel_col * 4 * dst_stride + dst.pitch * row;
                    for (col = 0; col != 4; ++col)
                    {
                        uint32_t index = rgb_indices & 0x3;
                        pfn_resampler(colors + index, dst_pixel);
                        rgb_indices >>= 2;
                        dst_pixel += dst_stride;
                    }
                }
            }
            dst_line += dst.pitch * 4; // ÿ��4��
        }
        return error_ok;
    }

    core::error_e dds_convert_bc2(image_codec_context & icctx, const image_data_t & src, image_data_t & dst)
    {
        pixel_convert_fun pfn_resampler = icctx.get_sampler ? icctx.get_sampler(src.format.format, dst.format.format) : image_get_samapler(src.format.format, dst.format.format);
        if (!pfn_resampler)
            return error_not_supported;

        int32_t texel_w = src.format.width >> 2;
        int32_t texel_h = src.format.height >> 2;

        int32_t dst_stride = format_bits(dst.format.format) / 8;

        byte_t * dst_line = dst.data;
        byte_t * dst_pixel = nullptr;
        const dds_texel_bc2_t * texel = reinterpret_cast<const dds_texel_bc2_t *>(src.data);

        for (int32_t texel_row = 0, texel_col = 0; texel_row != texel_h; ++texel_row)
        {
            for (texel_col = 0; texel_col != texel_w; ++texel_col, ++texel)
            {
                uint16_t colors[4];
                dxt_make_color(texel->color0, texel->color1, colors);

                uint32_t rgb_indices = texel->indices;
                for (int32_t row = 0, col = 0; row != 4; ++row)
                {
                    dst_pixel = dst_line + dst.pitch * row + texel_col * 4 * dst_stride;
                    uint16_t alphas = texel->alphas[row];
                    for (col = 0; col != 4; ++col)
                    {
                        uint32_t alpha = alphas & 0xF; // 4bits��Alphaͨ��
                        uint32_t color = colors[rgb_indices & 0x3];
                        uint32_t src_color = (alpha << 16) | color;
                        pfn_resampler((const void *)&src_color, dst_pixel);
                        rgb_indices >>= 2;
                        alphas >>= 4;
                        dst_pixel += dst_stride;
                    }
                }
            }
            dst_line += dst.pitch * 4; // ÿ��4��
        }
        return error_ok;
    }


    core::error_e dds_convert_bc3(image_codec_context & icctx, const image_data_t & src, image_data_t & dst)
    {
        pixel_convert_fun pfn_resampler = icctx.get_sampler ? icctx.get_sampler(src.format.format, dst.format.format) : image_get_samapler(src.format.format, dst.format.format);
        if (!pfn_resampler)
            return error_not_supported;

        int32_t texel_w = src.format.width >> 2;
        int32_t texel_h = src.format.height >> 2;

        int32_t dst_stride = format_bits(dst.format.format) / 8;

        byte_t * dst_line = dst.data;
        byte_t * dst_pixel = nullptr;
        const dds_texel_bc3_t * texel = reinterpret_cast<const dds_texel_bc3_t *>(src.data);

        for (int32_t texel_row = 0, texel_col = 0; texel_row != texel_h; ++texel_row)
        {
            for (texel_col = 0; texel_col != texel_w; ++texel_col, ++texel)
            {
                uint16_t colors[4];
                byte_t alphas[8];
                byte_t alpha_indices[4][4];

                dxt_make_color(texel->color0, texel->color1, colors);
                bc3_calc_alpha(texel->alpha0, texel->alpha1, alphas);
                bc3_calc_alpha_indices(texel->index0, texel->index1, texel->index2, alpha_indices);

                uint32_t rgb_indices = texel->indices;
                for (int32_t row = 0, col = 0; row != 4; ++row)
                {
                    dst_pixel = dst_line + row * dst.pitch + texel_col * 4 * dst_stride;
                    for (col = 0; col != 4; ++col)
                    {
                        uint32_t alpha = alphas[alpha_indices[row][col]];
                        uint32_t color = colors[rgb_indices & 0x3];
                        uint32_t src_color = (alpha << 16) | color;
                        pfn_resampler((const void *)&src_color, dst_pixel);
                        rgb_indices >>= 2;
                        dst_pixel += dst_stride;
                    }
                }
            }
            dst_line += dst.pitch * 4; // ÿ��4��
        }
        return error_ok;
    }

    void color_dds_a4r5g6b5_to_r5g5b5(const void * src_pixel, void * dst_pixel)
    {
        uint16_t * dst = (uint16_t *)dst_pixel;
        uint32_t src = *(uint32_t *)src_pixel;
        *dst = src & 0xFFFF;
    }

    void color_dds_a4r5g6b5_to_r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        color_r8g8b8_t * dst = (color_r8g8b8_t *)dst_pixel;
        uint32_t src = *(uint32_t *)src_pixel;
        dst->r = (src >> 8) & 0xF8;
        dst->g = (src >> 3) & 0xFC;
        dst->b = (src << 3) & 0xF8;
    }

    void color_dds_a4r5g6b5_to_a8r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        uint32_t src = *(uint32_t *)src_pixel;
        color_a8r8g8b8_t * dst = (color_a8r8g8b8_t *)dst_pixel;
        dst->a = ((src & (0x0F << 16)) >> 16) * 0xFF / 0x0F;
        dst->r = ((src & (0x1F << 11)) >> 11) * 0xFF / 0x1F;
        dst->g = ((src & (0x3F << 5)) >> 5) * 0xFF / 0x3F;
        dst->b = ((src & (0x1F << 0)) >> 0) * 0xFF / 0x1F;
    }

    void color_dds_a8r5g6b5_to_a8r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        uint32_t src = *(uint32_t *)src_pixel;
        color_a8r8g8b8_t * dst = (color_a8r8g8b8_t *)dst_pixel;
        dst->a = ((src & (0xFF << 16)) >> 16) * 0xFF / 0xFF;
        dst->r = ((src & (0x1F << 11)) >> 11) * 0xFF / 0x1F;
        dst->g = ((src & (0x3F << 5)) >> 5) * 0xFF / 0x3F;
        dst->b = ((src & (0x1F << 0)) >> 0) * 0xFF / 0x1F;
    }

    core::error_e dds_create(image_codec_context & ictx, const byte_t * buffer, int32_t length, image_data_t & image)
    {
        const dds_header_t * header = reinterpret_cast<const dds_header_t *>(buffer);
        if (header->magic != DDS_MAGIC)
            return error_bad_data;
        if (header->size != sizeof(dds_header_t) - sizeof(DDS_MAGIC))
            return error_bad_data;

        image_convert_fun pfn_convert = nullptr;
        image_format format = {};
        format.width = header->width;
        format.height = header->height;

        if (header->pixel_format.flags & DDSPF_ALPHA_ONLY)
        {
            format.format = format_gray8;
        }
        // ���ַ�����
        else if (header->pixel_format.flags & DDSPF_FOURCC)
        {
            // dx10 ����
            if (header->pixel_format.dxt_format == dxt_format_dx10)
            {
                pfn_convert = nullptr;
            }
            else
            {
                switch (header->pixel_format.dxt_format)
                {
                case dxt_format_dxt1:
                    format.format = format_r5g6b5;
                    pfn_convert = dds_convert_bc1;
                    break;
                case dxt_format_dxt2:
                case dxt_format_dxt3:
                    format.format = format_a4r5g6b5;
                    pfn_convert = dds_convert_bc2;
                    break;
                case dxt_format_dxt4:
                    format.format = format_a8r5g6b5;
                    pfn_convert = dds_convert_bc3;
                    break;
                case dxt_format_ati1:
                    break;
                case dxt_format_ati2:
                    break;
                case dxt_format_a16b16g16r16:
                    format.format = format_a16b16g16r16;
                    pfn_convert = image_convert_ex;
                    break;
                case dxt_format_r16f:
                    format.format = format_r16f;
                    pfn_convert = image_convert_ex;
                    break;
                case dxt_format_g16r16f:
                    format.format = format_g16r16f;
                    pfn_convert = image_convert_ex;
                    break;
                case dxt_format_a16b16g16r16f:
                    format.format = format_a16b16g16r16f;
                    pfn_convert = image_convert_ex;
                    break;
                case dxt_format_r32f:
                    format.format = format_r32f;
                    pfn_convert = image_convert_ex;
                    break;
                case dxt_format_g32r32f:
                    format.format = format_g32r32f;
                    pfn_convert = image_convert_ex;
                    break;
                case dxt_format_a32b32g32r32f:
                    format.format = format_a32b32g32r32f;
                    pfn_convert = image_convert_ex;
                    break;
                default:
                    pfn_convert = nullptr;
                    break;
                }
            }
        }
        // RGB ֱ�ӱ���
        else if (header->pixel_format.flags & DDSPF_RGB)
        {
            format.format = format_from_mask_abgr(header->pixel_format.color_mask, header->pixel_format.bit_count);
            pfn_convert = image_convert_ex;
        }
        else {}

        if (!pfn_convert)
            return error_not_supported;

        image.format = format;
        if (ictx.get_format)
            image.format = ictx.get_format(image_type_bmp, format);

        ictx.pfn_alloc(image, 4);
        image.pfn_free = ictx.pfn_free;

        image_data_t src_data = {};
        src_data.format = format;
        src_data.data = (byte_t *)buffer + sizeof(dds_header_t);
        src_data.pitch = align_to<int32_t>(format.width * (format_bits(format.format) / 8), 4);

        error_e err = pfn_convert(ictx, src_data, image);
        if (err < 0)
        {
            image.pfn_free(image);
            image = {};
            return err;
        }
        return error_ok;
    }
}
