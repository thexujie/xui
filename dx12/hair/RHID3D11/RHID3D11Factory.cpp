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
			adapter.reset();

			RHIAdapterDesc rhidesc;
			rhidesc.uri = core::fmt("DXGI\\DEVICEID_", dxgidesc.DeviceId);
			rhidesc.name = core::wstr_u8str(dxgidesc.Description);
			rhidescs.emplace_back(rhidesc);
		}
		return rhidescs;
	}

	std::shared_ptr<RHIDevice> RHID3D11Factory::CreateDevice(const std::u8string & uri) const
	{
		if (!_dxgi)
			return nullptr;

		std::vector<RHIAdapterDesc> rhidescs;
		UINT adapterIndex = 0;
		core::comptr<IDXGIAdapter1> adapter;

		UINT deviceId = std::atol(reinterpret_cast<const char *>(uri.data()) + 14);
		if (!deviceId)
			return nullptr;

		while (_dxgi->EnumAdapters1(adapterIndex++, adapter.getpp()) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC1 dxgidesc = {};
			adapter->GetDesc1(&dxgidesc);
			if (dxgidesc.DeviceId == deviceId)
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