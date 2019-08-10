#include "stdafx.h"
#include "RHID3D12Resource.h"
#include "RHID3D12CommandList.h"

namespace RHI::RHID3D12
{
	core::error RHID3D12Resource::Create(const ResourceArgs & params)
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
		resdesc.Layout = params.dimension == ResourceDimension::Raw? D3D12_TEXTURE_LAYOUT_ROW_MAJOR : D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resdesc.Flags = FromResourceFlags(params.flags);

		D3D12_HEAP_PROPERTIES heapprop = {};
		heapprop.Type = FromBufferType(params.heap.type);
		heapprop.CPUPageProperty = FromCPUPageProperty(params.heap.cpupageproperty);
		heapprop.MemoryPoolPreference = FromMemoryPool(params.heap.memorypool);
		heapprop.CreationNodeMask = 1;
		heapprop.VisibleNodeMask = 1;

		win32::comptr<ID3D12Resource> resource;
		hr = device->CreateCommittedResource(&heapprop, FromHeapFlags(params.heap.flags), &resdesc, FromResourceStates(params.states), nullptr, __uuidof(ID3D12Resource), resource.getvv());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " device->CreateCommittedResource failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}

		void * pointer = nullptr;
		if (params.heap.type == HeapType::Upload)
		{
			D3D12_RANGE range = {};
			resource->Map(0, &range, &pointer);
		}

		RHI::RHID3D12::SetD3D12ObjectName(resource.get(), L"resource");
		_args = params;
		_resource = resource;
		_states = _args.states;
		_pointer = pointer;
		return core::ok;
	}

	void * RHID3D12Resource::Data()
	{
		return _pointer;
	}
	
	const void * RHID3D12Resource::Data() const
	{
		return _pointer;
	}
	
	core::sizeu RHID3D12Resource::Size() const
	{
		return _args.size;
	}

	void * RHID3D12Resource::Map()
	{
		if (_pointer)
			return _pointer;
		
		D3D12_RANGE range = {};
		_resource->Map(0, &range, &_pointer);
		return _pointer;
	}
	
	void RHID3D12Resource::Unmap()
	{
		_resource->Unmap(0, nullptr);
		_pointer = nullptr;
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

		if ((barrier.Transition.StateBefore & barrier.Transition.StateAfter) != 0)
		{
			core::war() << __FUNCTION__ " RESOURCE_MANIPULATION";
			return;
		}
		
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		d3d12cmdlist->ResourceBarrier(1, &barrier);
		_states = states;
	}
}
