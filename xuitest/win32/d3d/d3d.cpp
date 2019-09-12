#include "stdafx.h"
#include "d3d.h"

#include "win32/windows.h"
#include <d3d9types.h>
#include <dxgi.h>
#include <libavcodec/avcodec.h>
#include <initguid.h>
#include <dxva2api.h>

namespace win32
{
    namespace d3d
    {
        AVCodecID MapDXVA2CodecID(const GUID & guid)
        {
            static struct GUID2AVCodecID
            {
                const GUID * guid;
                enum AVCodecID codecId;
            }
            MAPS[] =
            {
                /* MPEG-2 */
            { &DXVA2_ModeMPEG1_VLD, AV_CODEC_ID_MPEG2VIDEO },
            { &DXVA2_ModeMPEG2_VLD, AV_CODEC_ID_MPEG2VIDEO },
            { &DXVA2_ModeMPEG2and1_VLD, AV_CODEC_ID_MPEG2VIDEO },

            /* H.264 */
            { &DXVA2_ModeH264_A, AV_CODEC_ID_H264 },
            { &DXVA2_ModeH264_B, AV_CODEC_ID_H264 },
            { &DXVA2_ModeH264_C, AV_CODEC_ID_H264 },
            { &DXVA2_ModeH264_D, AV_CODEC_ID_H264 },
            { &DXVA2_ModeH264_E, AV_CODEC_ID_H264 },
            { &DXVA2_ModeH264_F, AV_CODEC_ID_H264 },
            { &DXVA2_ModeH264_VLD_WithFMOASO_NoFGT, AV_CODEC_ID_H264 },
            { &DXVA2_ModeH264_VLD_NoFGT_AMD, AV_CODEC_ID_H264 },
            { &DXVA2_ModeH264_VLD_Stereo_Progressive_NoFGT, AV_CODEC_ID_H264 },
            { &DXVA2_ModeH264_VLD_Stereo_NoFGT, AV_CODEC_ID_H264 },
            { &DXVA2_ModeH264_VLD_Multiview_NoFGT, AV_CODEC_ID_H264 },
            { &DXVA2_ModeH264_VLD_Multiview_AMD, AV_CODEC_ID_H264 },
            /* Intel specific H.264 mode */
            { &DXVADDI_Intel_ModeH264_E, AV_CODEC_ID_H264 },

            /* VC-1 / WMV3 */
            { &DXVA2_ModeVC1_D2010, AV_CODEC_ID_VC1 },
            { &DXVA2_ModeVC1_D2010, AV_CODEC_ID_WMV3 },
            { &DXVA2_ModeVC1_D, AV_CODEC_ID_VC1 },
            { &DXVA2_ModeVC1_D, AV_CODEC_ID_WMV3 },

            /* HEVC/H.265 */
            { &DXVA2_ModeHEVC_VLD_Main, AV_CODEC_ID_HEVC },
            { &DXVA2_ModeHEVC_VLD_Main10, AV_CODEC_ID_HEVC },

            /* VP8/9 */
            { &DXVA2_ModeVP9_VLD_Profile0, AV_CODEC_ID_VP9 },

            /* MJPEG */
            { &DXVA2_ModeMJPEG_VLD_Intel, AV_CODEC_ID_MJPEG },
            };

            for (const GUID2AVCodecID & item : MAPS)
            {
                if (*item.guid == guid)
                    return item.codecId;
            }

            return AV_CODEC_ID_NONE;
        }

        AVPixelFormat D3DFormat2AvPixelFormat(_D3DFORMAT d3dformat)
        {
            switch (d3dformat)
            {
            case D3DFMT_X8R8G8B8: return AV_PIX_FMT_0RGB;
            case D3DFMT_A8R8G8B8: return AV_PIX_FMT_ARGB;
            case D3DFormatExNV12: return AV_PIX_FMT_NV12;
            case D3DFormatExYV12: return AV_PIX_FMT_YUV420P;
            case D3DFormatExYUY2: return AV_PIX_FMT_YUYV422;
            case D3DFormatExUYVY: return AV_PIX_FMT_UYVY422;
            case D3DFormatEx411P: return AV_PIX_FMT_YUV411P;
            case D3DFormatEx444P: return AV_PIX_FMT_YUVA444P;
            case D3DFormatExP010: return AV_PIX_FMT_YUV420P10;
                //case D3DFormatExRGBP: return AV_PIX_FMT_RGB24;
                //case D3DFormatExBGRP: return AV_PIX_FMT_BGR24;
            default: return AV_PIX_FMT_NONE;
            }
        }

        _D3DFORMAT AvPixelFormat2D3DFormat(AVPixelFormat pixelFormat)
        {
            switch (pixelFormat)
            {
            case AV_PIX_FMT_0RGB: return D3DFMT_X8R8G8B8;
            case AV_PIX_FMT_ARGB: return D3DFMT_A8R8G8B8;
            case AV_PIX_FMT_NV12: return (D3DFORMAT)D3DFormatExNV12;
            case AV_PIX_FMT_YUV420P: return (D3DFORMAT)D3DFormatExYV12;
            case AV_PIX_FMT_YUV420P10: return (D3DFORMAT)D3DFormatExP010;
            default: return D3DFMT_UNKNOWN;
            }
        }

        AVPixelFormat DXGIFormat2AvPixelFormat(DXGI_FORMAT dxgiformat)
        {
            switch (dxgiformat)
            {
            case DXGI_FORMAT_B8G8R8X8_UNORM: return AV_PIX_FMT_0RGB;
            case DXGI_FORMAT_B8G8R8A8_UNORM: return AV_PIX_FMT_ARGB;
            case DXGI_FORMAT_NV12: return AV_PIX_FMT_NV12;
                //case D3DFormatExYV12: return AV_PIX_FMT_YUV420P;
            case DXGI_FORMAT_YUY2: return AV_PIX_FMT_YUYV422;
                //case D3DFormatExUYVY: return AV_PIX_FMT_UYVY422;
                //case D3DFormatEx411P: return AV_PIX_FMT_YUV411P;
                //case D3DFormatEx444P: return AV_PIX_FMT_YUVA444P;
            case DXGI_FORMAT_P010: return AV_PIX_FMT_YUV420P10;
                //case D3DFormatExRGBP: return AV_PIX_FMT_RGB24;
                //case D3DFormatExBGRP: return AV_PIX_FMT_BGR24;
            case DXGI_FORMAT_420_OPAQUE: return AV_PIX_FMT_YUV420P;
            default: return AV_PIX_FMT_NONE;
            }
        }

        DXGI_FORMAT AvPixelFormat2DXGIFormat(AVPixelFormat pixelFormat)
        {
            switch (pixelFormat)
            {
            case AV_PIX_FMT_0RGB: return DXGI_FORMAT_B8G8R8X8_UNORM;
            case AV_PIX_FMT_ARGB: return DXGI_FORMAT_B8G8R8A8_UNORM;
            case AV_PIX_FMT_NV12: return DXGI_FORMAT_NV12;
            case AV_PIX_FMT_YUV420P: return DXGI_FORMAT_420_OPAQUE;
            case AV_PIX_FMT_YUV420P10: return DXGI_FORMAT_P010;
            default: return DXGI_FORMAT_UNKNOWN;
            }
        }

#define DXGI_FORMAT_RETURN_NAME(fmt) case fmt: return #fmt

        const char * DXGIFormatName(DXGI_FORMAT dxgiformat)
        {
            switch(dxgiformat)
            {
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_UNKNOWN);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32G32B32A32_TYPELESS);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32G32B32A32_FLOAT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32G32B32A32_UINT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32G32B32A32_SINT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32G32B32_TYPELESS);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32G32B32_FLOAT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32G32B32_UINT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32G32B32_SINT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16G16B16A16_TYPELESS);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16G16B16A16_FLOAT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16G16B16A16_UNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16G16B16A16_UINT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16G16B16A16_SNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16G16B16A16_SINT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32G32_TYPELESS);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32G32_FLOAT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32G32_UINT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32G32_SINT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32G8X24_TYPELESS);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_D32_FLOAT_S8X24_UINT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_X32_TYPELESS_G8X24_UINT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R10G10B10A2_TYPELESS);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R10G10B10A2_UNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R10G10B10A2_UINT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R11G11B10_FLOAT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8G8B8A8_TYPELESS);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8G8B8A8_UNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8G8B8A8_UINT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8G8B8A8_SNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8G8B8A8_SINT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16G16_TYPELESS);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16G16_FLOAT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16G16_UNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16G16_UINT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16G16_SNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16G16_SINT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32_TYPELESS);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_D32_FLOAT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32_FLOAT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32_UINT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R32_SINT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R24G8_TYPELESS);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_D24_UNORM_S8_UINT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R24_UNORM_X8_TYPELESS);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_X24_TYPELESS_G8_UINT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8G8_TYPELESS);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8G8_UNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8G8_UINT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8G8_SNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8G8_SINT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16_TYPELESS);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16_FLOAT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_D16_UNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16_UNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16_UINT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16_SNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R16_SINT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8_TYPELESS);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8_UNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8_UINT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8_SNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8_SINT);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_A8_UNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R1_UNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R9G9B9E5_SHAREDEXP);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R8G8_B8G8_UNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_G8R8_G8B8_UNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC1_TYPELESS);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC1_UNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC1_UNORM_SRGB);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC2_TYPELESS);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC2_UNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC2_UNORM_SRGB);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC3_TYPELESS);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC3_UNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC3_UNORM_SRGB);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC4_TYPELESS);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC4_UNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC4_SNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC5_TYPELESS);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC5_UNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC5_SNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_B5G6R5_UNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_B5G5R5A1_UNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_B8G8R8A8_UNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_B8G8R8X8_UNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_B8G8R8A8_TYPELESS);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_B8G8R8A8_UNORM_SRGB);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_B8G8R8X8_TYPELESS);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_B8G8R8X8_UNORM_SRGB);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC6H_TYPELESS);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC6H_UF16);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC6H_SF16);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC7_TYPELESS);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC7_UNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_BC7_UNORM_SRGB);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_AYUV);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_Y410);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_Y416);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_NV12);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_P010);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_P016);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_420_OPAQUE);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_YUY2);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_Y210);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_Y216);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_NV11);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_AI44);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_IA44);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_P8);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_A8P8);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_B4G4R4A4_UNORM);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_P208);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_V208);
                DXGI_FORMAT_RETURN_NAME(DXGI_FORMAT_V408);
                default: return "Invalid Format";
            }
            
        }
    }
}
