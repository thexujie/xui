#include "stdafx.h"
#include "RHID3D12Device.h"
#include "RHID3D12CommandQueue.h"
#include "RHID3D12RenderTargetHWND.h"
#include "RHID3D12CommandList.h"
#include "RHID3D12Resource.h"
#include "RHID3D12CommandAllocator.h"
#include "RHID3D12PipelineState.h"
#include "RHID3D12ResourcePacket.h"

namespace RHI::RHID3D12
{
	static RHIAdapterDesc RHIAdapterDescFromAdapter(core::comptr<IDXGIAdapter1> adapter)
	{
		DXGI_ADAPTER_DESC1 dxgidesc = {};
		adapter->GetDesc1(&dxgidesc);
		adapter.reset();

		RHIAdapterDesc rhidesc;
		rhidesc.uri = core::fmt("DXGI\\DEVICEID_", dxgidesc.DeviceId);
		rhidesc.name = core::wstr_u8str(dxgidesc.Description);
		return rhidesc;
	}

	core::error RHID3D12Device::Create(core::comptr<IDXGIAdapter1> adapter)
	{
		if (!adapter)
			return core::e_args;

		core::comptr<ID3D12Device> device;
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
		auto err = queue->Create(type, flags);
		if (err)
			return nullptr;
		return queue;
	}

	std::shared_ptr<RHICommandAllocator> RHID3D12Device::CreateCommandAllocator(CommandType type) const
	{
		auto allocator = std::make_shared<RHID3D12CommandAllocator>(const_cast<RHID3D12Device *>(this));
		auto err = allocator->Create(type);
		if (err)
			return nullptr;
		return allocator;
	}

	std::shared_ptr<RHICommandList> RHID3D12Device::CreateCommandList(CommandType type) const
	{
		auto list = std::make_shared<RHID3D12CommandList>(const_cast<RHID3D12Device *>(this));
		auto err = list->Create(type);
		if (err)
			return nullptr;
		return list;
	}

	std::shared_ptr<RHIResource> RHID3D12Device::CreateResource(const ResourceArgs & args) const
	{
		auto buffer = std::make_shared<RHID3D12Resource>(const_cast<RHID3D12Device *>(this));
		auto err = buffer->Create(args);
		if (err)
			return nullptr;
		return buffer;
	}

	std::shared_ptr<RHIResourcePacket> RHID3D12Device::CreateResourcePacket(const ResourcePacketArgs & args) const
	{
		auto packet = std::make_shared<RHID3D12ResourcePacket>(const_cast<RHID3D12Device *>(this));
		auto err = packet->Create(args);
		if (err)
			return nullptr;
		return packet;
	}
	
	std::shared_ptr<RHIRenderTarget> RHID3D12Device::CreateRenderTargetForHWND(RHICommandQueue * cmdqueue, const RenderTargetArgs & args) const
	{
		auto rt = std::make_shared<RHID3D12RenderTargetHWND>(const_cast<RHID3D12Device *>(this));
		auto err = rt->Create(cmdqueue, args);
		if (err)
			return nullptr;

		return rt;
	}

	std::shared_ptr<RHIPipelineState> RHID3D12Device::CreatePipelineState(const PipelineStateArgs & args) const
	{
		auto rt = std::make_shared<RHID3D12PipelineState>(const_cast<RHID3D12Device *>(this));
		auto err = rt->Create(args);
		if (err)
			return nullptr;

		return rt;
	}
}
