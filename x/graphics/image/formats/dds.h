#pragma once

#include "graphics/image/image.h"

namespace graphics::image::formats
{
#define DDS_MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
	((uint32_t)(uint8_t)(ch0) | ((uint32_t)(uint8_t)(ch1) << 8) | \
	((uint32_t)(uint8_t)(ch2) << 16) | ((uint32_t)(uint8_t)(ch3) << 24))

    const uint8_t DDS_HEADER[4] = { 0x44, 0x44, 0x53, 0x20 };
    const uint32_t DDS_MAGIC = DDS_MAKEFOURCC('D', 'D', 'S', ' ');

    enum dxt_format_e
    {
        dxt_format_rgb = -1,
        dxt_format_invalid = 0,
        // bc1
        dxt_format_dxt1 = DDS_MAKEFOURCC('D', 'X', 'T', '1'),
        // bc2
        dxt_format_dxt2 = DDS_MAKEFOURCC('D', 'X', 'T', '2'),
        dxt_format_dxt3 = DDS_MAKEFOURCC('D', 'X', 'T', '3'),
        // bc3
        dxt_format_dxt4 = DDS_MAKEFOURCC('D', 'X', 'T', '4'),
        dxt_format_dxt5 = DDS_MAKEFOURCC('D', 'X', 'T', '5'),
        // bc4
        dxt_format_bc4s = DDS_MAKEFOURCC('B', 'C', '4', 'S'),
        dxt_format_bc4u = DDS_MAKEFOURCC('B', 'C', '4', 'U'),
        dxt_format_ati1 = DDS_MAKEFOURCC('A', 'T', 'I', '1'),
        // bc5
        dxt_format_bc5s = DDS_MAKEFOURCC('B', 'C', '5', 'S'),
        dxt_format_bc5u = DDS_MAKEFOURCC('B', 'C', '5', 'U'),
        dxt_format_ati2 = DDS_MAKEFOURCC('A', 'T', 'I', '2'),
        // dx10
        dxt_format_dx10 = DDS_MAKEFOURCC('D', 'X', '1', '0'),


        dxt_format_a16b16g16r16 = 36,

        dxt_format_r16f = 111,
        dxt_format_g16r16f = 112,
        dxt_format_a16b16g16r16f = 113,
        dxt_format_r32f = 114,
        dxt_format_g32r32f = 115,
        dxt_format_a32b32g32r32f = 116,
    };

    enum dxgi_format_e
    {
        dxgi_format_unknown = 0,
        dxgi_format_r32g32b32a32_typeless = 1,
        dxgi_format_r32g32b32a32_float = 2,
        dxgi_format_r32g32b32a32_uint = 3,
        dxgi_format_r32g32b32a32_sint = 4,
        dxgi_format_r32g32b32_typeless = 5,
        dxgi_format_r32g32b32_float = 6,
        dxgi_format_r32g32b32_uint = 7,
        dxgi_format_r32g32b32_sint = 8,
        dxgi_format_r16g16b16a16_typeless = 9,
        dxgi_format_r16g16b16a16_float = 10,
        dxgi_format_r16g16b16a16_unorm = 11,
        dxgi_format_r16g16b16a16_uint = 12,
        dxgi_format_r16g16b16a16_snorm = 13,
        dxgi_format_r16g16b16a16_sint = 14,
        dxgi_format_r32g32_typeless = 15,
        dxgi_format_r32g32_float = 16,
        dxgi_format_r32g32_uint = 17,
        dxgi_format_r32g32_sint = 18,
        dxgi_format_r32g8x24_typeless = 19,
        dxgi_format_d32_float_s8x24_uint = 20,
        dxgi_format_r32_float_x8x24_typeless = 21,
        dxgi_format_x32_typeless_g8x24_uint = 22,
        dxgi_format_r10g10b10a2_typeless = 23,
        dxgi_format_r10g10b10a2_unorm = 24,
        dxgi_format_r10g10b10a2_uint = 25,
        dxgi_format_r11g11b10_float = 26,
        dxgi_format_r8g8b8a8_typeless = 27,
        dxgi_format_r8g8b8a8_unorm = 28,
        dxgi_format_r8g8b8a8_unorm_srgb = 29,
        dxgi_format_r8g8b8a8_uint = 30,
        dxgi_format_r8g8b8a8_snorm = 31,
        dxgi_format_r8g8b8a8_sint = 32,
        dxgi_format_r16g16_typeless = 33,
        dxgi_format_r16g16_float = 34,
        dxgi_format_r16g16_unorm = 35,
        dxgi_format_r16g16_uint = 36,
        dxgi_format_r16g16_snorm = 37,
        dxgi_format_r16g16_sint = 38,
        dxgi_format_r32_typeless = 39,
        dxgi_format_d32_float = 40,
        dxgi_format_r32_float = 41,
        dxgi_format_r32_uint = 42,
        dxgi_format_r32_sint = 43,
        dxgi_format_r24g8_typeless = 44,
        dxgi_format_d24_unorm_s8_uint = 45,
        dxgi_format_r24_unorm_x8_typeless = 46,
        dxgi_format_x24_typeless_g8_uint = 47,
        dxgi_format_r8g8_typeless = 48,
        dxgi_format_r8g8_unorm = 49,
        dxgi_format_r8g8_uint = 50,
        dxgi_format_r8g8_snorm = 51,
        dxgi_format_r8g8_sint = 52,
        dxgi_format_r16_typeless = 53,
        dxgi_format_r16_float = 54,
        dxgi_format_d16_unorm = 55,
        dxgi_format_r16_unorm = 56,
        dxgi_format_r16_uint = 57,
        dxgi_format_r16_snorm = 58,
        dxgi_format_r16_sint = 59,
        dxgi_format_r8_typeless = 60,
        dxgi_format_r8_unorm = 61,
        dxgi_format_r8_uint = 62,
        dxgi_format_r8_snorm = 63,
        dxgi_format_r8_sint = 64,
        dxgi_format_a8_unorm = 65,
        dxgi_format_r1_unorm = 66,
        dxgi_format_r9g9b9e5_sharedexp = 67,
        dxgi_format_r8g8_b8g8_unorm = 68,
        dxgi_format_g8r8_g8b8_unorm = 69,
        dxgi_format_bc1_typeless = 70,
        dxgi_format_bc1_unorm = 71,
        dxgi_format_bc1_unorm_srgb = 72,
        dxgi_format_bc2_typeless = 73,
        dxgi_format_bc2_unorm = 74,
        dxgi_format_bc2_unorm_srgb = 75,
        dxgi_format_bc3_typeless = 76,
        dxgi_format_bc3_unorm = 77,
        dxgi_format_bc3_unorm_srgb = 78,
        dxgi_format_bc4_typeless = 79,
        dxgi_format_bc4_unorm = 80,
        dxgi_format_bc4_snorm = 81,
        dxgi_format_bc5_typeless = 82,
        dxgi_format_bc5_unorm = 83,
        dxgi_format_bc5_snorm = 84,
        dxgi_format_b5g6r5_unorm = 85,
        dxgi_format_b5g5r5a1_unorm = 86,
        dxgi_format_b8g8r8a8_unorm = 87,
        dxgi_format_b8g8r8x8_unorm = 88,
        dxgi_format_r10g10b10_xr_bias_a2_unorm = 89,
        dxgi_format_b8g8r8a8_typeless = 90,
        dxgi_format_b8g8r8a8_unorm_srgb = 91,
        dxgi_format_b8g8r8x8_typeless = 92,
        dxgi_format_b8g8r8x8_unorm_srgb = 93,
        dxgi_format_bc6h_typeless = 94,
        dxgi_format_bc6h_uf16 = 95,
        dxgi_format_bc6h_sf16 = 96,
        dxgi_format_bc7_typeless = 97,
        dxgi_format_bc7_unorm = 98,
        dxgi_format_bc7_unorm_srgb = 99,
        dxgi_format_ayuv = 100,
        dxgi_format_y410 = 101,
        dxgi_format_y416 = 102,
        dxgi_format_nv12 = 103,
        dxgi_format_p010 = 104,
        dxgi_format_p016 = 105,
        dxgi_format_420_opaque = 106,
        dxgi_format_yuy2 = 107,
        dxgi_format_y210 = 108,
        dxgi_format_y216 = 109,
        dxgi_format_nv11 = 110,
        dxgi_format_ai44 = 111,
        dxgi_format_ia44 = 112,
        dxgi_format_p8 = 113,
        dxgi_format_a8p8 = 114,
        dxgi_format_b4g4r4a4_unorm = 115,
        dxgi_format_force_uint = 0XFFFFFFFF
    };

    enum dds_dimension_e
    {
        dds_dimension_unknown = 0,
        dds_dimension_buffer = 1,
        dds_dimension_texture1d = 2,
        dds_dimension_texture2d = 3,
        dds_dimension_texture3d = 4
    };

#pragma pack(push, 1)
    struct dds_color_key_t
    {
        uint32_t low;
        uint32_t high;
    };

    struct dds_pixel_format_t
    {
        uint32_t size;
        uint32_t flags;
        dxt_format_e dxt_format : 32;
        uint32_t bit_count;
        color_mask_abgr_t color_mask;
    };

    struct dds_caps_t
    {
        uint32_t surface_flags;
        uint32_t cubemap_flags;
        uint32_t cap3;
        uint32_t cap4;
    };

    struct dds_header_t
    {
        uint32_t magic;
        uint32_t size;
        uint32_t flags;
        uint32_t height;
        uint32_t width;

        union
        {
            int32_t pitch;
            uint32_t linear_size;
        };

        union
        {
            uint32_t depth;
            uint32_t back_buffer_count;
        };

        uint32_t mipmap_count;

        union
        {
            uint32_t reserved1[11];

            struct
            {
                uint32_t alpha_bit_depth;
                uint32_t reserverd;
                uint32_t lpsurface;

                union
                {
                    dds_color_key_t dst_overlay;
                    uint32_t empty_face_color;
                };

                dds_color_key_t dst_blt;
                dds_color_key_t src_overlay;
                dds_color_key_t srcblt;;
            };
        };

        dds_pixel_format_t pixel_format;
        dds_caps_t caps;
        uint32_t texture_stage;
    };

    struct dds_header_10_t
    {
        dxgi_format_e dxgi_format;
        dds_dimension_e dimension;
        uint32_t misc_flag;
        uint32_t array_size;
        uint32_t reserved;
    };

    struct dds_texel_bc1_t
    {
        uint16_t color0;
        uint16_t color1;
        uint32_t indices;
    };

    struct dds_texel_bc2_t
    {
        uint16_t alphas[4];
        uint16_t color0;
        uint16_t color1;
        uint32_t indices;
    };

    struct dds_texel_bc3_t
    {
        struct
        {
            uint8_t alpha0;
            uint8_t alpha1;

            uint16_t index0;
            uint16_t index1;
            uint16_t index2;
        };

        uint16_t color0;
        uint16_t color1;
        uint32_t indices;
    };

    struct dxt1_info_t
    {
        color_r8g8b8_t colors[4];
    };

    // 用于 dxt 解压
    struct dxt_info_t
    {
        union
        {
            color_r8g8b8_t rgbs[4];
            color_a8r8g8b8_t argbs[4];
        };

        uint8_t alphas[8];
        uint8_t alpha_indices[4][4];
    };

    struct dxt_pixel_a16r16g16b16f_t
    {
        uint16_t r;
        uint16_t g;
        uint16_t b;
        uint16_t a;
    };

    struct dxt_pixel_a32r32g32b32f_t
    {
        float32_t r;
        float32_t g;
        float32_t b;
        float32_t a;
    };

    struct dxt_pixel_g16r16f_t
    {
        uint16_t r;
        uint16_t g;
    };

    struct dxt_pixel_g32r32f_t
    {
        float32_t r;
        float32_t g;
    };
#pragma pack(pop)

    struct dds_image_data_t : public image_data_t { };

    //////////////////////////////////////////////////////////////////////////
    /// dxt 压缩
    /// 

    core::error dds_convert_bc1(image_codec_context & icctx, const image_data_t & src, image_data_t & dst);
    core::error dds_convert_bc2(image_codec_context & icctx, const image_data_t & src, image_data_t & dst);
    core::error dds_convert_bc3(image_codec_context & icctx, const image_data_t & src, image_data_t & dst);

    void color_dds_a4r5g6b5_to_r5g5b5(const void * src_pixel, void * dst_pixel);
    void color_dds_a4r5g6b5_to_r8g8b8(const void * src_pixel, void * dst_pixel);

    void color_dds_a4r5g6b5_to_a8r8g8b8(const void * src_pixel, void * dst_pixel); //dxt2 dxt3
    void color_dds_a8r5g6b5_to_a8r8g8b8(const void * src_pixel, void * dst_pixel); //dxt4 dxt5

    //////////////////////////////////////////////////////////////////////////
    /// dds_header_t.flags 位掩码

    /// caps 有效（默认有此结构）
    const uint32_t DDSF_CAPS = 0x00000001;

    /// height 有效
    const uint32_t DDSF_HEIGHT = 0x00000002;

    /// width 有效
    const uint32_t DDSF_WIDTH = 0x00000004;

    /// pitch 有效
    const uint32_t DDSF_PITCH = 0x00000008;

    /// back_buffer_count 有效
    const uint32_t DDSF_BACKBUFFERCOUNT = 0x00000020;

    /// z_buffer_bit_depth 有效（弃用）
    const uint32_t DDSF_ZBUFFERBITDEPTH = 0x00000040;

    /// alpha_bit_depth 有效
    const uint32_t DDSF_ALPHA_BIT_DEPTH = 0x00000080;

    /// lpsurface 有效
    const uint32_t DDSF_LPSURFACE = 0x00000800;

    /// pixel_format 有效
    const uint32_t DDSF_PIXEL_FORMAT = 0x00001000;

    /// dst_overlay 有效
    const uint32_t DDSF_DST_OVERLAY = 0x00002000;

    /// dst_blt 有效
    const uint32_t DDSF_DST_BLT = 0x00004000;

    /// src_overlay 有效
    const uint32_t DDSF_SRC_OVERLAY = 0x00008000;

    /// src_blt 有效
    const uint32_t DDSF_SRC_BLT = 0x00010000;

    /// mipmap_count 有效
    const uint32_t DDSF_MIPMAP_COUNT = 0x00020000;

    /// refresh_rate 有效
    const uint32_t DDSF_REFRESH_RATE = 0x00040000;

    /// linear_size 有效
    const uint32_t DDSF_LINEAR_SIZE = 0x00080000;

    /// texture_stage 有效
    const uint32_t DDSF_TEXTURE_STAGE = 0x00100000;

    /// fvf 有效
    const uint32_t DDSF_FVF = 0x00200000;

    /// src_vb_handle 有效
    const uint32_t DDSF_SRC_VB_HANDLE = 0x00400000;

    /// depth 有效
    const uint32_t DDSF_VOLUME = 0x00800000;


    //////////////////////////////////////////////////////////////////////////
    /// dds_pixel_format_t.flags 位掩码

    /// 有 Alpha 通道
    const uint32_t DDSPF_ALPHA = 0x00000001;

    /// 只有 Alpha 通道
    const uint32_t DDSPF_ALPHA_ONLY = 0x00000002;

    /// 使用“四字符编码”
    const uint32_t DDSPF_FOURCC = 0x00000004;

    /// 使用 四位颜色索引
    const uint32_t DDSPF_INDEX_4 = 0x00000008;

    /// The surface is indexed into a palette which stores indices into the destination surface's 8-bit palette.
    const uint32_t DDSPF_INDEX_TO_8 = 0x00000010;

    /// 使用 八位颜色索引
    const uint32_t DDSPF_INDEX_8 = 0x00000020;

    /// 使用 RGB 颜色
    const uint32_t DDSPF_RGB = 0x00000040;

    /// 内容是经过压缩的
    const uint32_t DDSPF_COMPRESSED = 0x00000080;

    /*
    * The surface will accept RGB data and translate it during the write to YUV data.
    * The format of the data to be written will be contained in the pixel format structure.
    * The DDPF_RGB flag will be set.
    */
    const uint32_t DDSPF_RGB_TO_YUV = 0x000000100;

    /// 使用 YUV 颜色
    const uint32_t DDSPF_YUV = 0x000000200;

    /// 只拥有 Z-Buffer 信息
    const uint32_t DDSPF_Z_BUFFER = 0x000000400;


    /// 使用 一位颜色索引
    const uint32_t DDSPF_INDEX_1 = 0x000000800;

    /// 使用 二位颜色索引
    const uint32_t DDSPF_INDEX_2 = 0x000001000;

    /// 包含 深度信息
    const uint32_t DDSPF_Z_INFO = 0x000002000;

    /// 包含 与 深度信息相关的模板信息 The surface contains stencil information along with Z
    const uint32_t DDSPF_Z_STENCIL = 0x000004000;

    /// 颜色信息已经与 Alpha 做预乘
    const uint32_t DDSPF_ALPHA_PREMULT = 0x000008000;

    /// 包含 亮度信息
    /// Luminance data in the pixel format is valid.
    /// Use this flag for luminance-only or luminance+alpha surfaces,
    /// the bit depth is then ddpf.dwLuminanceBitCount.
    const uint32_t DDSPF_LUMINANCE = 0x000020000;


    /// Luminance data in the pixel format is valid. Use this flag when hanging luminance off bumpmap surfaces,
    /// the bit mask for the luminance portion of the pixel is then ddpf.dwBumpLuminanceBitMask
    const uint32_t DDSPF_BUMPLUMINANCE = 0x00040000;


    /// Bump map dUdV data in the pixel format is valid.
    const uint32_t DDSPF_BUMPDUDV = 0x00080000;


    //////////////////////////////////////////////////////////////////////////
    /// dds_caps_t.caps1 位掩码
    /// 
    const uint32_t DDS_CAPS_TEXTURE = 0x00080000;

    const uint32_t DDS_CAPS2_CUBE = 0x200;


    //////////////////////////////////////////////////////////////////////////
    /// 不用了的
    void dds_dxt1(dds_header_t * header, byte_t * buffer, dds_image_data_t * dds);
    void dds_dxt2(dds_header_t * header, byte_t * buffer, dds_image_data_t * dds);
    void dds_dxt3(dds_header_t * header, byte_t * buffer, dds_image_data_t * dds);
    void dds_dxt4(dds_header_t * header, byte_t * buffer, dds_image_data_t * dds);
    void dds_dxt5(dds_header_t * header, byte_t * buffer, dds_image_data_t * dds);

    bool is_dds_data(const byte_t * buffer, int32_t length);
    core::error dds_create(image_codec_context & ictx, const byte_t * buffer, int32_t length, image_data_t & image);
}
