#include "stdafx.h"
#include "RHID3D12Resource.h"
#include "RHID3D12CommandList.h"

namespace RHI::RHID3D12
{
	core::error RHID3D12Resource::Create(const ResourceParams & params)
	{
		if (!_device)
			return core::e_state;

		HRESULT hr = S_OK;

		auto device = _device->Inner();
		auto adapter = _device->InnerAdapter();
		assert(device);
		assert(adapter);

		D3D12_RESOURCE_DESC resdesc = {};
		resdesc.Dimension = FromResourceDimension(params.dimension);
		resdesc.Alignment = params.alignment;
		resdesc.Width = params.size.cx;
		resdesc.Height = params.size.cy;
		resdesc.DepthOrArraySize = params.depth;
		resdesc.MipLevels = params.miplevels;
		resdesc.Format = win32::DXGI::FromPixelFormat(params.format);
		resdesc.SampleDesc.Count = params.MSAA;
		resdesc.SampleDesc.Quality = params.MSAAQuality;
		resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resdesc.Flags = FromResourceFlags(params.flags);

		D3D12_HEAP_PROPERTIES heapprop = {};
		heapprop.Type = FromBufferType(params.heap.type);
		heapprop.CPUPageProperty = FromCPUPageProperty(params.heap.cpupageproperty);
		heapprop.MemoryPoolPreference = FromMemoryPool(params.heap.memorypool);
		heapprop.CreationNodeMask = 1;
		heapprop.VisibleNodeMask = 1;

		win32::comptr<ID3D12Resource> buffer;
		hr = device->CreateCommittedResource(&heapprop, FromHeapFlags(params.heap.flags), &resdesc, FromResourceStates(params.states), nullptr, __uuidof(ID3D12Resource), buffer.getvv());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " device->CreateCommittedResource failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}

		_params = params;
		_resource = buffer;
		_states = _params.states;
		return core::ok;
	}

	void RHID3D12Resource::TransitionBarrier(class RHICommandList * cmdlist, ResourceStates states)
	{
		if (!cmdlist)
			return;

		auto d3d12cmdlist = reinterpret_cast<RHID3D12CommandList *>(cmdlist)->Ptr();
		assert(d3d12cmdlist);

		D3D12_RESOURCE_BARRIER barrier;
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = _resource.get();
		barrier.Transition.StateBefore = FromResourceStates(_states);
		barrier.Transition.StateAfter = FromResourceStates(states);
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		d3d12cmdlist->ResourceBarrier(1, &barrier);
		_states = states;
	}
}
