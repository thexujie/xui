#include "stdafx.h"
#include "format.h"

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

namespace core
{
    using namespace core;
    using namespace core::image::formats;


    int32_t format_bits(format cmode)
    {
        switch (cmode)
        {
        case format::gray1: return 1;
        case format::gray2: return 2;
        case format::gray4: return 4;
        case format::gray8: return 8;

        case format::r3g3b2:
            return 8;

        case format::r8g8b8:
        case format::b8g8r8:
            return 24;

        case format::r5g6b5:
        case format::a1r5g5b5:
        case format::a1b5g5r5:
        case format::x1r5g5b5:
        case format::x1b5g5r5:
        case format::a4r4g4b4:
        case format::x4r4g4b4:
        case format::a8r3g3b2:
            return 16;

        case format::a8r8g8b8:
        case format::x8r8g8b8:
        case format::a8b8g8r8:
        case format::x8b8g8r8:
        case format::r8g8b8a8:
        case format::r8g8b8x8:
        case format::b8g8r8a8:
        case format::b8g8r8x8:
        case format::g16r16:
        case format::a2r10g10b10:
        case format::a2b10g10r10:
            return 32;

        case format::a16b16g16r16:
            return 64;

        case format::r16f: return 16;
        case format::g16r16f: return 32;
        case format::a16b16g16r16f: return 64;
        case format::r32f: return 32;
        case format::g32r32f: return 64;
        case format::b32g32r32f: return 96;
        case format::a32b32g32r32f: return 128;

        case format::x32y32z32f: return 96;

        default:
            return 0;
        }
    }

#define COLOR_MODE_TEXT(mode) case mode: return (#mode)

    const char * format_text(format cmode)
    {
        switch (cmode)
        {
        COLOR_MODE_TEXT(format::none);
        COLOR_MODE_TEXT(format::gray2);
        COLOR_MODE_TEXT(format::gray4);
        COLOR_MODE_TEXT(format::gray8);

        COLOR_MODE_TEXT(format::r8g8b8);
        COLOR_MODE_TEXT(format::a8r8g8b8);
        COLOR_MODE_TEXT(format::x8r8g8b8);

        COLOR_MODE_TEXT(format::b8g8r8);
        COLOR_MODE_TEXT(format::a8b8g8r8);
        COLOR_MODE_TEXT(format::x8b8g8r8);

        COLOR_MODE_TEXT(format::yuyv);
        COLOR_MODE_TEXT(format::cmyk);
        COLOR_MODE_TEXT(format::ycck);

        COLOR_MODE_TEXT(format::r5g6b5);
        COLOR_MODE_TEXT(format::a8r5g6b5);
        COLOR_MODE_TEXT(format::a1r5g5b5);
        COLOR_MODE_TEXT(format::a1b5g5r5);
        COLOR_MODE_TEXT(format::x1r5g5b5);
        COLOR_MODE_TEXT(format::a4r4g4b4);
        COLOR_MODE_TEXT(format::x4r4g4b4);
        COLOR_MODE_TEXT(format::g16r16);
        COLOR_MODE_TEXT(format::a16b16g16r16);
        COLOR_MODE_TEXT(format::r3g3b2);

        COLOR_MODE_TEXT(format::r8g8b8a8);
        COLOR_MODE_TEXT(format::r8g8b8x8);

        COLOR_MODE_TEXT(format::r16f);
        COLOR_MODE_TEXT(format::g16r16f);
        COLOR_MODE_TEXT(format::a16b16g16r16f);
        COLOR_MODE_TEXT(format::r32f);
        COLOR_MODE_TEXT(format::g32r32f);
        COLOR_MODE_TEXT(format::a32b32g32r32f);

        COLOR_MODE_TEXT(format::x32y32z32f);

        default:
            return "color_mode unkonwn";
        }
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
                return format::r3g3b2;
            break;
        case 16:
            if (mask_equal(mask, MASK_A8R3G3B2))
                return format::a8r3g3b2;
            if (mask_equal(mask, MASK_A4R4G4B4))
                return format::a4r4g4b4;
            if (mask_equal(mask, MASK_X4R4G4B4))
                return format::x4r4g4b4;
            if (mask_equal(mask, MASK_A0R5G6B5))
                return format::r5g6b5;
            if (mask_equal(mask, MASK_X1R5G5B5))
                return format::x1r5g5b5;
            if (mask_equal(mask, MASK_A1R5G5B5))
                return format::a1r5g5b5;
            break;
        case 24:
            if (mask_equal(mask, MASK_R8G8B8))
                return format::r8g8b8;
            if (mask_equal(mask, MASK_B8G8R8))
                return format::b8g8r8;
            break;
        case 32:
            if (mask_equal(mask, MASK_X8R8G8B8))
                return format::x8r8g8b8;
            if (mask_equal(mask, MASK_A8R8G8B8))
                return format::a8r8g8b8;
            if (mask_equal(mask, MASK_R8G8B8X8))
                return format::r8g8b8x8;
            if (mask_equal(mask, MASK_R8G8B8A8))
                return format::r8g8b8a8;
            if (mask_equal(mask, MASK_X8B8G8R8))
                return format::x8b8g8r8;
            if (mask_equal(mask, MASK_A8B8G8R8))
                return format::a8b8g8r8;

            // dds file format format::a2b10g10r10 is error, it should be format::a2r10g10b10.
            if (mask_equal(mask, MASK_A2R10G10B10))
                return format::a2b10g10r10;
            if (mask_equal(mask, MASK_A2B10G10R10))
                return format::a2r10g10b10;


            if (mask_equal(mask, MASK_G16R16))
                return format::g16r16;
            break;
		default:
			break;
        }
        return format::none;
    }

    color_mask_abgr_t mask_from_format_abgr(format format)
    {
        switch(format)
        {
        case format::r3g3b2: return MASK_R3G3B2;
        case format::a8r3g3b2: return MASK_A8R3G3B2;
        case format::a4r4g4b4: return MASK_A4R4G4B4;
        case format::x4r4g4b4: return MASK_X4R4G4B4;
        case format::r5g6b5: return MASK_A0R5G6B5;
        case format::x1r5g5b5: return MASK_X1R5G5B5;
        case format::a1r5g5b5: return MASK_A1R5G5B5;
        case format::r8g8b8: return MASK_R8G8B8;
        case format::b8g8r8: return MASK_B8G8R8;
        case format::x8r8g8b8: return MASK_X8R8G8B8;
        case format::a8r8g8b8: return MASK_A8R8G8B8;
        case format::x8b8g8r8: return MASK_X8B8G8R8;
        case format::a8b8g8r8: return MASK_A8B8G8R8;
        case format::r8g8b8x8: return MASK_R8G8B8X8;
        case format::r8g8b8a8: return MASK_R8G8B8A8;
        case format::g16r16: return MASK_G16R16;
        case format::a2r10g10b10: return MASK_A2R10G10B10;
        case format::a2b10g10r10: return MASK_A2B10G10R10;
        default: return {};
        }
    }

    pixel_convert_fun image_get_samapler(format src, format dst)
    {
    	struct format_map_item
    	{
			format src;
			format dst;

    		bool operator < (const format_map_item & another) const
    		{
				if (int(src) < int(another.src))
					return true;
				else if (int(src) == int(another.src))
					return int(dst) < int(another.dst);
				else
					return false;
    		}
    	};

		static const std::map<format_map_item, pixel_convert_fun> pfns =
        {
			{ { format::gray8, format::gray8 }, color_8_to_8 },
			{ { format::gray8, format::r3g3b2 }, color_8_to_8 },
			{ { format::gray8, format::r8g8b8 }, color_gray8_to_r8g8b8 },
			{ { format::gray8, format::x8r8g8b8 }, color_gray8_to_x8r8g8b8 },

			{ { format::r3g3b2, format::r3g3b2 }, color_8_to_8 },
			{ { format::r3g3b2, format::r8g8b8 }, color_r3g3b2_to_r8g8b8 },

			{ { format::r5g6b5, format::r5g6b5 }, color_16_to_16 },
			{ { format::b5g6r5, format::b5g6r5 }, color_16_to_16 },
			{ { format::r8g8b8, format::r8g8b8 }, color_24_to_24 },
			{ { format::r8g8b8, format::x8r8g8b8 }, color_r8g8b8_to_x8r8g8b8 },
			{ { format::b8g8r8, format::b8g8r8 }, color_24_to_24 },
			{ { format::b8g8r8, format::r8g8b8 }, color_r8g8b8_to_b8g8r8 },
			{ { format::b8g8r8, format::b8g8r8x8 }, color_r8g8b8_to_r8g8b8a8 },
			{ { format::b8g8r8, format::b8g8r8a8 }, color_r8g8b8_to_r8g8b8a8 },
			{ { format::x1r5g5b5, format::x1r5g5b5 }, color_16_to_16 },
			{ { format::x1r5g5b5, format::r8g8b8 }, color_x1r5g5b5_to_r8g8b8 },
			{ { format::a4r4g4b4, format::a4r4g4b4 }, color_16_to_16 },
			{ { format::x4r4g4b4, format::x4r4g4b4 }, color_16_to_16 },

			{ { format::a8r8g8b8, format::a8r8g8b8 }, color_32_to_32 },
			{ { format::a8b8g8r8, format::a8b8g8r8 }, color_32_to_32 },
			{ { format::a8b8g8r8, format::a8r8g8b8 }, color_a8b8g8r8_to_a8r8g8b8 },
			{ { format::x8r8g8b8, format::x8r8g8b8 }, color_32_to_32 },
			{ { format::x8b8g8r8, format::x8b8g8r8 }, color_32_to_32 },
			{ { format::r8g8b8x8, format::r8g8b8x8 }, color_32_to_32 },
			{ { format::b8g8r8x8, format::b8g8r8x8 }, color_32_to_32 },

			{ { format::a4r5g6b5, format::a8r8g8b8 }, color_dds_a4r5g6b5_to_a8r8g8b8 },
			{ { format::a8r5g6b5, format::a8r8g8b8 }, color_dds_a8r5g6b5_to_a8r8g8b8 },
        };

		auto iter = pfns.find({ src, dst });
		return iter == pfns.end() ? nullptr : iter->second;
    }

    image_format image_get_format(image_type type, image_format format)
    {
        image_format result = format;
        switch(format.format)
        {
        case format::b8g8r8: result.format = format::r8g8b8; break;
        case format::a8b8g8r8: result.format = format::a8r8g8b8; break;
        default: break;
        }
        return result;
    }
}
