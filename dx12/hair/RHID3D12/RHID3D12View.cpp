#include "stdafx.h"
#include "RHID3D12View.h"
#include "RHID3D12CommandList.h"
#include "RHID3D12Resource.h"

namespace RHI::RHID3D12
{
	core::error RHID3D12ShaderResourceView::Create(const RHID3D12Resource * resource, const ShaderResourceViewArgs & args)
	{
		if (!_device)
			return core::e_state;

		HRESULT hr = S_OK;

		auto device = _device->Inner();
		assert(device);

		win32::comptr<ID3D12DescriptorHeap> heap;

		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = 1;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		heapDesc.Flags = FromDescriptorHeapFlags(args.flags);
		hr = device->CreateDescriptorHeap(&heapDesc, __uuidof(ID3D12DescriptorHeap), heap.getvv());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " device->CreateDescriptorHeap failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = win32::DXGI::FromPixelFormat(args.format);
		srvDesc.ViewDimension = FromResourceViewDimension(args.dimension);
		srvDesc.Texture2D.MipLevels = 1;
		device->CreateShaderResourceView(resource->Ptr(), &srvDesc, heap->GetCPUDescriptorHandleForHeapStart());

		_args = args;
		_heap = heap;
		return core::ok;
	}

	core::error RHID3D12ConstBufferView::Create(const RHID3D12Resource * resource, const ConstBufferViewArgs & args)
	{
		if (!_device)
			return core::e_state;

		HRESULT hr = S_OK;

		auto device = _device->Inner();
		assert(device);

		win32::comptr<ID3D12DescriptorHeap> heap;

		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = 1;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		heapDesc.Flags = FromDescriptorHeapFlags(args.flags);
		hr = device->CreateDescriptorHeap(&heapDesc, __uuidof(ID3D12DescriptorHeap), heap.getvv());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " device->CreateDescriptorHeap failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}

		D3D12_CONSTANT_BUFFER_VIEW_DESC viewDesc = {};
		viewDesc.BufferLocation = resource->GPUVirtualAddress();
		viewDesc.SizeInBytes = (resource->Size().cx + 255) & ~255;
		//viewDesc.SizeInBytes = resource->Size().cx;
		device->CreateConstantBufferView(&viewDesc, heap->GetCPUDescriptorHandleForHeapStart());

		_args = args;
		_heap = heap;
		return core::ok;
	}
}
