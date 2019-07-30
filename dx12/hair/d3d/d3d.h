#pragma once
#include "win32/win32.h"

#include <d3d9types.h>
#include <dxgi.h>

namespace win32::d3d
{
#define D3D_MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((uint32_t)(uint8_t)(ch0) | ((uint32_t)(uint8_t)(ch1) << 8) |   \
                ((uint32_t)(uint8_t)(ch2) << 16) | ((uint32_t)(uint8_t)(ch3) << 24 ))

    inline int32_t align_to(int32_t size, int32_t align)
    {
        return (((size)+(align)-1)&~((align)-1));
    }

    typedef uint32_t deviceid_t;

    enum D3DFormatEx
    {
        D3DFormatExNV12 = D3D_MAKEFOURCC('N', 'V', '1', '2'),
        D3DFormatExYV12 = D3D_MAKEFOURCC('Y', 'V', '1', '2'),
        D3DFormatExP010 = D3D_MAKEFOURCC('P', '0', '1', '0'),
        D3DFormatExIYUV = D3D_MAKEFOURCC('I', 'Y', 'U', 'V'),
        D3DFormatEx411P = D3D_MAKEFOURCC('4', '1', '1', 'P'),
        D3DFormatEx444P = D3D_MAKEFOURCC('4', '4', '4', 'P'),
        D3DFormatExRGBP = D3D_MAKEFOURCC('R', 'G', 'B', 'P'),
        D3DFormatExBGRP = D3D_MAKEFOURCC('B', 'G', 'R', 'P'),
    };

    struct D3DDeviceDesc
    {
        uint32_t adapter = 0;
        uint32_t deviceId = 0;
        std::u8string application;
		std::u8string description;
        uint32_t vendorId = 0;
        uint32_t subSysId = 0;
        uint32_t revision = 0;
    };

    core::pixelformat D3DFormat2PixelFormat(D3DFORMAT d3dformat);
    D3DFORMAT PixelFormat2D3DFormat(core::pixelformat pixelformat);
    const char * D3DFormatName(_D3DFORMAT d3dformat);

    const guid GUID_D3DDebugObjectName = { 0x429b8c22, 0x9188, 0x4b0c, 0x87, 0x42, 0xac, 0xb0, 0xbf, 0x85, 0xc2, 0x00 };
    template<typename OT>
    void DxSetDebugName(OT * pObject, std::string name)
    {
        if (pObject)
            pObject->SetPrivateData(GUID_D3DDebugObjectName, (uint32_t)name.length(), name.c_str());
    }
}
