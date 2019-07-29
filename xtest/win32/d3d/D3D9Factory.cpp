#include "stdafx.h"
#include "D3D9Factory.h"

#include <d3d9.h>

namespace win32 { namespace d3d
{
    D3D9Factory::D3D9Factory() { }

    D3D9Factory::~D3D9Factory() { }

    error_e D3D9Factory::loadModule()
    {
        if(!_d3d9)
        {
            _d3d9.reset(Direct3DCreate9(D3D_SDK_VERSION));
            if(!_d3d9)
                return error_inner;
        }
        return error_ok;
    }

    com_ptr<IDirect3D9> D3D9Factory::GetD3D9() const
    {
        if(const_cast<D3D9Factory *>(this)->loadModule() != 0)
            return nullptr;

        return _d3d9;
    }

    std::vector<D3D9DeviceDesc> D3D9Factory::GetDeviceDescs() const
    {
        std::vector<D3D9DeviceDesc> descs;
        if(const_cast<D3D9Factory *>(this)->loadModule() != 0)
            return descs;

        com_ptr<struct IDirect3D9> d3d9 = const_cast<D3D9Factory *>(this)->_d3d9;

        HRESULT hr = S_OK;
        D3DCAPS9 caps = {};
        uint32_t adapterCount = d3d9->GetAdapterCount();
        for(uint32_t iadapter = 0; iadapter < adapterCount; ++iadapter)
        {
            D3DADAPTER_IDENTIFIER9 adapterIdentifier = {};
            hr = d3d9->GetAdapterIdentifier(iadapter, 0, &adapterIdentifier);
            if(FAILED(hr))
                continue;

            D3DDISPLAYMODE displayMode = {};
            hr = d3d9->GetAdapterDisplayMode(iadapter, &displayMode);
            if(FAILED(hr))
                return {};

            hr = d3d9->GetDeviceCaps(iadapter, D3DDEVTYPE_HAL, &caps);
            if(FAILED(hr))
                continue;

            D3D9DeviceDesc desc = {};

            desc.adapter = iadapter;
            desc.deviceId = adapterIdentifier.DeviceId;
            std::memcpy(&desc.deviceIdentifier, &adapterIdentifier.DeviceIdentifier,
                std::min(std::size(desc.deviceIdentifier), sizeof(GUID)));

            desc.driver = adapterIdentifier.Driver;
            desc.application = "Direct3D 9";
            desc.description = adapterIdentifier.Description;
            desc.displayDeviceName = adapterIdentifier.DeviceName;
            desc.driverVersion = adapterIdentifier.DriverVersion.QuadPart;
            desc.vendorId = adapterIdentifier.VendorId;
            desc.subSysId = adapterIdentifier.SubSysId;
            desc.revision = adapterIdentifier.Revision;
            desc.WHQLLevel = adapterIdentifier.WHQLLevel;

            //// 关联的显示器信息
            //HMONITOR hMonitor = d3d9->GetAdapterMonitor(iadapter);
            //MONITORINFOEXW monInfoEx;
            //monInfoEx.cbSize = sizeof(MONITORINFOEXW);
            //if (GetMonitorInfoW(hMonitor, &monInfoEx))
            //    desc.displayDeviceName = monInfoEx.szDevice;

            desc.displayWidth = displayMode.Width;
            desc.displayHeight = displayMode.Height;

            descs.emplace_back(desc);
        }
        return descs;
    }
}}
