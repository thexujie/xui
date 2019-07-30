#include "stdafx.h"
#include "RHID3D12.h"
#include "RHID3D12/RHID3D12Device.h"

namespace RHI::RHID3D12
{
	core::error RHID3D12::Load()
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
#ifdef _DEBUG
		win32::comptr<ID3D12Debug> debugInterface;
		hr = D3D12GetDebugInterface(IID_PPV_ARGS(debugInterface.getpp()));
		if (SUCCEEDED(hr))
			debugInterface->EnableDebugLayer();
#endif

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

	std::vector<RHIAdapterDesc> RHID3D12::AdapterDescs() const
	{
		if (!_dxgi)
			return {};

		std::vector<RHIAdapterDesc> rhidescs;
		UINT adapterIndex = 0;
		win32::comptr<IDXGIAdapter1> adapter;
		while(_dxgi->EnumAdapters1(adapterIndex++, adapter.getpp()) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC1 dxgidesc = {};
			adapter->GetDesc1(&dxgidesc);
			adapter.reset();

			RHIAdapterDesc rhidesc;
			rhidesc.uri = core::format("DXGI\\DEVICEID_", dxgidesc.DeviceId);
			rhidesc.name = core::wstr_u8str(dxgidesc.Description);
			rhidescs.emplace_back(rhidesc);
		}
		return rhidescs;
	}

	std::shared_ptr<RHIDevice> RHID3D12::CreateDevice(const std::u8string & uri) const
	{
		if (!_dxgi)
			return nullptr;

		std::vector<RHIAdapterDesc> rhidescs;
		UINT adapterIndex = 0;
		win32::comptr<IDXGIAdapter1> adapter;

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

		auto device = std::make_shared<RHID3D12Device>();
		auto err = device->Init(adapter);
		if (err)
			return nullptr;
		return device;
	}
}