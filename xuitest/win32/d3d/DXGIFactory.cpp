#include "stdafx.h"
#include "DXGIFactory.h"

#include "windows.h"
#include <dxgi.h>
#include <d3d11.h>
#include "win32/windows.h"


namespace win32 {
    namespace d3d {

        DXGIFactory::DXGIFactory()
        {
            
        }

        DXGIFactory::~DXGIFactory()
        {
            
        }

        common::error_e DXGIFactory::load()
        {
            if (_dxgi)
                return error_ok;

            error_e err = _dll.load("dxgi.dll");
            if (err < 0)
                return err;

            auto pfnCreateDXGIFactory = _dll.get<decltype(CreateDXGIFactory) *>("CreateDXGIFactory");
            if(!pfnCreateDXGIFactory)
            {
                logger::err() << __FUNCTION__" canot find CreateDXGIFactory in dxgi.dll " << winerr_str(GetLastError());
                return error_inner;
            }

            HRESULT hr  = pfnCreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&_dxgi));
            if (FAILED(hr))
                return error_inner;
            
            return error_ok;
        }


        com_ptr<IDXGIFactory> DXGIFactory::GetPointer() const
        {
            return _dxgi;
        }

        std::vector<D3DDeviceDesc> DXGIFactory::GetDeviceDescs() const
        {
            if (!_dxgi &&  const_cast<DXGIFactory *>(this)->load() < 0)
                return {};

            std::vector<D3DDeviceDesc> descs;

            HRESULT hr = S_OK;
            uint32_t iadapter = 0;
            while (true)
            {
                com_ptr<IDXGIAdapter> adapter;
                hr = _dxgi->EnumAdapters(iadapter++, &adapter);
                if (FAILED(hr))
                    break;

                DXGI_ADAPTER_DESC dxgiDesc = {};
                hr = adapter->GetDesc(&dxgiDesc);
                if (FAILED(hr))
                    break;

                D3DDeviceDesc desc = {};

                desc.adapter = iadapter;
                desc.deviceId = dxgiDesc.DeviceId;
                desc.application = "Direct3D 11";
                desc.description = tools::string::ucs2_u8(dxgiDesc.Description);
                desc.vendorId = dxgiDesc.VendorId;
                desc.subSysId = dxgiDesc.SubSysId;
                desc.revision = dxgiDesc.Revision;

                //// 关联的显示器信息
                //HMONITOR hMonitor = d3d9->GetAdapterMonitor(iadapter);
                //MONITORINFOEXW monInfoEx;
                //monInfoEx.cbSize = sizeof(MONITORINFOEXW);
                //if (GetMonitorInfoW(hMonitor, &monInfoEx))
                //    desc.displayDeviceName = monInfoEx.szDevice;

                descs.emplace_back(desc);
            }
            return descs;
        }

        com_ptr<IDXGIAdapter> DXGIFactory::GetAdapter(deviceid_t deviceId)
        {
            if (!_dxgi &&  const_cast<DXGIFactory *>(this)->load() < 0)
                return nullptr;

            HRESULT hr = S_OK;
            uint32_t iadapter = 0;
            while (true)
            {
                com_ptr<IDXGIAdapter> adapter;
                hr = _dxgi->EnumAdapters(iadapter++, &adapter);
                if (FAILED(hr))
                    break;

                DXGI_ADAPTER_DESC dxgiDesc = {};
                hr = adapter->GetDesc(&dxgiDesc);
                if (FAILED(hr))
                    continue;

                if (!deviceId || dxgiDesc.DeviceId == deviceId)
                    return adapter;
            }
            return nullptr;
        }
    }
}
