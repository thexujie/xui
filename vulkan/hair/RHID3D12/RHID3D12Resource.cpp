#include "stdafx.h"
#include "RHID3D12Resource.h"
#include "RHID3D12CommandList.h"

namespace RHI::RHID3D12
{
	core::error RHID3D12Resource::Create(const ResourceArgs & args)
	{
		if (!_device)
			return core::e_state;

		HRESULT hr = S_OK;

		auto device = _device->Device();
		auto adapter = _device->Adapter();
		assert(device);
		assert(adapter);

		D3D12_RESOURCE_DESC resdesc = {};
		resdesc.Dimension = FromResourceDimension(args.dimension);
		resdesc.Alignment = args.alignment;
		resdesc.Width = args.size.cx;
		resdesc.Height = args.size.cy;
		resdesc.DepthOrArraySize = args.depth;
		resdesc.MipLevels = args.miplevels;
		resdesc.Format = FromFormat(args.format);
		resdesc.SampleDesc.Count = args.MSAA;
		resdesc.SampleDesc.Quality = args.MSAAQuality;
		resdesc.Layout = args.dimension == ResourceDimension::Buffer ? D3D12_TEXTURE_LAYOUT_ROW_MAJOR : D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resdesc.Flags = FromResourceFlags(args.flags);

		D3D12_HEAP_PROPERTIES heapprop = {};
		heapprop.Type = FromHeapType(args.heap.type);
		heapprop.CPUPageProperty = FromCPUPageProperty(args.heap.cpupageproperty);
		heapprop.MemoryPoolPreference = FromMemoryPool(args.heap.memorypool);
		heapprop.CreationNodeMask = 1;
		heapprop.VisibleNodeMask = 1;

		core::comptr<ID3D12Resource> resource;
		hr = device->CreateCommittedResource(&heapprop, FromHeapFlags(args.heap.flags), &resdesc, FromResourceStates(args.states), nullptr, __uuidof(ID3D12Resource), resource.getvv());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " device->CreateCommittedResource failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}

		void * pointer = nullptr;
		if (args.heap.type == HeapType::Upload)
		{
			D3D12_RANGE range = {};
			resource->Map(0, &range, &pointer);
		}

		_args = args;
		_resource = resource;
		_state = _args.states;
		_pointer = pointer;
		return core::ok;
	}

	void RHID3D12Resource::SetName(const std::u8string & name)
	{
		SetD3D12ObjectName(_resource.get(), name);
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
}
