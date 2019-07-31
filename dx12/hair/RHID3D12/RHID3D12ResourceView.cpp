#include "stdafx.h"
#include "RHID3D12ResourceView.h"
#include "RHID3D12CommandList.h"
#include "RHID3D12Resource.h"

namespace RHI::RHID3D12
{
	core::error RHID3D12ResourceView2::Create(const RHID3D12Resource * resource, const ResourceViewParams & params)
	{
		if (!_device)
			return core::e_state;

		HRESULT hr = S_OK;

		auto device = _device->Inner();
		assert(device);

		win32::comptr<ID3D12DescriptorHeap> heap;

		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = 1;
		heapDesc.Type = FromDescriptorHeapType(params.type);
		heapDesc.Flags = FromDescriptorHeapFlags(params.flags);
		hr = device->CreateDescriptorHeap(&heapDesc, __uuidof(ID3D12DescriptorHeap), heap.getvv());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " device->CreateDescriptorHeap failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = win32::DXGI::FromPixelFormat(params.format);
		srvDesc.ViewDimension = FromResourceViewDimension(params.dimension);
		srvDesc.Texture2D.MipLevels = 1;
		device->CreateShaderResourceView(resource->Ptr(), &srvDesc, heap->GetCPUDescriptorHandleForHeapStart());

		_params = params;
		_heap = heap;
		return core::ok;
	}
}
