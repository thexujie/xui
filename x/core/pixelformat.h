#pragma once

namespace core
{
    inline constexpr unsigned int format_fourcc(const char (&fourcc)[5])
    {
        return (static_cast<unsigned int>(fourcc[0])) |
            (static_cast<unsigned int>(fourcc[1]) << 8) |
            (static_cast<unsigned int>(fourcc[2]) << 16) |
            (static_cast<unsigned int>(fourcc[3]) << 24);
    }

    inline constexpr unsigned int format_fourcc(char a, char b, char c, char d)
    {
        return (static_cast<unsigned int>(a)) |
            (static_cast<unsigned int>(b) << 8) |
            (static_cast<unsigned int>(c) << 16) |
            (static_cast<unsigned int>(d) << 24);
    }

    enum class format
    {
        none = 0,
        y8 = format_fourcc("y8  "),
        rgb = format_fourcc("24RG"),
        bgr = format_fourcc("24BG"),
        argb = format_fourcc("ARGB"),
        abgr = format_fourcc("ABGR"),
        bgra = format_fourcc("BGRA"),
        rgba = format_fourcc("RGBA"),
        xrgb = format_fourcc("XRGB"),
        xbgr = format_fourcc("XBGR"),
        bgrx = format_fourcc("BGRX"),
        rgbx = format_fourcc("RGBX"),

        i420 = format_fourcc("I420"),
        i422 = format_fourcc("I422"),
        i444 = format_fourcc("I444"),
        nv12 = format_fourcc("NV12"),
        nv21 = format_fourcc("NV21"),

        yu12 = format_fourcc("YU12"),
        yuv2 = format_fourcc("YUV2"),
        yuy2 = format_fourcc("YUY2"),
        yuyv = format_fourcc("YUYV"),
        yv12 = format_fourcc("YV12"),
        yvyu = format_fourcc("YVYU"),
        uyuv = format_fourcc("UYUV"),
        uyvy = format_fourcc("UYVY"),
        vyvu = format_fourcc("VYVU"),
        vuvy = format_fourcc("VUVY"),

		cmyk = format_fourcc("CMYK"),
		ycck = format_fourcc("YCCK"),

        png = format_fourcc("PNG "),
        jpeg = format_fourcc("JPEG"),
        mjpg = format_fourcc("MJPG"),
        h264 = format_fourcc("H264"),
        h265 = format_fourcc("H265"),
        hevc = format_fourcc("HEVC"),
        x264 = format_fourcc("x264"),
        y420 = format_fourcc("y420"),

		p010 = format_fourcc("p010"),

    	// packaged
		half1 = format_fourcc("00h1"),
		half2 = format_fourcc("00h2"),
		half3 = format_fourcc("00h3"),
		half4 = format_fourcc("00h4"),
		float1 = format_fourcc("00f1"),
		float2 = format_fourcc("00f2"),
		float3 = format_fourcc("00f3"),
		float4 = format_fourcc("00f4"),
		int1 = format_fourcc("00i1"),
		int2 = format_fourcc("00i2"),
		int3 = format_fourcc("00i3"),
		int4 = format_fourcc("00i4"),
		uint1 = format_fourcc("0ui1"),
		uint2 = format_fourcc("0ui2"),
		uint3 = format_fourcc("0ui3"),
		uint4 = format_fourcc("0ui4"),
    	
        raw = format_fourcc("raw "),
        fourcc_i420 = format_fourcc("I420"),
        fourcc_i422 = format_fourcc("I422"),
        fourcc_i444 = format_fourcc("I444"),
        fourcc_i400 = format_fourcc("I400"),
        fourcc_nv21 = format_fourcc("NV21"),
        fourcc_nv12 = format_fourcc("NV12"),
        fourcc_yuy2 = format_fourcc("YUY2"),
        fourcc_uyvy = format_fourcc("UYVY"),
        fourcc_m420 = format_fourcc("M420"),
        fourcc_argb = format_fourcc("ARGB"),
        fourcc_abgr = format_fourcc("ABGR"),
        fourcc_bgra = format_fourcc("BGRA"),
        fourcc_rgba = format_fourcc("RGBA"),
        fourcc_ar30 = format_fourcc("AR30"), // 10 bit per channel. 2101010.
        fourcc_ab30 = format_fourcc("AB30"), // ABGR version of 10 bit
        fourcc_rgbp = format_fourcc("RGBP"), // rgb565 LE.
        fourcc_rgb0 = format_fourcc("RGB0"), // argb1555 LE.
        fourcc_r444 = format_fourcc("R444"), // argb4444 LE.
        fourcc_mjpg = format_fourcc("MJPG"),
        fourcc_yv12 = format_fourcc("YV12"),
        fourcc_yv16 = format_fourcc("YV16"),
        fourcc_yv24 = format_fourcc("YV24"),
        fourcc_yu12 = format_fourcc("YU12"),
        fourcc_yu16 = format_fourcc("YU16"), // Alias for I422.
        fourcc_yu24 = format_fourcc("YU24"), // Alias for I444.
        fourcc_j420 = format_fourcc("J420"),
        fourcc_j400 = format_fourcc("J400"),
        fourcc_h420 = format_fourcc("H420"),
        fourcc_h422 = format_fourcc("H422"),
        fourcc_iyuv = format_fourcc("IYUV"), // Alias for I420.
        fourcc_yuyv = format_fourcc("YUYV"), // Alias for YUY2.
        fourcc_yuvs = format_fourcc("YUVS"), // Alias for YUY2 on Mac.
        fourcc_hdyc = format_fourcc("HDYV"), // Alias for UYVY.
        fourcc_jpeg = format_fourcc("JPEG"), // Alias for JPEG.
        fourcc_dmb1 = format_fourcc("DMB1"), // Alias for MJPG on Mac.
        fourcc_ba81 = format_fourcc("BA81"), // Alias for BGGR.
        fourcc_bgb3 = format_fourcc("BGB3"), // Alias for RAW.
        fourcc_bgr3 = format_fourcc("BGR3"), // Alias for 24BG.
        fourcc_cm32 = format_fourcc(0, 0, 0, 32), // Alias for BGRA kCMPixelFormat_32ARGB
        fourcc_cm24 = format_fourcc(0, 0, 0, 24), // Alias for RAW kCMPixelFormat_24RGB
        fourcc_l555 = format_fourcc("L555"), // Alias for RGBO.
        fourcc_l565 = format_fourcc("L565"), // Alias for RGBP.
        fourcc_l551 = format_fourcc("L551"), // Alias for RGB0.

		gray1 = format_fourcc(0xff, 0xff, 0, 0),
		gray2,
		gray4,
		gray8,

		r3g3b2,

		//---------------------------------16 bits
		r8g8,
		r5g6b5,
		b5g6r5,
		a1r5g5b5,
		a1b5g5r5,
		x1r5g5b5,
		x1b5g5r5,
		a4r4g4b4,
		x4r4g4b4,
		a8r3g3b2,

		//---------------------------------24 bits
		r8g8b8 = rgb,
		b8g8r8 = bgr,
		a8r5g6b5,
		a8b5g6r5,
		a8x1r5g5b5,
		a8x1b5g5r5,

		//---------------------------------32 bits
		a8r8g8b8 = argb,
		x8r8g8b8 = xrgb,
		a8b8g8r8 = abgr,
		x8b8g8r8 = xbgr,
		r8g8b8a8 = rgba,
		r8g8b8x8 = rgbx,
		b8g8r8a8 = bgra,
		b8g8r8x8 = bgrx,

		g16r16,

		a2r10g10b10,
		a2b10g10r10,

		//---------------------------------
		a16b16g16r16,

		r16f,
		g16r16f,
		a16b16g16r16f,
		r32f,
		g32r32f,
		b32g32r32f,
		a32b32g32r32f,

		x32y32z32f,

		a4r5g6b5,
    };
}
