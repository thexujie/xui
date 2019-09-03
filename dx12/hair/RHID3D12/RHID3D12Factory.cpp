#include "stdafx.h"
#include "RHID3D12Factory.h"
#include "RHID3D12Device.h"

namespace RHI::RHID3D12
{
	core::error RHID3D12Factory::Load()
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
		core::comptr<ID3D12Debug> debugInterface;
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

	std::vector<RHIAdapterDesc> RHID3D12Factory::AdapterDescs() const
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

			core::comptr<ID3D12Device> device;
			HRESULT hr = D3D12CreateDevice(adapter.get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), device.getvv());
			if (FAILED(hr))
			{
				core::war() << __FUNCTION__ << " D3D12CreateDevice failed.";
				continue;
			}

			D3D12_FEATURE_DATA_ARCHITECTURE architecture = {};
			hr = device->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE, &architecture, sizeof(architecture));
			if (FAILED(hr))
			{
				core::war() << __FUNCTION__ << " CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE) failed.";
				continue;
			}
			
			//D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS MSAALevels = {};
			//MSAALevels.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			//MSAALevels.SampleCount = 2;
			//hr = device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &MSAALevels, sizeof(MSAALevels));
			//if (FAILED(hr))
			//{
			//	core::war() << __FUNCTION__ << " CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS) failed.";
			//	continue;
			//}
			
			RHIAdapterDesc rhidesc;
			rhidesc.id = dxgidesc.DeviceId;
			rhidesc.name = core::wstr_u8str(dxgidesc.Description);
			rhidesc.flags.set(RHIAdapterFlag::CacheCoherentUMA, architecture.CacheCoherentUMA);
			rhidesc.flags.set(RHIAdapterFlag::TileBasedRender, architecture.TileBasedRenderer);
			rhidesc.flags.set(RHIAdapterFlag::Software, dxgidesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE);
			rhidescs.emplace_back(rhidesc);
		}
		return rhidescs;
	}

	std::shared_ptr<RHIDevice> RHID3D12Factory::CreateDevice(uint64_t id) const
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

		auto device = std::make_shared<RHID3D12Device>(const_cast<RHID3D12Factory *>(this));
		auto err = device->Create(adapter);
		if (err)
			return nullptr;
		return device;
	}
}