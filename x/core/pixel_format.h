#pragma once

namespace core
{
    inline constexpr unsigned int pixelformat_fourcc(const char (&fourcc)[5])
    {
        return (static_cast<unsigned int>(fourcc[0])) |
            (static_cast<unsigned int>(fourcc[1]) << 8) |
            (static_cast<unsigned int>(fourcc[2]) << 16) |
            (static_cast<unsigned int>(fourcc[3]) << 24);
    }

    inline constexpr unsigned int pixelformat_fourcc(char a, char b, char c, char d)
    {
        return (static_cast<unsigned int>(a)) |
            (static_cast<unsigned int>(b) << 8) |
            (static_cast<unsigned int>(c) << 16) |
            (static_cast<unsigned int>(d) << 24);
    }

    enum class pixelformat
    {
        none = 0,
        y8 = pixelformat_fourcc("y8  "),
        rgb = pixelformat_fourcc("24RG"),
        bgr = pixelformat_fourcc("24BG"),
        argb = pixelformat_fourcc("ARGB"),
        abgr = pixelformat_fourcc("ABGR"),
        bgra = pixelformat_fourcc("BGRA"),
        rgba = pixelformat_fourcc("RGBA"),
        xrgb = pixelformat_fourcc("XRGB"),
        xbgr = pixelformat_fourcc("XBGR"),
        bgrx = pixelformat_fourcc("BGRX"),
        rgbx = pixelformat_fourcc("RGBX"),

        i420 = pixelformat_fourcc("I420"),
        i422 = pixelformat_fourcc("I422"),
        i444 = pixelformat_fourcc("I444"),
        nv12 = pixelformat_fourcc("NV12"),
        nv21 = pixelformat_fourcc("NV21"),

        yu12 = pixelformat_fourcc("YU12"),
        yuv2 = pixelformat_fourcc("YUV2"),
        yuy2 = pixelformat_fourcc("YUY2"),
        yuyv = pixelformat_fourcc("YUYV"),
        yv12 = pixelformat_fourcc("YV12"),
        yvyu = pixelformat_fourcc("YVYU"),
        uyuv = pixelformat_fourcc("UYUV"),
        uyvy = pixelformat_fourcc("UYVY"),
        vyvu = pixelformat_fourcc("VYVU"),
        vuvy = pixelformat_fourcc("VUVY"),

        png = pixelformat_fourcc("PNG "),
        jpeg = pixelformat_fourcc("JPEG"),
        mjpg = pixelformat_fourcc("MJPG"),
        h264 = pixelformat_fourcc("H264"),
        h265 = pixelformat_fourcc("H265"),
        hevc = pixelformat_fourcc("HEVC"),
        x264 = pixelformat_fourcc("x264"),
        y420 = pixelformat_fourcc("y420"),

		p010 = pixelformat_fourcc("P010"),

        raw = pixelformat_fourcc("raw "),
        fourcc_i420 = pixelformat_fourcc("I420"),
        fourcc_i422 = pixelformat_fourcc("I422"),
        fourcc_i444 = pixelformat_fourcc("I444"),
        fourcc_i400 = pixelformat_fourcc("I400"),
        fourcc_nv21 = pixelformat_fourcc("NV21"),
        fourcc_nv12 = pixelformat_fourcc("NV12"),
        fourcc_yuy2 = pixelformat_fourcc("YUY2"),
        fourcc_uyvy = pixelformat_fourcc("UYVY"),
        fourcc_m420 = pixelformat_fourcc("M420"),
        fourcc_argb = pixelformat_fourcc("ARGB"),
        fourcc_abgr = pixelformat_fourcc("ABGR"),
        fourcc_bgra = pixelformat_fourcc("BGRA"),
        fourcc_rgba = pixelformat_fourcc("RGBA"),
        fourcc_ar30 = pixelformat_fourcc("AR30"), // 10 bit per channel. 2101010.
        fourcc_ab30 = pixelformat_fourcc("AB30"), // ABGR version of 10 bit
        fourcc_rgbp = pixelformat_fourcc("RGBP"), // rgb565 LE.
        fourcc_rgb0 = pixelformat_fourcc("RGB0"), // argb1555 LE.
        fourcc_r444 = pixelformat_fourcc("R444"), // argb4444 LE.
        fourcc_mjpg = pixelformat_fourcc("MJPG"),
        fourcc_yv12 = pixelformat_fourcc("YV12"),
        fourcc_yv16 = pixelformat_fourcc("YV16"),
        fourcc_yv24 = pixelformat_fourcc("YV24"),
        fourcc_yu12 = pixelformat_fourcc("YU12"),
        fourcc_yu16 = pixelformat_fourcc("YU16"), // Alias for I422.
        fourcc_yu24 = pixelformat_fourcc("YU24"), // Alias for I444.
        fourcc_j420 = pixelformat_fourcc("J420"),
        fourcc_j400 = pixelformat_fourcc("J400"),
        fourcc_h420 = pixelformat_fourcc("H420"),
        fourcc_h422 = pixelformat_fourcc("H422"),
        fourcc_iyuv = pixelformat_fourcc("IYUV"), // Alias for I420.
        fourcc_yuyv = pixelformat_fourcc("YUYV"), // Alias for YUY2.
        fourcc_yuvs = pixelformat_fourcc("YUVS"), // Alias for YUY2 on Mac.
        fourcc_hdyc = pixelformat_fourcc("HDYV"), // Alias for UYVY.
        fourcc_jpeg = pixelformat_fourcc("JPEG"), // Alias for JPEG.
        fourcc_dmb1 = pixelformat_fourcc("DMB1"), // Alias for MJPG on Mac.
        fourcc_ba81 = pixelformat_fourcc("BA81"), // Alias for BGGR.
        fourcc_bgb3 = pixelformat_fourcc("BGB3"), // Alias for RAW.
        fourcc_bgr3 = pixelformat_fourcc("BGR3"), // Alias for 24BG.
        fourcc_cm32 = pixelformat_fourcc(0, 0, 0, 32), // Alias for BGRA kCMPixelFormat_32ARGB
        fourcc_cm24 = pixelformat_fourcc(0, 0, 0, 24), // Alias for RAW kCMPixelFormat_24RGB
        fourcc_l555 = pixelformat_fourcc("L555"), // Alias for RGBO.
        fourcc_l565 = pixelformat_fourcc("L565"), // Alias for RGBP.
        fourcc_l551 = pixelformat_fourcc("L551"), // Alias for RGB0.
    };
}
