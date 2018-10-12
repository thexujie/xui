#include "stdafx.h"
#include "image.h"

#include "formats/bmp.h"
#include "formats/tga.h"
#include "formats/png.h"
#include "formats/jpg.h"
#include "formats/dds.h"

//#include "converter/image_converter_yuv.h"
//#include "converter/image_converter_ycck.h"
//#include "converter/image_converter_gray.h"
//#include "converter/image_converter_cmyk.h"
//#include "converter/image_converter_rgb.h"

namespace drawing::image
{
    using namespace core;
    using namespace drawing::image::formats;

    error image_load(std::string path, image_data_t & img)
    {
        std::wstring pathw = core::u8str_wstr(path);
        std::fstream fs;
        fs.open(pathw.c_str(), std::ios::in | std::ios::binary);
        if (!fs.good())
            return error_not_found;

        fs.seekg(0, std::ios::end);
        auto length = fs.tellg();
        fs.seekg(0, std::ios::beg);

        std::shared_ptr<byte_t[]> buffer(image_malloc((int32_t)length));
        fs.read(buffer.get(), length);
        if (fs.tellg() != length)
            return error_io;
        fs.close();

        return image_create(buffer.get(), (int32_t)length, img);
    }

    image_type image_get_type_from_ext(const char * ext, size_t length)
    {
        if (!ext || !ext[0])
            return image_type_none;

        if (length == core::npos)
            length = core::textlen(ext, 16);

        if (ext[0] == '.')
        {
            ++ext;
            --length;
        }

        if(length <= 0)
            return image_type_none;

        if (core::equal_ic(ext, length, "bmp", 3))
            return image_type_bmp;
        if (core::equal_ic(ext, length, "png", 3))
            return image_type_png;
        if (core::equal_ic(ext, length, "jpg", 3))
            return image_type_jpeg;
        if (core::equal_ic(ext, length, "tga", 3))
            return image_type_tga;
        if (core::equal_ic(ext, length, "dds", 3))
            return image_type_dds;

        return image_type_none;
    }

    image_type image_get_type(const byte_t * buffer, size_t length)
    {
        if (is_bmp_data(buffer, length))
            return image_type_bmp;
        if (is_png_data(buffer, length))
            return image_type_png;
        if (is_jpg_data(buffer, length))
            return image_type_jpeg;
        if (is_tga_data(buffer, length))
            return image_type_tga;
        if (is_dds_data(buffer, length))
            return image_type_dds;
        return image_type_none;
    }

    core::error image_create(const byte_t * buffer, int32_t length, image_data_t & img)
    {
        image_codec_context ictx;
        ictx.get_format = image_get_format;
        ictx.get_sampler = image_get_samapler;
        ictx.pfn_alloc = image_buffer_alloc_default;
        ictx.pfn_free = image_buffer_free;
        return image_create(ictx, buffer, length, img);
    }

    core::error image_create(image_codec_context & ictx, const byte_t * buffer, int32_t length, image_data_t & img)
    {
        if(ictx.type == image_type_none)
            ictx.type = image_get_type(buffer, length);

        switch (ictx.type)
        {
        case image_type_bmp:
            return bmp_create(ictx, buffer, length, img);
        case image_type_tga:
            return tga_create(ictx, buffer, length, img);
        case image_type_png:
            return png_create(ictx, buffer, length, img);
        case image_type_jpeg:
            return jpg_create(ictx, buffer, length, img);
        case image_type_dds:
            return dds_create(ictx, buffer, length, img);
        default:
            return error_bad_format;
        }
    }

    core::error  image_convert_ex(image_codec_context & icctx, const image_data_t & src, image_data_t & dst)
    {
        if (src.format == dst.format && src.pitch == dst.pitch)
        {
            image_memcpy(dst.data, dst.format.height * std::abs(dst.pitch), src.data, src.format.height * std::abs(src.pitch));
            return error_ok;
        }

        pixel_convert_fun pfn_resampler = icctx.get_sampler ? icctx.get_sampler(src.format.format, dst.format.format) : image_get_samapler(src.format.format, dst.format.format);
        assert(pfn_resampler);
        if (!pfn_resampler)
            return error_not_supported;
        
        int32_t src_stride = format_bits(src.format.format) / 8;
        int32_t dst_stride = format_bits(dst.format.format) / 8;

        const byte_t * src_line = src.data;
        byte_t * dst_line = dst.data;

        if (src.pitch < 0)
            src_line = src.data + (src.format.height - 1) * -src.pitch;

        for (int32_t row = 0, col = 0; row != src.format.height; ++row)
        {
            const byte_t * src_pixel = src_line;
            byte_t * dst_pixel = dst_line;
            for (col = 0; col != src.format.width; ++col)
            {
                pfn_resampler(src_pixel, dst_pixel);
                src_pixel += src_stride;
                dst_pixel += dst_stride;
            }
            src_line += src.pitch;
            dst_line += dst.pitch;
        }
        return error_ok;
    }

    core::error image_convert_copy_ex(image_codec_context & icctx, const image_data_t & src, image_data_t & dst)
    {
        if (src.pitch != dst.pitch)
            return error_not_supported;

        if (src.format == dst.format)
        {
            image_memcpy(dst.data, dst.format.height * std::abs(dst.pitch), src.data, src.format.height * std::abs(src.pitch));
            return error_ok;
        }

        const byte_t * src_line = src.data;
        byte_t * dst_line = dst.data;

        if (src.pitch < 0)
            src_line = src.data + (src.format.height - 1) * src.pitch;

        for (int32_t row = 0, col = 0; row != src.format.height; ++row)
        {
            image_memcpy(dst_line, std::abs(dst.pitch), src_line, std::abs(src.pitch));
            src_line += src.pitch;
            dst_line += dst.pitch;
        }
        return error_ok;
    }

    core::error image_convert_index1_ex(image_codec_context & icctx, const image_data_t & src, image_data_t & dst)
    {
        pixel_convert_fun pfn_resampler = icctx.get_sampler ? icctx.get_sampler(src.format.format, dst.format.format) : image_get_samapler(src.format.format, dst.format.format);
        if (!pfn_resampler)
            return error_not_supported;

        const byte_t * src_pixel = src.data;
        byte_t * dst_line = dst.data;

        int32_t dst_pitch = dst.pitch;
        if (src.pitch < 0)
        {
            dst_line = dst.data + (dst.format.height - 1) * dst.pitch;
            dst_pitch = -dst_pitch;
        }

        int32_t src_stride = format_bits(src.format.format) / 8;
        int32_t dst_stride = format_bits(dst.format.format) / 8;

        for (int32_t row = 0; row != src.format.height; ++row)
        {
            byte_t * dst_pixel = dst_line;
            for (int32_t col = 0; col != src.format.width; ++col)
            {
#pragma pack(push, 1)
                union
                {
                    struct
                    {
                        uint8_t index7 : 1;
                        uint8_t index6 : 1;
                        uint8_t index5 : 1;
                        uint8_t index4 : 1;
                        uint8_t index3 : 1;
                        uint8_t index2 : 1;
                        uint8_t index1 : 1;
                        uint8_t index0 : 1;
                    };

                    uint8_t index;
                };

#pragma pack(pop)
                const uint32_t colors[2] = {0, 0xffffffff};
                const_cast<byte_t *&>(src.palette) = (byte_t *)colors;
                switch (col & 0x7)
                {
                case 0:
                    index = *src_pixel;
                    pfn_resampler(src.palette + index0 * src_stride, dst_pixel);
                    break;
                case 1:
                    pfn_resampler(src.palette + index1 * src_stride, dst_pixel);
                    break;
                case 2:
                    pfn_resampler(src.palette + index2 * src_stride, dst_pixel);
                    break;
                case 3:
                    pfn_resampler(src.palette + index3 * src_stride, dst_pixel);
                    break;
                case 4:
                    pfn_resampler(src.palette + index4 * src_stride, dst_pixel);
                    break;
                case 5:
                    pfn_resampler(src.palette + index5 * src_stride, dst_pixel);
                    break;
                case 6:
                    pfn_resampler(src.palette + index6 * src_stride, dst_pixel);
                    break;
                case 7:
                    pfn_resampler(src.palette + index7 * src_stride, dst_pixel);
                    src_pixel += 1;
                    break;
                default:
                    break;
                }
                dst_pixel += dst_stride;
            }
            dst_line += dst_pitch;
        }

        return error_ok;
    }

    core::error image_convert_index2_ex(image_codec_context & icctx, const image_data_t & src, image_data_t & dst)
    {
        pixel_convert_fun pfn_resampler = icctx.get_sampler ? icctx.get_sampler(src.format.format, dst.format.format) : image_get_samapler(src.format.format, dst.format.format);
        if (!pfn_resampler)
            return error_not_supported;

        const byte_t * src_pixel = src.data;
        byte_t * dst_line = dst.data;
        byte_t * dst_pixel = nullptr;

        int32_t dst_pitch = dst.pitch;
        if (src.pitch < 0)
        {
            dst_line = dst.data + (dst.format.height - 1) * dst.pitch;
            dst_pitch = -dst_pitch;
        }

        int32_t src_stride = format_bits(src.format.format) / 8;
        int32_t dst_stride = format_bits(dst.format.format) / 8;

        for (int32_t row = 0, col = 0; row != src.format.height; ++row)
        {
            dst_pixel = dst_line;
            for (col = 0; col != src.format.width; ++col)
            {
#pragma pack(push, 1)
                union
                {
                    struct
                    {
                        uint8_t index3 : 2;
                        uint8_t index2 : 2;
                        uint8_t index1 : 2;
                        uint8_t idnex0 : 2;
                    };

                    byte_t index;
                };
#pragma pack(pop)
                switch (col & 0x3)
                {
                case 0:
                    index = *src_pixel;
                    pfn_resampler(src.palette + idnex0 * src_stride, dst_pixel);
                    break;
                case 1:
                    pfn_resampler(src.palette + index1 * src_stride, dst_pixel);
                    break;
                case 2:
                    pfn_resampler(src.palette + index2 * src_stride, dst_pixel);
                    break;
                case 3:
                    pfn_resampler(src.palette + index3 * src_stride, dst_pixel);
                    src_pixel += 1;
                    break;
                default:
                    break;
                }
                dst_pixel += dst_stride;
            }
            dst_line += dst_pitch;
        }
        return error_ok;
    }

    core::error image_convert_index4_ex(image_codec_context & icctx, const image_data_t & src, image_data_t & dst)
    {
        pixel_convert_fun pfn_resampler = icctx.get_sampler ? icctx.get_sampler(src.format.format, dst.format.format) : image_get_samapler(src.format.format, dst.format.format);
        if (!pfn_resampler)
            return error_not_supported;

        const byte_t * src_pixel = src.data;
        byte_t * dst_line = dst.data;
        byte_t * dst_pixel = nullptr;

        int32_t dst_pitch = dst.pitch;
        if (src.pitch < 0)
        {
            dst_line = dst.data + (dst.format.height - 1) * dst.pitch;
            dst_pitch = -dst.pitch;
        }

        int32_t src_stride = format_bits(src.format.format) / 8;
        int32_t dst_stride = format_bits(dst.format.format) / 8;

        for (int32_t row = 0; row != src.format.height; ++row)
        {
            dst_pixel = dst_line;
            for (int32_t col = 0; col != src.format.width; ++col)
            {
#pragma pack(push, 1)
                union
                {
                    struct
                    {
                        uint8_t index0 : 4;
                        uint8_t index1 : 4;
                    };

                    byte_t index;
                };
#pragma pack(pop)
                if (col & 0x1)
                {
                    pfn_resampler(src.palette + index0 * src_stride, dst_pixel);
                    src_pixel += 1;
                }
                else
                {
                    index = *src_pixel;
                    pfn_resampler(src.palette + index1 * src_stride, dst_pixel);
                }
                dst_pixel += dst_stride;
            }
            dst_line += dst_pitch;
        }
        return error_ok;
    }

    core::error image_convert_index8_ex(image_codec_context & icctx, const image_data_t & src, image_data_t & dst)
    {
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

        const uint8_t * src_pixel = (const uint8_t *)src.data;
        byte_t * dst_pixel = nullptr;
        int32_t src_stride = format_bits(src.format.format) / 8;
        int32_t dst_stride = format_bits(dst.format.format) / 8;

        for (int32_t row = 0, col = 0; row != src.format.height; ++row)
        {
            dst_pixel = dst_line;
            for (col = 0; col != src.format.width; ++col)
            {
                pfn_resampler(src.palette + *src_pixel * src_stride, dst_pixel);
                src_pixel += 1;
                dst_pixel += dst_stride;
            }
            dst_line += dst_pitch;
        }
        return error_ok;
    }


    int32_t format_bits(format cmode)
    {
        switch (cmode)
        {
        case format_gray1: return 1;
        case format_gray2: return 2;
        case format_gray4: return 4;
        case format_gray8: return 8;

        case format_r3g3b2:
            return 8;

        case format_r8g8b8:
        case format_b8g8r8:
            return 24;

        case format_r5g6b5:
        case format_a1r5g5b5:
        case format_a1b5g5r5:
        case format_x1r5g5b5:
        case format_x1b5g5r5:
        case format_a4r4g4b4:
        case format_x4r4g4b4:
        case format_a8r3g3b2:
            return 16;

        case format_a8r8g8b8:
        case format_x8r8g8b8:
        case format_a8b8g8r8:
        case format_x8b8g8r8:
        case format_r8g8b8a8:
        case format_r8g8b8x8:
        case format_b8g8r8a8:
        case format_b8g8r8x8:
        case format_g16r16:
        case format_a2r10g10b10:
        case format_a2b10g10r10:
            return 32;

        case format_a16b16g16r16:
            return 64;

        case format_r16f: return 16;
        case format_g16r16f: return 32;
        case format_a16b16g16r16f: return 64;
        case format_r32f: return 32;
        case format_g32r32f: return 64;
        case format_b32g32r32f: return 96;
        case format_a32b32g32r32f: return 128;

        case format_x32y32z32f: return 96;

        default:
            return 0;
        }
    }

#define COLOR_MODE_TEXT(mode) case mode: return (#mode)

    const char * format_text(format cmode)
    {
        switch (cmode)
        {
        COLOR_MODE_TEXT(format_none);
        COLOR_MODE_TEXT(format_gray2);
        COLOR_MODE_TEXT(format_gray4);
        COLOR_MODE_TEXT(format_gray8);

        COLOR_MODE_TEXT(format_r8g8b8);
        COLOR_MODE_TEXT(format_a8r8g8b8);
        COLOR_MODE_TEXT(format_x8r8g8b8);

        COLOR_MODE_TEXT(format_b8g8r8);
        COLOR_MODE_TEXT(format_a8b8g8r8);
        COLOR_MODE_TEXT(format_x8b8g8r8);

        COLOR_MODE_TEXT(format_yuv);
        COLOR_MODE_TEXT(format_cmyk);
        COLOR_MODE_TEXT(format_ycck);

        COLOR_MODE_TEXT(format_r5g6b5);
        COLOR_MODE_TEXT(format_a8r5g6b5);
        COLOR_MODE_TEXT(format_a1r5g5b5);
        COLOR_MODE_TEXT(format_a1b5g5r5);
        COLOR_MODE_TEXT(format_x1r5g5b5);
        COLOR_MODE_TEXT(format_a4r4g4b4);
        COLOR_MODE_TEXT(format_x4r4g4b4);
        COLOR_MODE_TEXT(format_g16r16);
        COLOR_MODE_TEXT(format_a16b16g16r16);
        COLOR_MODE_TEXT(format_r3g3b2);

        COLOR_MODE_TEXT(format_r8g8b8a8);
        COLOR_MODE_TEXT(format_r8g8b8x8);

        COLOR_MODE_TEXT(format_r16f);
        COLOR_MODE_TEXT(format_g16r16f);
        COLOR_MODE_TEXT(format_a16b16g16r16f);
        COLOR_MODE_TEXT(format_r32f);
        COLOR_MODE_TEXT(format_g32r32f);
        COLOR_MODE_TEXT(format_a32b32g32r32f);

        COLOR_MODE_TEXT(format_x32y32z32f);

        default:
            return _T("color_mode unkonwn");
        }
    }

    int32_t align_to_4(int32_t val)
    {
        return val + 3 & ~3;
    }

    uint8_t color_float16_to_8(uint16_t f16)
    {
#pragma pack(push, 4)
        union
        {
            int32_t ival;
            float32_t fval;
        };
#pragma pack(pop)

        int32_t signific = (f16 >> 15) & 0x1;
        int32_t exponent = (f16 >> 10) & 0x1F;
        int32_t mantissa = (f16 & 0x3FF);

        if (mantissa == 0)
            return 0xFF;
        if (exponent == 0)
        {
            while (!(mantissa & 0x00000400))
            {
                mantissa <<= 1;
                exponent -= 1;
            }

            exponent += 1;
            mantissa &= ~0x00000400;
        }
        else if (exponent == 31)
        {
            ival = (signific << 31) | 0x7f800000 | (mantissa << 13);
        }
        else
        {
            exponent += 0x70;
            ival = (signific << 31) | (exponent << 23) | (mantissa << 13);
        }
        //if(fval < 0.0f)
        //	return 0;
        //else if(fval > 1.0f)
        //	return 0xFF;
        //else
        return (uint8_t)(fval * 0xFF);
    }


    void image_buffer_alloc_default(image_data_t & data, int32_t align)
    {
        data.pitch = align_to<int32_t>(data.format.width * (format_bits(data.format.format) / 8), align);
        data.data = (byte_t *)malloc(data.format.height * data.pitch);
    }

    void image_buffer_free(image_data_t & data)
    {
        free(data.data);
        data = {};
    }

    static bool mask_equal(const color_mask_abgr_t & ma, const color_mask_abgr_t & mb)
    {
        return
            ma.r == mb.r &&
            ma.g == mb.g &&
            ma.b == mb.b &&
            ma.a == mb.a;
    }

    format format_from_mask_abgr(const color_mask_abgr_t & mask, int32_t bits)
    {
        switch (bits)
        {
        case 8:
            if (mask_equal(mask, MASK_R3G3B2))
                return format_r3g3b2;
            break;
        case 16:
            if (mask_equal(mask, MASK_A8R3G3B2))
                return format_a8r3g3b2;
            if (mask_equal(mask, MASK_A4R4G4B4))
                return format_a4r4g4b4;
            if (mask_equal(mask, MASK_X4R4G4B4))
                return format_x4r4g4b4;
            if (mask_equal(mask, MASK_A0R5G6B5))
                return format_r5g6b5;
            if (mask_equal(mask, MASK_X1R5G5B5))
                return format_x1r5g5b5;
            if (mask_equal(mask, MASK_A1R5G5B5))
                return format_a1r5g5b5;
            break;
        case 24:
            if (mask_equal(mask, MASK_R8G8B8))
                return format_r8g8b8;
            if (mask_equal(mask, MASK_B8G8R8))
                return format_b8g8r8;
            break;
        case 32:
            if (mask_equal(mask, MASK_X8R8G8B8))
                return format_x8r8g8b8;
            if (mask_equal(mask, MASK_A8R8G8B8))
                return format_a8r8g8b8;
            if (mask_equal(mask, MASK_R8G8B8X8))
                return format_r8g8b8x8;
            if (mask_equal(mask, MASK_R8G8B8A8))
                return format_r8g8b8a8;
            if (mask_equal(mask, MASK_X8B8G8R8))
                return format_x8b8g8r8;
            if (mask_equal(mask, MASK_A8B8G8R8))
                return format_a8b8g8r8;

            // dds file format format_a2b10g10r10 is error, it should be format_a2r10g10b10.
            if (mask_equal(mask, MASK_A2R10G10B10))
                return format_a2b10g10r10;
            if (mask_equal(mask, MASK_A2B10G10R10))
                return format_a2r10g10b10;


            if (mask_equal(mask, MASK_G16R16))
                return format_g16r16;
            break;
        }
        return format_none;
    }

    color_mask_abgr_t mask_from_format_abgr(format format)
    {
        switch(format)
        {
        case format_r3g3b2: return MASK_R3G3B2;
        case format_a8r3g3b2: return MASK_A8R3G3B2;
        case format_a4r4g4b4: return MASK_A4R4G4B4;
        case format_x4r4g4b4: return MASK_X4R4G4B4;
        case format_r5g6b5: return MASK_A0R5G6B5;
        case format_x1r5g5b5: return MASK_X1R5G5B5;
        case format_a1r5g5b5: return MASK_A1R5G5B5;
        case format_r8g8b8: return MASK_R8G8B8;
        case format_b8g8r8: return MASK_B8G8R8;
        case format_x8r8g8b8: return MASK_X8R8G8B8;
        case format_a8r8g8b8: return MASK_A8R8G8B8;
        case format_x8b8g8r8: return MASK_X8B8G8R8;
        case format_a8b8g8r8: return MASK_A8B8G8R8;
        case format_r8g8b8x8: return MASK_R8G8B8X8;
        case format_r8g8b8a8: return MASK_R8G8B8A8;
        case format_g16r16: return MASK_G16R16;
        case format_a2r10g10b10: return MASK_A2R10G10B10;
        case format_a2b10g10r10: return MASK_A2B10G10R10;
        default: return {};
        }
    }

    pixel_convert_fun image_get_samapler(format src, format dst)
    {
        static pixel_convert_fun pfns[format_count][format_count] = {};
        if(!pfns[format_gray8][format_gray8])
        {
            pfns[format_gray8][format_gray8] = color_8_to_8;
            pfns[format_gray8][format_r3g3b2] = color_8_to_8;
            pfns[format_gray8][format_r8g8b8] = color_gray8_to_r8g8b8;
            pfns[format_gray8][format_x8r8g8b8] = color_gray8_to_x8r8g8b8;

            pfns[format_r3g3b2][format_r3g3b2] = color_8_to_8;
            pfns[format_r3g3b2][format_r8g8b8] = color_r3g3b2_to_r8g8b8;

            pfns[format_r5g6b5][format_r5g6b5] = color_16_to_16;
            pfns[format_b5g6r5][format_b5g6r5] = color_16_to_16;
            pfns[format_r8g8b8][format_r8g8b8] = color_24_to_24;
            pfns[format_r8g8b8][format_x8r8g8b8] = color_r8g8b8_to_x8r8g8b8;
            pfns[format_b8g8r8][format_b8g8r8] = color_24_to_24;
            pfns[format_b8g8r8][format_r8g8b8] = color_r8g8b8_to_b8g8r8;
            pfns[format_x1r5g5b5][format_x1r5g5b5] = color_16_to_16;
            pfns[format_x1r5g5b5][format_r8g8b8] = color_x1r5g5b5_to_r8g8b8;
            pfns[format_a4r4g4b4][format_a4r4g4b4] = color_16_to_16;
            pfns[format_x4r4g4b4][format_x4r4g4b4] = color_16_to_16;

            pfns[format_a8r8g8b8][format_a8r8g8b8] = color_32_to_32;
            pfns[format_a8b8g8r8][format_a8b8g8r8] = color_32_to_32;
            pfns[format_a8b8g8r8][format_a8r8g8b8] = color_a8b8g8r8_to_a8r8g8b8;
            pfns[format_x8r8g8b8][format_x8r8g8b8] = color_32_to_32;
            pfns[format_x8b8g8r8][format_x8b8g8r8] = color_32_to_32;
            pfns[format_r8g8b8x8][format_r8g8b8x8] = color_32_to_32;
            pfns[format_b8g8r8x8][format_b8g8r8x8] = color_32_to_32;

            pfns[format_a4r5g6b5][format_a8r8g8b8] = color_dds_a4r5g6b5_to_a8r8g8b8;
            pfns[format_a8r5g6b5][format_a8r8g8b8] = color_dds_a8r5g6b5_to_a8r8g8b8;
        }
        return pfns[src][dst];
    }

    image_format image_get_format(image_type type, image_format format)
    {
        image_format result = format;
        switch(format.format)
        {
        case format_b8g8r8: result.format = format_r8g8b8; break;
        case format_a8b8g8r8: result.format = format_a8r8g8b8; break;
        default: break;
        }
        return result;
    }

    //////////////////////////////////////////////////////////////////////////
    /// color_a_to_b

    void color_8_to_8(const void * src_pixel, void * dst_pixel)
    {
        byte_t * src = (byte_t *)src_pixel;
        byte_t * dst = (byte_t *)dst_pixel;
        *dst = *src;
    }

    void color_16_to_16(const void * src_pixel, void * dst_pixel)
    {
        uint16_t * src = (uint16_t *)src_pixel;
        uint16_t * dst = (uint16_t *)dst_pixel;
        *dst = *src;
    }

    void color_24_to_24(const void * src_pixel, void * dst_pixel)
    {
        color_r8g8b8_t * src = (color_r8g8b8_t *)src_pixel;
        color_r8g8b8_t * dst = (color_r8g8b8_t *)dst_pixel;
        *dst = *src;
    }

    void color_32_to_32(const void * src_pixel, void * dst_pixel)
    {
        uint32_t * src = (uint32_t *)src_pixel;
        uint32_t * dst = (uint32_t *)dst_pixel;
        *dst = *src;
    }

    void color_32_to_32_ex0(const void * src_pixel, void * dst_pixel)
    {
        uint32_t * src = (uint32_t *)src_pixel;
        uint32_t * dst = (uint32_t *)dst_pixel;
        if (*src & 0xFF000000)
            *dst = *src;
        else if (*src)
            *dst = *src | 0xFF000000;
        else
            *dst = 0;
    }

    void color_r5g6b5_to_x1r5g5b5(const void * src_pixel, void * dst_pixel)
    {
        uint16_t * src = (uint16_t *)src_pixel;
        uint16_t * dst = (uint16_t *)dst_pixel;
        *dst =
            ((*src & 0xF800) >> 1) |
            (((*src & 0x07E0) >> 1) & 0x03E0) |
            ((*src & 0x001F));
    }

    void color_r5g6b5_to_r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        uint16_t * src = (uint16_t *)src_pixel;
        color_r8g8b8_t * dst = (color_r8g8b8_t *)dst_pixel;

        dst->r = uint8_t((*src & 0xF800) >> 8);
        dst->g = uint8_t((*src & 0x07E0) >> 3);
        dst->b = uint8_t((*src & 0x001F) << 3);
    }

    void color_r5g6b5_to_x8r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        uint16_t * src = (uint16_t *)src_pixel;
        color_a8r8g8b8_t * dst = (color_a8r8g8b8_t *)dst_pixel;

        dst->r = uint8_t((*src & 0xF800) >> 8);
        dst->g = uint8_t((*src & 0x07E0) >> 3);
        dst->b = uint8_t((*src & 0x001F) << 3);
        dst->a = 0xFF;
    }

    void color_r5g6b5_to_x8b8g8r8(const void * src_pixel, void * dst_pixel)
    {
        uint16_t src = *(const uint16_t *)src_pixel;
        uint32_t * dst = (uint32_t *)dst_pixel;

        *dst =
            0xFF000000 |
            ((((src >> 0) & 0x1F) * 0xFF / 0x1F) << 16) |
            ((((src >> 5) & 0x3F) * 0xFF / 0x3F) << 8) |
            ((((src >> 11) & 0x1F) * 0xFF / 0x1F) << 0);
    }

    void color_b5g6r5_to_r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        uint16_t src = *(const uint16_t *)src_pixel;
        uint8_t * dst = (uint8_t *)dst_pixel;

        dst[0] = ((src >> 11) & 0x1F) * 0xFF / 0x1F;
        dst[1] = ((src >> 05) & 0x3F) * 0xFF / 0x3F;
        dst[2] = ((src >> 00) & 0x1F) * 0xFF / 0x1F;
    }

    void color_b5g6r5_to_x8r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        uint16_t src = *(const uint16_t *)src_pixel;
        uint8_t * dst = (uint8_t *)dst_pixel;

        dst[0] = ((src >> 11) & 0x1F) * 0xFF / 0x1F;
        dst[1] = ((src >> 05) & 0x3F) * 0xFF / 0x3F;
        dst[2] = ((src >> 00) & 0x1F) * 0xFF / 0x1F;
        dst[3] = 0xFF;
    }

    void color_a1r5g5b5_to_x1r5g5b5(const void * src_pixel, void * dst_pixel)
    {
        uint16_t * src = (uint16_t *)src_pixel;
        uint16_t * dst = (uint16_t *)dst_pixel;
        *dst = *src | 0x8000;
    }

    void color_a1r5g5b5_to_a8r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        uint16_t src = *(uint16_t *)src_pixel;
        color_a8r8g8b8_t * dst = (color_a8r8g8b8_t *)dst_pixel;
        dst->a = uint8_t((src & (0x01 << 15)) >> 15) * 0xFF / 0x01;
        dst->r = uint8_t((src & (0x1F << 10)) >> 10) * 0xFF / 0x1F;
        dst->g = uint8_t((src & (0x1F << 5)) >> 5) * 0xFF / 0x1F;
        dst->b = uint8_t((src & (0x1F << 0)) >> 0) * 0xFF / 0x1F;
    }

    void color_a1r5g6b5_to_a8r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        uint32_t src = *(uint32_t *)src_pixel;
        color_a8r8g8b8_t * dst = (color_a8r8g8b8_t *)dst_pixel;
        dst->a = ((src & (0x01 << 16)) >> 16) * 0xFF / 0x01;
        dst->r = ((src & (0x1F << 11)) >> 11) * 0xFF / 0x1F;
        dst->g = ((src & (0x3F << 5)) >> 5) * 0xFF / 0x3F;
        dst->b = ((src & (0x1F << 0)) >> 0) * 0xFF / 0x1F;
    }

    void color_a4r5g6b5_to_a8r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        uint32_t src = *(uint32_t *)src_pixel;
        color_a8r8g8b8_t * dst = (color_a8r8g8b8_t *)dst_pixel;
        dst->a = ((src & (0x0F << 16)) >> 16) * 0xFF / 0x0F;
        dst->r = ((src & (0x1F << 11)) >> 11) * 0xFF / 0x1F;
        dst->g = ((src & (0x3F << 5)) >> 5) * 0xFF / 0x3F;
        dst->b = ((src & (0x1F << 0)) >> 0) * 0xFF / 0x1F;
    }

    void color_x4r4g4b4_to_x1r5g5b5(const void * src_pixel, void * dst_pixel)
    {
        uint16_t * src = (uint16_t *)src_pixel;
        uint16_t * dst = (uint16_t *)dst_pixel;
        *dst =
            ((*src & 0x0F00) << 3) |
            ((*src & 0x00F0) << 2) |
            ((*src & 0x00F) << 1);
    }

    void color_x4r4g4b4_to_x8r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        uint16_t src = *(const uint16_t *)src_pixel;
        uint32_t * dst = (uint32_t *)dst_pixel;
        *dst =
            (0xFF000000) |
            ((((src & (0xF << 8)) >> 8) * 0xFF / 0xF) << 16) |
            ((((src & (0xF << 4)) >> 4) * 0xFF / 0xF) << 8) |
            ((((src & (0xF << 0)) >> 0) * 0xFF / 0xF) << 0);
    }

    void color_x4r4g4b4_to_x8b8g8r8(const void * src_pixel, void * dst_pixel)
    {
        uint16_t src = *(const uint16_t *)src_pixel;
        uint32_t * dst = (uint32_t *)dst_pixel;
        *dst =
            (0xFF000000) |
            ((((src & (0xF << 0)) >> 0) * 0xFF / 0xF) << 16) |
            ((((src & (0xF << 8)) >> 8) * 0xFF / 0xF) << 8) |
            ((((src & (0xF << 12)) >> 12) * 0xFF / 0xF) << 0);
    }

    void color_a4r4g4b4_to_a8r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        uint16_t & src = *(uint16_t *)src_pixel;
        uint32_t & dst = *(uint32_t *)dst_pixel;
        uint32_t a = ((src & 0xF000) >> 12) * 0xFF / 0xF;
        uint32_t r = ((src & 0x0F00) >> 8) * 0xFF / 0xF;
        uint32_t g = ((src & 0x00F0) >> 4) * 0xFF / 0xF;
        uint32_t b = ((src & 0x000F) >> 0) * 0xFF / 0xF;
        dst = (a << 24) | (r << 16) | (g << 8) | b;
    }

    void color_a4r4g4b4_to_a8b8g8r8(const void * src_pixel, void * dst_pixel)
    {
        uint16_t src = *(const uint16_t *)src_pixel;
        uint32_t * dst = (uint32_t *)dst_pixel;
        *dst =
            (src & 0xFF000000) |
            ((((src & 0x0F00) >> 8) * 0xFF / 0xF) << 0) |
            ((((src & 0x00F0) >> 4) * 0xFF / 0xF) << 8) |
            ((((src & 0x000F) >> 0) * 0xFF / 0xF) << 16);
    }

    void color_a4r4g4b4_to_x8b8g8r8(const void * src_pixel, void * dst_pixel)
    {
        uint16_t src = *(const uint16_t *)src_pixel;
        uint32_t * dst = (uint32_t *)dst_pixel;
        *dst =
            (0xFF000000) |
            ((((src >> 0) & 0x0003) * 0xFF / 0x0003) << 16) |
            ((((src >> 2) & 0x0007) * 0xFF / 0x0007) << 8) |
            ((((src >> 5) & 0x0007) * 0xFF / 0x0007) << 0);
    }

    void color_a16b16g16r16_to_a8r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        color_a16b16g16r16_t * src = (color_a16b16g16r16_t *)src_pixel;
        color_a8r8g8b8_t * dst = (color_a8r8g8b8_t *)dst_pixel;
        dst->a = src->a >> 8;
        dst->r = src->r >> 8;
        dst->g = src->g >> 8;
        dst->b = src->b >> 8;
    }

    void color_a16b16g16r16f_to_a8r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        color_a16b16g16r16_t * src = (color_a16b16g16r16_t *)src_pixel;
        color_a8r8g8b8_t * dst = (color_a8r8g8b8_t *)dst_pixel;
        dst->a = color_float16_to_8(src->a);
        dst->r = color_float16_to_8(src->r);
        dst->g = color_float16_to_8(src->g);
        dst->b = color_float16_to_8(src->b);
    }

    void color_g16r16_to_r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        uint32_t * src = (uint32_t *)src_pixel;
        color_r8g8b8_t * dst = (color_r8g8b8_t *)dst_pixel;
        dst->r = (*src & 0x0000FFFF) >> 8;
        dst->g = (*src & 0xFFFF0000) >> 24;
        dst->b = 0xFF;
    }

    void color_g16r16f_to_r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        color_g16r16_t * src = (color_g16r16_t *)src_pixel;
        color_r8g8b8_t * dst = (color_r8g8b8_t *)dst_pixel;
        dst->r = color_float16_to_8(src->r);
        dst->g = color_float16_to_8(src->g);
        dst->b = 0xFF;
    }

    void color_r16f_to_r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        uint16_t * src = (uint16_t *)src_pixel;
        color_r8g8b8_t * dst = (color_r8g8b8_t *)dst_pixel;
        dst->r = color_float16_to_8(*src);
        dst->g = 0xFF;
        dst->b = 0xFF;
    }

    void color_r32f_to_r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        float32_t * src = (float32_t *)src_pixel;
        color_r8g8b8_t * dst = (color_r8g8b8_t *)dst_pixel;
        dst->r = uint8_t(*src * 255.0f);
        dst->g = 0xFF;
        dst->b = 0xFF;
    }

    void color_g32r32f_to_r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        color_g32r32f_t * src = (color_g32r32f_t *)src_pixel;
        color_r8g8b8_t * dst = (color_r8g8b8_t *)dst_pixel;
        dst->r = uint8_t(src->r * 255.0f);
        dst->g = uint8_t(src->g * 255.0f);
        dst->b = 0xFF;
    }

    void color_a32b32g32r32f_to_a8r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        color_a32b32g32r32f_t * src = (color_a32b32g32r32f_t *)src_pixel;
        color_a8r8g8b8_t * dst = (color_a8r8g8b8_t *)dst_pixel;
        dst->a = uint8_t(src->a * 255.0f);
        dst->r = uint8_t(src->r * 255.0f);
        dst->g = uint8_t(src->g * 255.0f);
        dst->b = uint8_t(src->b * 255.0f);
    }

    void color_r3g3b2_to_x1r5g5b5(const void * src_pixel, void * dst_pixel)
    {
        byte_t src = *(const byte_t *)src_pixel;
        uint16_t * dst = (uint16_t *)dst_pixel;
        *dst =
            (0x8000) |
            ((((src >> 0) & 0x3) * 0x1F / 0x3) << 10) |
            ((((src >> 2) & 0x7) * 0x1F / 0x7) << 5) |
            ((((src >> 5) & 0x7) * 0x1F / 0x7) << 0);
    }

    void color_r3g3b2_to_a1r5g5b5(const void * src_pixel, void * dst_pixel)
    {
        byte_t src = *(const byte_t *)src_pixel;
        uint16_t * dst = (uint16_t *)dst_pixel;
        *dst =
            (0x8000) |
            ((((src >> 0) & 0x3) * 0x1F / 0x3) << 10) |
            ((((src >> 2) & 0x7) * 0x1F / 0x7) << 5) |
            ((((src >> 5) & 0x7) * 0x1F / 0x7) << 0);
    }

    void color_r3g3b2_to_r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        byte_t src = *(const byte_t *)src_pixel;
        color_r8g8b8_t * dst = (color_r8g8b8_t *)dst_pixel;
        dst->r = ((src & (0x7 << 5)) >> 5) * 0xFF / 0x7;
        dst->g = ((src & (0x7 << 2)) >> 2) * 0xFF / 0x7;
        dst->b = ((src & (0x3 << 0)) >> 0) * 0xFF / 0x3;
    }

    void color_r3g3b2_to_x8r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        byte_t src = *(const byte_t *)src_pixel;
        uint32_t * dst = (uint32_t *)dst_pixel;
        *dst =
            (0xFF000000) |
            ((((src & (0x7 << 5)) >> 5) * 0xFF / 0x7) << 16) |
            ((((src & (0x7 << 2)) >> 2) * 0xFF / 0x7) << 8) |
            ((((src & (0x3 << 0)) >> 0) * 0xFF / 0x3) << 0);
    }

    void color_a8r3g3b2_to_a8r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        uint16_t src = *(const uint16_t *)src_pixel;
        uint32_t * dst = (uint32_t *)dst_pixel;
        *dst =
            ((((src >> 8) & 0xFF) * 0xFF / 0xFF) << 24) |
            ((((src >> 5) & 0x07) * 0xFF / 0x07) << 16) |
            ((((src >> 2) & 0x07) * 0xFF / 0x07) << 8) |
            ((((src >> 0) & 0x03) * 0xFF / 0x03) << 0);
    }

    void color_a8r3g3b2_to_a8b8g8r8(const void * src_pixel, void * dst_pixel)
    {
        uint16_t src = *(const uint16_t *)src_pixel;
        uint32_t * dst = (uint32_t *)dst_pixel;
        *dst =
            ((src & 0xFF00) << 16) |
            ((((src >> 0) & 0x3) * 0xFF / 0x3) << 16) |
            ((((src >> 2) & 0x7) * 0xFF / 0x7) << 8) |
            ((((src >> 5) & 0x7) * 0xFF / 0x7) << 0);
    }

    void color_r8g8b8_to_b8g8r8(const void * src_pixel, void * dst_pixel)
    {
        color_r8g8b8_t * src = (color_r8g8b8_t *)src_pixel;
        color_r8g8b8_t * dst = (color_r8g8b8_t *)dst_pixel;
        dst->r = src->b;
        dst->g = src->g;
        dst->b = src->r;
    }

    void color_r8g8b8_to_x8r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        color_r8g8b8_t * src = (color_r8g8b8_t *)src_pixel;
        color_a8r8g8b8_t * dst = (color_a8r8g8b8_t *)dst_pixel;
        dst->a = 0xFF;
        dst->rgb = *src;
    }

    void color_r8g8b8_to_r8g8b8a8(const void * src_pixel, void * dst_pixel)
    {
        color_r8g8b8_t * src = (color_r8g8b8_t *)src_pixel;
        color_a8b8g8r8_t * dst = (color_a8b8g8r8_t *)dst_pixel;
        dst->a = 0xFF;
        dst->r = src->r;
        dst->g = src->g;
        dst->b = src->b;
    }

    void color_a8r8g8b8_to_r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        color_a8r8g8b8_t * src = (color_a8r8g8b8_t *)src_pixel;
        color_r8g8b8_t * dst = (color_r8g8b8_t *)dst_pixel;
        *dst = src->rgb;
    }

    void color_a8b8g8r8_to_a8r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        color_a8r8g8b8_t * src = (color_a8r8g8b8_t *)src_pixel;
        color_a8r8g8b8_t * dst = (color_a8r8g8b8_t *)dst_pixel;
        dst->a = src->a;
        dst->r = src->b;
        dst->g = src->g;
        dst->b = src->r;
    }

    void color_x8b8g8r8_to_r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        color_a8r8g8b8_t * src = (color_a8r8g8b8_t *)src_pixel;
        color_r8g8b8_t * dst = (color_r8g8b8_t *)dst_pixel;
        dst->r = src->b;
        dst->g = src->g;
        dst->b = src->r;
    }

    void color_x8r8g8b8_to_r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        color_a8r8g8b8_t * src = (color_a8r8g8b8_t *)src_pixel;
        color_r8g8b8_t * dst = (color_r8g8b8_t *)dst_pixel;
        *dst = src->rgb;
    }

    void color_x8r8g8b8_to_x1r5g5b5(const void * src_pixel, void * dst_pixel)
    {
        color_a8r8g8b8_t * src = (color_a8r8g8b8_t *)src_pixel;
        uint16_t * dst = (uint16_t *)dst_pixel;
        byte_t r = src->r >> 3;
        byte_t g = src->g >> 3;
        byte_t b = src->b >> 3;
        *dst = 0x8000 | (r << 10) | (g << 5) | b;
    }

    void color_x8r8g8b8_to_r5g6b5(const void * src_pixel, void * dst_pixel)
    {
        color_a8r8g8b8_t * src = (color_a8r8g8b8_t *)src_pixel;
        uint16_t * dst = (uint16_t *)dst_pixel;
        byte_t r = src->r >> 3;
        byte_t g = src->g >> 2;
        byte_t b = src->b >> 3;
        *dst = (r << 11) | (g << 5) | b;
    }

    void color_gray8_to_x8r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        byte_t src = *(const byte_t *)src_pixel;
        uint32_t * dst = (uint32_t *)dst_pixel;
        *dst = 0xFF000000 | (src << 16) | (src << 8) | src;
    }

    void color_gray8_to_r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        color_r8g8b8_t * dst_rgb = (color_r8g8b8_t *)dst_pixel;
        byte_t val = *((byte_t *)src_pixel);
        dst_rgb->r = val;
        dst_rgb->g = val;
        dst_rgb->b = val;
    }

    void color_x1r5g5b5_to_a1r5g5b5(const void * src_pixel, void * dst_pixel)
    {
        uint16_t src = *((const uint16_t *)src_pixel);
        uint16_t * dst = (uint16_t *)dst_pixel;
        *dst = src | 0x08000;
    }

    void color_x1r5g5b5_to_r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        uint16_t src_rgb = *((uint16_t *)src_pixel);
        color_r8g8b8_t * dst_rgb = (color_r8g8b8_t *)dst_pixel;
        dst_rgb->r = uint8_t((src_rgb & 0x7C00) >> 7);
        dst_rgb->g = uint8_t((src_rgb & 0x3E0) >> 2);
        dst_rgb->b = uint8_t((src_rgb & 0x1F) << 3);
    }

    void color_x1r5g5b5_to_x8r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        uint16_t src = *((uint16_t *)src_pixel);
        color_a8r8g8b8_t * dst = (color_a8r8g8b8_t *)dst_pixel;
        dst->a = 0xFF;
        dst->r = uint8_t((src & 0x7C00) >> 7);
        dst->g = uint8_t((src & 0x3E0) >> 2);
        dst->b = uint8_t((src & 0x1F) << 3);
    }

    void color_x1r5g5b5_to_x8b8g8r8(const void * src_pixel, void * dst_pixel)
    {
        uint16_t src = *((uint16_t *)src_pixel);
        color_a8b8g8r8_t * dst = (color_a8b8g8r8_t *)dst_pixel;
        dst->a = 0xFF;
        dst->r = uint8_t((src & 0x7C00) >> 7);
        dst->g = uint8_t((src & 0x3E0) >> 2);
        dst->b = uint8_t((src & 0x1F) << 3);
    }

    void color_a8r8g8b8_to_a8r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        color_a8r8g8b8_t * src_argb = (color_a8r8g8b8_t *)src_pixel;
        color_a8r8g8b8_t * dst_argb = (color_a8r8g8b8_t *)dst_pixel;
        *dst_argb = *src_argb;
    }

    void color_yuv_to_r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        color_yuv_t * yuv = (color_yuv_t *)src_pixel;
        color_r8g8b8_t * rgb = (color_r8g8b8_t *)dst_pixel;
        int32_t temp = 0;

        temp = (int32_t)(yuv->y + 1.402f * (yuv->v - 128) + 0.03125f);
        if (temp < 0)
            rgb->r = 0;
        else if (temp > 0xFF)
            rgb->r = 0xFF;
        else
            rgb->r = (byte_t)temp;

        temp = (int32_t)(yuv->y - 0.34413f * (yuv->u - 128) - 0.71414f * (yuv->v - 128));
        if (temp < 0)
            rgb->g = 0;
        else if (temp > 0xFF)
            rgb->g = 0xFF;
        else
            rgb->g = (byte_t)temp;

        temp = (int32_t)(yuv->y + 1.772f * (yuv->u - 128));
        if (temp < 0)
            rgb->b = 0;
        else if (temp > 0xFF)
            rgb->b = 0xFF;
        else
            rgb->b = (byte_t)temp;
    }

    void color_r8g8b8_to_yuv(const void * src_pixel, void * dst_pixel)
    {
        color_r8g8b8_t * src = (color_r8g8b8_t *)src_pixel;
        color_yuv_t * dst = (color_yuv_t *)dst_pixel;
        dst->y = uint8_t(0.299f * src->r + 0.587f * src->g + 0.114f * src->b);
        dst->u = uint8_t(0.169 * src->r - 0.3313f * src->g + 0.5f * src->b);
        dst->v = uint8_t(0.5f * src->r - 0.4187f * src->g - 0.0813f * src->b);
    }

    void color_yuv_to_a8r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        color_yuv_t * yuv = (color_yuv_t *)src_pixel;
        color_a8r8g8b8_t * argb = (color_a8r8g8b8_t *)dst_pixel;
        argb->a = 0xFF;
        color_yuv_to_r8g8b8(yuv, (color_r8g8b8_t *)argb);
    }

    void color_ycck_to_r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        color_ycck_t * ycck = (color_ycck_t *)src_pixel;
        color_r8g8b8_t * rgb = (color_r8g8b8_t *)dst_pixel;
        int32_t temp = 0;

        temp = (int32_t)(ycck->y + 1.402f * (ycck->r - 128) + 0.03125f);
        if (temp < 0)
            rgb->r = 0;
        else if (temp > 0xFF)
            rgb->r = 0xFF;
        else
            rgb->r = (byte_t)temp;

        temp = (int32_t)(ycck->y - 0.34413f * (ycck->b - 128) - 0.71414f * (ycck->r - 128));
        if (temp < 0)
            rgb->g = 0;
        else if (temp > 0xFF)
            rgb->g = 0xFF;
        else
            rgb->g = (byte_t)temp;

        temp = (int32_t)(ycck->y + 1.772f * (ycck->b - 128));
        if (temp < 0)
            rgb->b = 0;
        else if (temp > 0xFF)
            rgb->b = 0xFF;
        else
            rgb->b = (byte_t)temp;
    }

    void color_ycck_to_a8r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        color_ycck_t * ycck = (color_ycck_t *)src_pixel;
        color_a8r8g8b8_t * argb = (color_a8r8g8b8_t *)dst_pixel;
        argb->a = 0xFF;
        color_ycck_to_r8g8b8(ycck, (color_r8g8b8_t *)argb);
    }

    void color_gray_to_r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        byte_t * gray = (byte_t *)src_pixel;
        color_r8g8b8_t * rgb = (color_r8g8b8_t *)dst_pixel;
        rgb->r = *gray;
        rgb->g = *gray;
        rgb->b = *gray;
    }

    void color_gray_to_a8r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        byte_t * gray = (byte_t *)src_pixel;
        color_a8r8g8b8_t * argb = (color_a8r8g8b8_t *)dst_pixel;
        argb->a = 0xFF;
        argb->r = *gray;
        argb->g = *gray;
        argb->b = *gray;
    }

    void color_cmyk_to_r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        color_cmyk_t * cmyk = (color_cmyk_t *)src_pixel;
        color_r8g8b8_t * rgb = (color_r8g8b8_t *)dst_pixel;
        rgb->r = (cmyk->k * cmyk->c) >> 8;
        rgb->g = (cmyk->k * cmyk->m) >> 8;
        rgb->b = (cmyk->k * cmyk->y) >> 8;
    }

    void color_cmyk_to_a8r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        color_cmyk_t * cmyk = (color_cmyk_t *)src_pixel;
        color_a8r8g8b8_t * argb = (color_a8r8g8b8_t *)dst_pixel;
        argb->a = 0xFF;
        color_cmyk_to_r8g8b8(cmyk, (color_r8g8b8_t *)argb);
    }

    void color_r8g8b8_to_x32y32z32f(const void * src_pixel, void * dst_pixel)
    {
        color_r8g8b8_t * src = (color_r8g8b8_t *)src_pixel;
        color_x32y32z32f_t * dst = (color_x32y32z32f_t *)dst_pixel;

        float32_t r = src->r / 255.0f;
        float32_t g = src->g / 255.0f;
        float32_t b = src->b / 255.0f;

        dst->x = 0.412453f * r + 0.357580f * g + 0.180423f * b;
        dst->y = 0.212671f * r + 0.715160f * g + 0.072169f * b;
        dst->z = 0.019334f * r + 0.119193f * g + 0.950227f * b;
    }

    void color_x32y32z32f_tor_r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        color_x32y32z32f_t * src = (color_x32y32z32f_t *)src_pixel;
        color_r8g8b8_t * dst = (color_r8g8b8_t *)dst_pixel;

        float32_t r = 3.240479f * src->x - 1.537150f * src->y - 0.498535f * src->z;
        float32_t g = -0.969256f * src->x + 1.875992f * src->y + 0.041556f * src->z;
        float32_t b = 0.055648f * src->x - 0.204043f * src->y + 1.057311f * src->z;

        if (r <= 0.0f) dst->r = 0;
        else if (r >= 1.0f) dst->r = 0xFF;
        else dst->r = uint8_t(r * 0xFF);

        if (g <= 0.0f) dst->g = 0;
        else if (g >= 1.0f) dst->g = 0xFF;
        else dst->g = uint8_t(g * 0xFF);

        if (b <= 0.0f) dst->b = 0;
        else if (r >= 1.0f) dst->b = 0xFF;
        else dst->b = uint8_t(b * 0xFF);
    }

    void color_r8g8b8x8_to_r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        uint32_t src = *(const uint32_t *)src_pixel;
        color_r8g8b8_t * dst = (color_r8g8b8_t *)dst_pixel;
        dst->r = (src >> 24) & 0xFF;
        dst->g = (src >> 16) & 0xFF;
        dst->b = (src >> 8) & 0xFF;
    }

    void color_r8g8b8x8_to_x8b8g8r8(const void * src_pixel, void * dst_pixel)
    {
        uint32_t src = *(const uint32_t *)src_pixel;
        uint32_t * dst = (uint32_t *)dst_pixel;
        *dst = (0xFF000000) | ((src << 8) & 0xFF0000) | ((src >> 8) & 0xFF00) | ((src >> 24) & 0xFF);
    }

    void color_r8g8b8a8_to_b8g8r8a8(const void * src_pixel, void * dst_pixel)
    {
        const color_r8g8b8a8_t * src = (color_r8g8b8a8_t *)src_pixel;
        color_b8g8r8a8_t * dst = (color_b8g8r8a8_t *)dst_pixel;
        dst->a = src->a;
        dst->r = src->r;
        dst->g = src->g;
        dst->b = src->b;
    }

    void color_a8r8g8b8_to_a8b8g8r8(const void * src_pixel, void * dst_pixel)
    {
        const color_a8r8g8b8_t * src = (color_a8r8g8b8_t *)src_pixel;
        color_a8b8g8r8_t * dst = (color_a8b8g8r8_t *)dst_pixel;
        dst->a = src->a;
        dst->r = src->r;
        dst->g = src->g;
        dst->b = src->b;
    }

    void color_x8r8g8b8_to_x8b8g8r8(const void * src_pixel, void * dst_pixel)
    {
        const color_a8r8g8b8_t * src = (const color_a8r8g8b8_t *)src_pixel;
        color_a8b8g8r8_t * dst = (color_a8b8g8r8_t *)dst_pixel;
        dst->a = 0xFF;
        dst->r = src->r;
        dst->g = src->g;
        dst->b = src->b;
    }

    void color_x8b8g8r8_to_x8r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        const color_a8b8g8r8_t * src = (const color_a8b8g8r8_t *)src_pixel;
        color_a8r8g8b8_t * dst = (color_a8r8g8b8_t *)dst_pixel;
        dst->a = 0xFF;
        dst->r = src->r;
        dst->g = src->g;
        dst->b = src->b;
    }

    void color_r8g8b8_to_a8b8g8r8(const void * src_pixel, void * dst_pixel)
    {
        const color_r8g8b8_t * src = (color_r8g8b8_t *)src_pixel;
        color_a8b8g8r8_t * dst = (color_a8b8g8r8_t *)dst_pixel;
        dst->a = 0xFF;
        dst->r = src->r;
        dst->g = src->g;
        dst->b = src->b;
    }

    void color_r8g8b8_to_x8b8g8r8(const void * src_pixel, void * dst_pixel)
    {
        const color_r8g8b8_t * src = (color_r8g8b8_t *)src_pixel;
        color_a8b8g8r8_t * dst = (color_a8b8g8r8_t *)dst_pixel;
        dst->a = 0xFF;
        dst->r = src->r;
        dst->g = src->g;
        dst->b = src->b;
    }

    void color_b8g8r8_to_a8b8g8r8(const void * src_pixel, void * dst_pixel)
    {
        const color_b8g8r8_t * src = (color_b8g8r8_t *)src_pixel;
        *(uint32_t *)dst_pixel =
            0xFF000000 |
            (src->b << 16) |
            (src->g << 8) |
            (src->r << 0);
    }

    void color_b8g8r8_to_x8r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        const color_b8g8r8_t * src = (color_b8g8r8_t *)src_pixel;
        *(uint32_t *)dst_pixel =
            0xFF000000 |
            (src->r << 16) |
            (src->g << 8) |
            (src->b << 0);
    }

    void color_a2r10g10b10_to_a8r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        uint32_t src = *(const uint32_t *)src_pixel;
        *(uint32_t *)dst_pixel =
            ((((src & 0xC0000000) >> 0x1E) * 0xFF / 0x003) << 0x18) |
            ((((src & 0x3FF00000) >> 0x14) * 0xFF / 0x3FF) << 0x10) |
            ((((src & 0x000FFC00) >> 0x0A) * 0xFF / 0x3FF) << 0x08) |
            ((((src & 0x000003FF) >> 0x00) * 0xFF / 0x3FF) << 0x00);
    }

    void color_a2b10g10r10_to_a8r8g8b8(const void * src_pixel, void * dst_pixel)
    {
        uint32_t src = *(const uint32_t *)src_pixel;
        *(uint32_t *)dst_pixel =
            ((((src & 0xC0000000) >> 0x1E) * 0xFF / 0x003) << 0x18) |
            ((((src & 0x000003FF) >> 0x00) * 0xFF / 0x3FF) << 0x10) |
            ((((src & 0x000FFC00) >> 0x0A) * 0xFF / 0x3FF) << 0x08) |
            ((((src & 0x3FF00000) >> 0x14) * 0xFF / 0x3FF) << 0x00);
    }

    void color_a2b10g10r10_to_a2b10g10r10(const void * src_pixel, void * dst_pixel)
    {
        uint32_t src = *(const uint32_t *)src_pixel;
        uint32_t * dst = (uint32_t *)dst_pixel;
        *dst = (src & 0xC0000000) | ((src & 0x3FF00000) >> 20) | (src & 0xFFC00) | ((src & 0x3FF) << 20);
    }

    void color_a2b10g10r10_to_a2r10g10b10(const void * src_pixel, void * dst_pixel)
    {
        uint32_t src = *(const uint32_t *)src_pixel;
        uint32_t * dst = (uint32_t *)dst_pixel;
        *dst = (src & 0xC0000000) | ((src & 0x3FF00000) >> 20) | (src & 0xFFC00) | ((src & 0x3FF) << 20);
    }
}
