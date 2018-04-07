#include "stdafx.h"
#include "dds.h"

namespace graphics { namespace image { namespace formats
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

    void dds_convert_bc1(int32_t width, int32_t height,
                         pixel_convert_fun_t conv_fun,
                         const byte_t * /*pal*/, int32_t /*pal_stride*/,
                         const byte_t * src, int32_t /*src_stride*/, int32_t /*src_pitch*/,
                         byte_t * dst, int32_t dst_stride, int32_t dst_pitch,
                         int32_t flags)
    {
        int32_t texel_w = width >> 2;
        int32_t texel_h = height >> 2;

        uint16_t colors[4];

        byte_t * dst_line = dst;
        byte_t * dst_pixel = nullptr;
        const dds_texel_bc1_t * texel = (const dds_texel_bc1_t *)src;
        for (int32_t texel_row = 0, texel_col = 0; texel_row != texel_h; ++texel_row)
        {
            for (texel_col = 0; texel_col != texel_w; ++texel_col, ++texel)
            {
                dxt_make_color(texel->color0, texel->color1, colors);
                uint32_t rgb_indices = texel->indices;
                for (int32_t row = 0, col = 0; row != 4; ++row)
                {
                    dst_pixel = dst_line + texel_col * 4 * dst_stride + dst_pitch * row;
                    for (col = 0; col != 4; ++col)
                    {
                        uint32_t index = rgb_indices & 0x3;
                        conv_fun(colors + index, dst_pixel);
                        rgb_indices >>= 2;
                        dst_pixel += dst_stride;
                    }
                }
            }
            dst_line += dst_pitch * 4; // 每次4行
        }
    }

    void dds_convert_bc2(int32_t width, int32_t height,
                         pixel_convert_fun_t conv_fun,
                         const byte_t * /*pal*/, int32_t /*pal_stride*/,
                         const byte_t * src, int32_t src_strike, int32_t src_pitch,
                         byte_t * dst, int32_t dst_strike, int32_t dst_pitch,
                         int32_t flags)
    {
        int32_t texel_w = width >> 2;
        int32_t texel_h = height >> 2;

        uint16_t colors[4] = {};
        uint16_t alphas = 0;
        uint32_t rgb_indices = 0;

        byte_t * dst_line = dst;
        byte_t * dst_pixel = nullptr;
        const dds_texel_bc2_t * texel = (const dds_texel_bc2_t *)src;
        for (int32_t texel_row = 0, texel_col = 0; texel_row != texel_h; ++texel_row)
        {
            for (texel_col = 0; texel_col != texel_w; ++texel_col, ++texel)
            {
                dxt_make_color(texel->color0, texel->color1, colors);

                rgb_indices = texel->indices;
                for (int32_t row = 0, col = 0; row != 4; ++row)
                {
                    dst_pixel = dst_line + dst_pitch * row + texel_col * 4 * dst_strike;
                    alphas = texel->alphas[row];
                    for (col = 0; col != 4; ++col)
                    {
                        uint32_t alpha = alphas & 0xF; // 4bits的Alpha通道
                        uint32_t color = colors[rgb_indices & 0x3];
                        uint32_t src_color = (alpha << 16) | color;
                        conv_fun((const void *)&src_color, dst_pixel);
                        rgb_indices >>= 2;
                        alphas >>= 4;
                        dst_pixel += dst_strike;
                    }
                }
            }
            dst_line += dst_pitch * 4; // 每次4行
        }
    }


    void dds_convert_bc3(int32_t width, int32_t height,
                         pixel_convert_fun_t conv_fun,
                         const byte_t * /*pal*/, int32_t /*pal_stride*/,
                         const byte_t * src, int32_t src_strike, int32_t src_pitch,
                         byte_t * dst, int32_t dst_strike, int32_t dst_pitch,
                         int32_t flags)
    {
        int32_t texel_w = width >> 2;
        int32_t texel_h = height >> 2;

        uint16_t colors[4];
        byte_t alphas[8];
        byte_t alpha_indices[4][4];
        uint32_t rgb_indices;

        byte_t * dst_line = dst;
        byte_t * dst_pixel = nullptr;
        const dds_texel_bc3_t * texel = (const dds_texel_bc3_t *)src;
        for (int32_t texel_row = 0, texel_col = 0; texel_row != texel_h; ++texel_row)
        {
            for (texel_col = 0; texel_col != texel_w; ++texel_col, ++texel)
            {
                dxt_make_color(texel->color0, texel->color1, colors);
                bc3_calc_alpha(texel->alpha0, texel->alpha1, alphas);
                bc3_calc_alpha_indices(texel->index0, texel->index1, texel->index2, alpha_indices);

                rgb_indices = texel->indices;
                for (int32_t row = 0, col = 0; row != 4; ++row)
                {
                    dst_pixel = dst_line + dst_pitch * row + texel_col * 4 * dst_strike;
                    for (col = 0; col != 4; ++col)
                    {
                        uint32_t alpha = alphas[alpha_indices[row][col]];
                        uint32_t color = colors[rgb_indices & 0x3];
                        uint32_t src_color = (alpha << 16) | color;
                        conv_fun((const void *)&src_color, dst_pixel);
                        rgb_indices >>= 2;
                        dst_pixel += dst_strike;
                    }
                }
            }
            dst_line += dst_pitch * 4; // 每次4行
        }
    }

    void dds_convert_copy_dxt1(int32_t width, int32_t height,
                               pixel_convert_fun_t conv_fun,
                               const byte_t *, int32_t,
                               const byte_t * src, int32_t src_stride, int32_t src_pitch,
                               byte_t * dst, int32_t dst_stride, int32_t dst_pitch,
                               int32_t flags)
    {
        int32_t texel_h = (height + 3) >> 2;
        for (int32_t row = 0; row < texel_h; ++row)
        {
            image_memcpy(dst, src_pitch, src, src_pitch);
            src += src_pitch;
            dst += dst_pitch;
        }
    }

    void dds_convert_copy_dxt23(int32_t width, int32_t height,
                                pixel_convert_fun_t conv_fun,
                                const byte_t *, int32_t,
                                const byte_t * src, int32_t src_strike, int32_t src_pitch,
                                byte_t * dst, int32_t dst_strike, int32_t dst_pitch,
                                int32_t flags)
    {
        int32_t texel_h = (height + 3) >> 2;
        for (int32_t row = 0; row < texel_h; ++row)
        {
            image_memcpy(dst, src_pitch, src, src_pitch);
            src += src_pitch;
            dst += dst_pitch;
        }
    }

    void dds_convert_copy_dxt45(int32_t width, int32_t height,
                                pixel_convert_fun_t conv_fun,
                                const byte_t *, int32_t,
                                const byte_t * src, int32_t src_strike, int32_t src_pitch,
                                byte_t * dst, int32_t dst_strike, int32_t dst_pitch,
                                int32_t flags)
    {
        int32_t texel_h = (height + 3) >> 2;
        for (int32_t row = 0; row < texel_h; ++row)
        {
            image_memcpy(dst, src_pitch, src, src_pitch);
            src += src_pitch;
            dst += dst_pitch;
        }
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

    core::error_e dds_create(const byte_t * buffer, int32_t length, image_data_t * img,
                       image_convert_rule_fun_t pfn_match, void * user_data)
    {
        if (!pfn_match)
            pfn_match = dds_rule_default;

        const dds_header_t * header = (const dds_header_t *)buffer;
        if (header->magic != DDS_MAGIC)
            return error_bad_data;
        if (header->size != sizeof(dds_header_t) - sizeof(DDS_MAGIC))
            return error_bad_data;

        buffer += sizeof(dds_header_t);


        cmode_e color_mode = dds_get_cmode(header->pixel_format);

        image_convert_rule_t rule = {image_format_dds, (int32_t)header->width, (int32_t)header->height, color_mode, user_data};
        if (!pfn_match(&rule))
            return error_bad_format;

        img->width = rule.width;
        img->height = rule.height;
        img->bits = rule.dst_bits;
        img->pitch = rule.dst_pitch;
        img->length = rule.dst_length;
        img->buffer = rule.dst_buffer;
        img->src_mode = rule.src_mode;
        img->dst_mode = rule.dst_mode;
        img->flags = 0;

        img->buffer = image_malloc(img->length);
        //! dds 不使用调色板
        rule.image_convert_fun(rule.width, rule.height,
                               rule.pixel_convert_fun,
                               nullptr, 0,
                               buffer, rule.src_stride, rule.src_pitch,
                               img->buffer, rule.dst_stride, rule.dst_pitch, 0);
        return error_ok;
    }

    cmode_e dds_get_cmode(const dds_pixel_format_t & pixel_format)
    {
        cmode_e color_mode = cmode_none;

        if (pixel_format.flags & DDSPF_ALPHA_ONLY)
        {
            color_mode = cmode_gray8;
        }
            // 四字符编码
        else if (pixel_format.flags & DDSPF_FOURCC)
        {
            // dx10 编码
            if (pixel_format.dxt_format == dxt_format_dx10)
                color_mode = cmode_dx10;
            else
            {
                switch (pixel_format.dxt_format)
                {
                case dxt_format_dxt1:
                    color_mode = cmode_bc1;
                    break;
                case dxt_format_dxt2:
                    color_mode = cmode_bc2;
                    break;
                case dxt_format_dxt3:
                    color_mode = cmode_bc2;
                    break;
                case dxt_format_dxt4:
                    color_mode = cmode_bc3;
                    break;
                case dxt_format_dxt5:
                    color_mode = cmode_bc3;
                    break;
                case dxt_format_ati1:
                    break;
                case dxt_format_ati2:
                    break;
                case dxt_format_a16b16g16r16:
                    color_mode = cmode_a16b16g16r16;
                    break;
                case dxt_format_r16f:
                    color_mode = cmode_r16f;
                    break;
                case dxt_format_g16r16f:
                    color_mode = cmode_g16r16f;
                    break;
                case dxt_format_a16b16g16r16f:
                    color_mode = cmode_a16b16g16r16f;
                    break;
                case dxt_format_r32f:
                    color_mode = cmode_r32f;
                    break;
                case dxt_format_g32r32f:
                    color_mode = cmode_g32r32f;
                    break;
                case dxt_format_a32b32g32r32f:
                    color_mode = cmode_a32b32g32r32f;
                    break;
                default:
                    break;
                }
            }
        }
            // RGB 直接编码
        else if (pixel_format.flags & DDSPF_RGB)
        {
            color_mode = cmode_from_mask_abgr(pixel_format.color_mask, pixel_format.bit_count);
        }
        else
        {
        }
        return color_mode;
    }

    cmode_e dds_get_cmode(dxgi_format_e format)
    {
        switch (format)
        {
        case dxgi_format_r8g8_typeless:
        case dxgi_format_r8g8_unorm:
        case dxgi_format_r8g8_uint:
        case dxgi_format_r8g8_snorm:
        case dxgi_format_r8g8_sint:
            return cmode_r8g8;

        case dxgi_format_r8g8b8a8_typeless:
        case dxgi_format_r8g8b8a8_unorm:
        case dxgi_format_r8g8b8a8_unorm_srgb:
        case dxgi_format_r8g8b8a8_uint:
        case dxgi_format_r8g8b8a8_snorm:
        case dxgi_format_r8g8b8a8_sint:
            return cmode_a8b8g8r8;

        default:
            return cmode_none;
        }
    }

    void dds_get_pitch(cmode_e cmode, int32_t width, int32_t height, int32_t * row, int32_t * col, int32_t * pitch)
    {
        int32_t _row = height;
        int32_t _col = width;
        int32_t _pitch = 0;
        int32_t bits = 0;
        switch (cmode)
        {
        case cmode_gray8:
            bits = 8;
            break;
        case cmode_r8g8:
            bits = 16;
            break;
        case cmode_a8r8g8b8:
            bits = 32;
            break;
        case cmode_bc1:
            bits = sizeof(dds_texel_bc1_t);
            _col = (_col + 3) / 4;
            _col = (_row + 3) / 4;
            break;
        case cmode_bc2:
            bits = sizeof(dds_texel_bc2_t);
            _col = (_col + 3) / 4;
            _col = (_row + 3) / 4;
            break;
        case cmode_bc3:
            bits = sizeof(dds_texel_bc3_t);
            _col = (_col + 3) / 4;
            _col = (_row + 3) / 4;
            break;
        default:
            bits = cmode_bits(cmode);
            break;
        }

        _pitch = align_to_4(_col * bits / 8);
        if (row) *row = _row;
        if (col) *col = _col;
        if (pitch) *pitch = _pitch;
    }

    bool dds_rule_default(image_convert_rule_t * rule)
    {
        return false;
    }
}}}
