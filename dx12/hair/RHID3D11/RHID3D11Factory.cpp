#include "stdafx.h"
#include "RHID3D11Factory.h"
#include "RHID3D11Device.h"

namespace RHI::RHID3D11
{
	core::error RHID3D11Factory::Load()
	{
		if (_dxgi)
			return core::ok;

		core::error err = _dll.load(u8"dxgi.dll");
		if (err < 0)
		{
			core::err() << __FUNCTION__" load dxgi.dll failed, " << win32::winerr_str(GetLastError());
			return err;
		}

		HRESULT hr = S_OK;

		auto pfnCreateDXGIFactory = _dll.get<decltype(CreateDXGIFactory) *>("CreateDXGIFactory");
		if (!pfnCreateDXGIFactory)
		{
			core::err() << __FUNCTION__" canot find CreateDXGIFactory in dxgi.dll " << win32::winerr_str(GetLastError());
			return core::e_not_found;
		}

		hr = pfnCreateDXGIFactory(__uuidof(IDXGIFactory3), _dxgi.getvv());
		if (FAILED(hr))
		{
			core::err() << __FUNCTION__" CreateDXGIFactory<IDXGIFactory3> faild: " << win32::winerr_str(GetLastError());
			return core::e_not_found;
		}

		return core::ok;
	}

	std::vector<RHIAdapterDesc> RHID3D11Factory::AdapterDescs() const
	{
		if (!_dxgi)
			return {};

		std::vector<RHIAdapterDesc> rhidescs;
		UINT adapterIndex = 0;
		core::comptr<IDXGIAdapter1> adapter;
		while(_dxgi->EnumAdapters1(adapterIndex++, adapter.getpp()) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC1 dxgidesc = {};
			adapter->GetDesc1(&dxgidesc);

			D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0 };
			D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_11_0;
			core::comptr<ID3D11Device> device;
			HRESULT hr = D3D11CreateDevice(adapter.get(), D3D_DRIVER_TYPE_UNKNOWN, NULL, 0, levels, 1, D3D11_SDK_VERSION, device.getpp(), &level, NULL);
			adapter.reset();
			if (FAILED(hr))
			{
				core::war() << __FUNCTION__ << " D3D11CreateDevice failed.";
				continue;
			}

			D3D11_FEATURE_DATA_ARCHITECTURE_INFO architecture = {};
			hr = device->CheckFeatureSupport(D3D11_FEATURE_ARCHITECTURE_INFO, &architecture, sizeof(architecture));
			if (FAILED(hr))
			{
				core::war() << __FUNCTION__ << " CheckFeatureSupport failed.";
				continue;
			}
			
			RHIAdapterDesc rhidesc;
			rhidesc.id = dxgidesc.DeviceId;
			rhidesc.name = core::wstr_u8str(dxgidesc.Description);
			rhidesc.flags.set(RHIAdapterFlag::TileBasedRender, architecture.TileBasedDeferredRenderer);
			rhidesc.flags.set(RHIAdapterFlag::Software, dxgidesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE);
			rhidescs.emplace_back(rhidesc);
		}
		return rhidescs;
	}

	std::shared_ptr<RHIDevice> RHID3D11Factory::CreateDevice(uint64_t id) const
	{
		if (!_dxgi)
			return nullptr;

		std::vector<RHIAdapterDesc> rhidescs;
		UINT adapterIndex = 0;
		core::comptr<IDXGIAdapter1> adapter;

		while (_dxgi->EnumAdapters1(adapterIndex++, adapter.getpp()) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC1 dxgidesc = {};
			adapter->GetDesc1(&dxgidesc);
			if (dxgidesc.DeviceId == id)
				break;

			adapter.reset();
		}

		if (!adapter)
			return nullptr;

		auto device = std::make_shared<RHID3D11Device>();
		auto err = device->Create(adapter);
		if (err)
			return nullptr;
		return device;
	}
}