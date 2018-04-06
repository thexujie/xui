#pragma once

#include "player/media.h"

enum _D3DFORMAT;
enum DXGI_FORMAT;
namespace win32 { namespace d3d
{
    using namespace common;

#define D3D_MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((uint32_t)(uint8_t)(ch0) | ((uint32_t)(uint8_t)(ch1) << 8) |   \
                ((uint32_t)(uint8_t)(ch2) << 16) | ((uint32_t)(uint8_t)(ch3) << 24 ))

    constexpr GUID DXVADDI_Intel_ModeH264_E = { 0x604F8E68, 0x4951, 0x4C54, 0x88, 0xFE, 0xAB, 0xD2, 0x5C, 0x15, 0xB3, 0xD6 };
    //const GUID DXVA2_ModeVP9_VLD_Profile0 = { 0x463707f8, 0xa1d0, 0x4585, 0x87, 0x6d, 0x83, 0xaa, 0x6d, 0x60, 0xb8, 0x9e };
    constexpr GUID DXVA2_ModeMJPEG_VLD_Intel = { 0x91CD2D6E, 0x897B, 0x4FA1, 0xB0, 0xD7, 0x51, 0xDC, 0x88, 0x01, 0x0E, 0x0A };
    constexpr GUID DXVA2_ModeH264_VLD_NoFGT_AMD = { 0x4245F676, 0x2BBC, 0x4166, 0xA0, 0xBB, 0x54, 0xE7, 0xB8, 0x49, 0xC3, 0x80 };
    constexpr GUID DXVA2_ModeH264_VLD_Multiview_AMD = { 0x9901CCD3, 0xCA12, 0x4B7E, 0x86, 0x7A, 0xE2, 0x22, 0x3D, 0x92, 0x55, 0xC3 };

    inline int32_t align_to(int32_t size, int32_t align)
    {
        return (((size)+(align)-1)&~((align)-1));
    }

    typedef uint32_t deviceid_t;

    enum D3DFormatEx
    {
        D3DFormatExNV12 = D3D_MAKEFOURCC('N', 'V', '1', '2'),
        D3DFormatExYV12 = D3D_MAKEFOURCC('Y', 'V', '1', '2'),
        D3DFormatExYUY2 = D3D_MAKEFOURCC('Y', 'U', 'Y', '2'),
        D3DFormatExP010 = D3D_MAKEFOURCC('P', '0', '1', '0'),
        D3DFormatExIYUV = D3D_MAKEFOURCC('I', 'Y', 'U', 'V'),
        D3DFormatExUYVY = D3D_MAKEFOURCC('U', 'Y', 'V', 'Y'),
        D3DFormatEx411P = D3D_MAKEFOURCC('4', '1', '1', 'P'),
        D3DFormatEx444P = D3D_MAKEFOURCC('4', '4', '4', 'P'),
        D3DFormatExRGBP = D3D_MAKEFOURCC('R', 'G', 'B', 'P'),
        D3DFormatExBGRP = D3D_MAKEFOURCC('B', 'G', 'R', 'P'),
    };

    struct D3DDeviceDesc
    {
        uint32_t adapter = 0;
        uint32_t deviceId = 0;
        u8string application;
        u8string description;
        uint32_t vendorId = 0;
        uint32_t subSysId = 0;
        uint32_t revision = 0;
    };

    AVCodecID MapDXVA2CodecID(const GUID & guid);
    AVPixelFormat D3DFormat2AvPixelFormat(_D3DFORMAT d3dformat);
    AVPixelFormat DXGIFormat2AvPixelFormat(DXGI_FORMAT dxgiformat);
    _D3DFORMAT AvPixelFormat2D3DFormat(AVPixelFormat pixelFormat);
    DXGI_FORMAT AvPixelFormat2DXGIFormat(AVPixelFormat pixelFormat);
    const char * DXGIFormatName(DXGI_FORMAT dxgiformat);

    const GUID GUID_D3DDebugObjectName = { 0x429b8c22, 0x9188, 0x4b0c, 0x87, 0x42, 0xac, 0xb0, 0xbf, 0x85, 0xc2, 0x00 };
    template<typename OT>
    void DxSetDebugName(OT * pObject, std::string name)
    {
        if (pObject)
            pObject->SetPrivateData(GUID_D3DDebugObjectName, (uint32_t)name.length(), name.c_str());
    }
}}
