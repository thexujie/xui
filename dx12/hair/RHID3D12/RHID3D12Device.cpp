#include "stdafx.h"
#include "RHID3D12Device.h"
#include "RHID3D12CommandQueue.h"
#include "RHID3D12RenderTarget.h"
#include "RHID3D12CommandList.h"
#include "RHID3D12Buffer.h"

namespace RHI::RHID3D12
{
	static RHIAdapterDesc RHIAdapterDescFromAdapter(win32::comptr<IDXGIAdapter1> adapter)
	{
		DXGI_ADAPTER_DESC1 dxgidesc = {};
		adapter->GetDesc1(&dxgidesc);
		adapter.reset();

		RHIAdapterDesc rhidesc;
		rhidesc.uri = core::format("DXGI\\DEVICEID_", dxgidesc.DeviceId);
		rhidesc.name = core::wstr_u8str(dxgidesc.Description);
		return rhidesc;
	}

	core::error RHID3D12Device::Init(win32::comptr<IDXGIAdapter1> adapter)
	{
		if (!adapter)
			return core::e_args;

		win32::comptr<ID3D12Device> device;
		HRESULT hr = D3D12CreateDevice(adapter.get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), device.getvv());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " D3D12CreateDevice failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}

		_desc = RHIAdapterDescFromAdapter(adapter);
		_adapter = adapter;
		_device = device;
		return core::ok;
	}

	std::shared_ptr<RHICommandQueue> RHID3D12Device::CreateCommandQueue(CommandType type, CommandQueueFlags flags) const
	{
		auto queue = std::make_shared<RHID3D12CommandQueue>(const_cast<RHID3D12Device *>(this));
		auto err = queue->Init(type, flags);
		if (err)
			return nullptr;
		return queue;
	}

	std::shared_ptr<RHICommandList> RHID3D12Device::CreateCommandList(CommandType type) const
	{
		auto list = std::make_shared<RHID3D12CommandList>(const_cast<RHID3D12Device *>(this));
		auto err = list->Init(type);
		if (err)
			return nullptr;
		return list;
	}

	std::shared_ptr<RHIBuffer> RHID3D12Device::CreateBuffer(const BufferParams & params) const
	{
		auto buffer = std::make_shared<RHID3D12Buffer>(const_cast<RHID3D12Device *>(this));
		auto err = buffer->Init(params);
		if (err)
			return nullptr;
		return buffer;
	}

	std::shared_ptr<RHIRenderTarget> RHID3D12Device::CreateRenderTargetForHWND(const RenderTargetParams & params) const
	{
		auto rt = std::make_shared<RHID3D12RenderTarget>(const_cast<RHID3D12Device *>(this));
		auto err = rt->Init(params);
		if (err)
			return nullptr;

		return rt;
	}
}
