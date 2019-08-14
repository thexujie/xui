#include "stdafx.h"
#include "RHID3D11Device.h"
#include "RHID3D11CommandAllocator.h"
#include "RHID3D11CommandQueue.h"
#include "RHID3D11CommandList.h"
#include "RHID3D11Resource.h"
#include "RHID3D11ResourcePacket.h"
#include "RHID3D11RenderTargetHWND.h"
#include "RHID3D11PipelineState.h"

namespace RHI::RHID3D11
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

	core::error RHID3D11Device::Create(core::comptr<IDXGIAdapter1> adapter)
	{
		if (!adapter)
			return core::e_args;

		core::comptr<ID3D11Device> device;
		core::comptr<ID3D11DeviceContext> immediateContext;
		uint32_t flags = 0;
#ifdef _DEBUG
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
		D3D_FEATURE_LEVEL featureLevel = static_cast<D3D_FEATURE_LEVEL>(0);
		HRESULT hr = D3D11CreateDevice(adapter.get(), D3D_DRIVER_TYPE_UNKNOWN, NULL,
			flags, featureLevels, std::size(featureLevels), D3D11_SDK_VERSION, device.getpp(), &featureLevel, immediateContext.getpp());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " D3D11CreateDevice failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}

		_desc = RHIAdapterDescFromAdapter(adapter);
		_adapter = adapter;
		_device = device;

		return core::ok;
	}

	std::shared_ptr<RHICommandQueue> RHID3D11Device::CreateCommandQueue(CommandType type, CommandQueueFlags flags) const
	{
		auto queue = std::make_shared<RHID3D11CommandQueue>(const_cast<RHID3D11Device *>(this));
		auto err = queue->Create(type, flags);
		if (err)
			return nullptr;
		return queue;
	}

	std::shared_ptr<RHICommandAllocator> RHID3D11Device::CreateCommandAllocator(CommandType type) const
	{
		auto allocator = std::make_shared<RHID3D11CommandAllocator>(const_cast<RHID3D11Device *>(this));
		auto err = allocator->Create(type);
		if (err)
			return nullptr;
		return allocator;
	}

	std::shared_ptr<RHICommandList> RHID3D11Device::CreateCommandList(CommandType type, RHICommandAllocator * allocator) const
	{
		auto list = std::make_shared<RHID3D11CommandList>(const_cast<RHID3D11Device *>(this));
		auto err = list->Create(type, allocator);
		if (err)
			return nullptr;
		return list;
	}

	std::shared_ptr<RHIResource> RHID3D11Device::CreateResource(const ResourceArgs & args) const
	{
		auto resource = std::make_shared<RHID3D11Resource>(const_cast<RHID3D11Device *>(this));
		auto err = resource->Create(args);
		if (err)
			return nullptr;
		return resource;
	}

	std::shared_ptr<RHIResourcePacket> RHID3D11Device::CreateResourcePacket(const ResourcePacketArgs & args) const
	{
		auto packet = std::make_shared<RHID3D11ResourcePacket>(const_cast<RHID3D11Device *>(this));
		auto err = packet->Create(args);
		if (err)
			return nullptr;
		return packet;
	}
	
	std::shared_ptr<RHIRenderTarget> RHID3D11Device::CreateRenderTargetForHWND(RHICommandQueue * cmdqueue, const RenderTargetArgs & args) const
	{
		auto rt = std::make_shared<RHID3D11RenderTargetHWND>(const_cast<RHID3D11Device *>(this));
		auto err = rt->Create(cmdqueue, args);
		if (err)
			return nullptr;

		return rt;
	}

	std::shared_ptr<RHIPipelineState> RHID3D11Device::CreatePipelineState(const PipelineStateArgs & args) const
	{
		auto pso = std::make_shared<RHID3D11PipelineState>(const_cast<RHID3D11Device *>(this));
		auto err = pso->Create(args);
		if (err)
			return nullptr;

		return pso;
	}
}
