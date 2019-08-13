#pragma once

#include "../core/format.h"

namespace core
{
    /**
    * ͼ���ʽ
    */
    enum image_type
    {
        image_type_none = 0,
        image_type_bmp = core::make_fourcc('b', 'm', 'p', 0),
        image_type_tga = core::make_fourcc('t', 'g', 'a', 0),
        image_type_png = core::make_fourcc('p', 'n', 'g', 0),
        image_type_jpeg = core::make_fourcc('j', 'p', 'g', 0),
        image_type_dds = core::make_fourcc('d', 'd', 's', 0),
        image_type_user = core::make_fourcc('u', 's', 'e', 'r'),
    };

    const char * format_text(format cmode);

    /**
    * ֻ�ܴ��� argb ��ʽ��
    */
    int32_t format_bits(format cmode);

#pragma pack(push, 1)

    struct image_frame_t
    {
        int32_t width;
        int32_t height;
        int32_t pitch;
        int32_t bits;
        int32_t length;
        format mode;
        const byte_t * buffer;
    };

    struct color_yuv_t
    {
        uint8_t y;
        uint8_t u;
        uint8_t v;
    };


    struct color_ycck_t
    {
        union
        {
            uint32_t color;

            struct
            {
                uint8_t y;
                uint8_t b;
                uint8_t r;
                uint8_t k;
            };
        };
    };


    struct color_r8g8b8_t
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
    };


    struct color_b8g8r8_t
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };


    struct color_a8b8g8r8_t
    {
        union
        {
            uint32_t color;

            struct
            {
                uint8_t r;
                uint8_t g;
                uint8_t b;
                uint8_t a;
            };
        };
    };


    struct color_a8r8g8b8_t
    {
        union
        {
            uint32_t color;

            struct
            {
                union
                {
                    color_r8g8b8_t rgb;

                    struct
                    {
                        uint8_t b;
                        uint8_t g;
                        uint8_t r;
                    };
                };

                uint8_t a;
            };
        };
    };


    struct color_r5g6b5
    {
        union
        {
            uint16_t color;

            struct
            {
                uint16_t b : 5;
                uint16_t g : 6;
                uint16_t r : 5;
            };
        };
    };


    struct color_b8g8r8a8_t
    {
        union
        {
            uint32_t color;

            struct
            {
                uint8_t a;

                union
                {
                    color_b8g8r8_t bgr;

                    struct
                    {
                        uint8_t r;
                        uint8_t g;
                        uint8_t b;
                    };
                };
            };
        };
    };


    struct color_r8g8b8a8_t
    {
        union
        {
            uint32_t color;

            struct
            {
                union
                {
                    uint8_t a;
                    uint8_t x;
                };

                union
                {
                    color_r8g8b8_t rgb;

                    struct
                    {
                        uint8_t b;
                        uint8_t g;
                        uint8_t r;
                    };
                };
            };
        };
    };


    struct color_cmyk_t
    {
        uint8_t c;
        uint8_t m;
        uint8_t y;
        uint8_t k;
    };


    struct color_a16b16g16r16_t
    {
        uint16_t r;
        uint16_t g;
        uint16_t b;
        uint16_t a;
    };


    struct color_g16r16_t
    {
        uint16_t r;
        uint16_t g;
    };


    struct color_g32r32f_t
    {
        float32_t r;
        float32_t g;
    };


    struct color_a32b32g32r32f_t
    {
        float32_t r;
        float32_t g;
        float32_t b;
        float32_t a;
    };
#pragma pack(pop)

    struct color_x32y32z32f_t
    {
        float32_t x;
        float32_t y;
        float32_t z;
    };

    //////////////////////////////////////////////////////////////////////////
    /// ��ɫ����


    struct color_mask_abgr_t
    {
        uint32_t r = 0;
        uint32_t g = 0;
        uint32_t b = 0;
        uint32_t a = 0;

        operator bool() const { return (r || g || b || a); }
    };

    struct image_format
    {
        int32_t width = 0;
        int32_t height = 0;
        format format = format::none;
    };

    inline bool operator == (const image_format & lhs, const image_format & rhs)
    {
        return lhs.width == rhs.width && lhs.height == rhs.height && lhs.format == rhs.format;
    }

    inline bool operator != (const image_format & lhs, const image_format & rhs)
    {
        return !operator==(lhs, rhs);
    }

    const color_mask_abgr_t MASK_R3G3B2 = { 0x00E0, 0x001C, 0x0003, 0x0000 };

    const color_mask_abgr_t MASK_A8R3G3B2 = { 0x00E0, 0x001C, 0x0003, 0xFF00 };

    const color_mask_abgr_t MASK_A4R4G4B4 = { 0x0F00, 0x00F0, 0x000F, 0xF000 };
    const color_mask_abgr_t MASK_X4R4G4B4 = { 0x0F00, 0x00F0, 0x000F, 0x0000 };

    const color_mask_abgr_t MASK_A0R5G6B5 = { 0xF800, 0x07E0, 0x001F, 0x0000 };

    const color_mask_abgr_t MASK_X1R5G5B5 = { 0x7C00, 0x03E0, 0x001F, 0x0000 };
    const color_mask_abgr_t MASK_A1R5G5B5 = { 0x7C00, 0x03E0, 0x001F, 0x8000 };

    const color_mask_abgr_t MASK_R8G8B8 = { 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 };
    const color_mask_abgr_t MASK_B8G8R8 = { 0x000000FF, 0x0000FF00, 0x00FF0000, 0x00000000 };

    const color_mask_abgr_t MASK_X8R8G8B8 = MASK_R8G8B8;
    const color_mask_abgr_t MASK_A8R8G8B8 = { 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000 };

    const color_mask_abgr_t MASK_X8B8G8R8 = MASK_B8G8R8;
    const color_mask_abgr_t MASK_A8B8G8R8 = { 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000 };

    const color_mask_abgr_t MASK_R8G8B8X8 = { 0xFF000000, 0x00FF0000, 0x0000FF00, 0x00000000 };
    const color_mask_abgr_t MASK_R8G8B8A8 = { 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF };

    const color_mask_abgr_t MASK_G16R16 = { 0x0000FFFF, 0xFFFF0000, 0x00000000, 0x00000000 };

    const color_mask_abgr_t MASK_A2R10G10B10 = { 0x3FF00000, 0x000FFC00, 0x000003FF, 0xC0000000 };
    const color_mask_abgr_t MASK_A2B10G10R10 = { 0x000003FF, 0x000FFC00, 0x3FF00000, 0xC0000000 };

    inline byte_t * image_malloc(int32_t size)
    {
        return (byte_t *)malloc(size);
    }

    inline void image_free(void * dst)
    {
        if (dst)
            free(dst);
    }

    inline void image_clear(void * dst, int32_t length)
    {
        memset(dst, 0, length);
    }

    inline void image_memcpy(void * dst, int32_t dst_size, const void * src, int32_t length)
    {
        memcpy_s(dst, dst_size, src, length);
    }

	template<typename T>
	inline T align_to(T val, T align)
	{
		return (val + align - 1) / align * align;
	}

	inline int32_t align_to_4(int32_t val)
	{
		return val + 3 & ~3;
	}

	const int32_t IMAGE_CONVERT_FLIP_Y = 0x1;
	// ת��ʱ�Ƿ���Ҫ��ת����
	typedef byte_t * (*image_buffer_alloc_fun)(image_format format);
	typedef void(*image_buffer_free_fun)(byte_t * ptr);

	struct image_data_t
	{
		image_format format;
		int32_t pitch = 0;
		byte_t * data = nullptr;
		byte_t * palette = nullptr;
		int32_t palette_size = 0;
		void (*pfn_free)(image_data_t &) = nullptr;
	};

	void image_buffer_alloc_default(image_data_t & data, int32_t align = 4);
	void image_buffer_free(image_data_t & data);

	typedef void(*pixel_convert_fun)(const void * src_pixel, void * dst_pixel);
	typedef core::error(*image_convert_fun)(struct image_codec_context & ctx, const image_data_t & src, image_data_t & dst);
	
	pixel_convert_fun image_get_samapler(format src, format dst);
	image_format image_get_format(image_type type, image_format format);

	struct image_codec_context
	{
		image_type type = image_type_none;

		decltype(image_get_format) * get_format = image_get_format;
		decltype(image_get_samapler) * get_sampler = image_get_samapler;
		decltype(image_buffer_alloc_default) * pfn_alloc = image_buffer_alloc_default;
		decltype(image_buffer_free) * pfn_free = image_buffer_free;
	};
}
