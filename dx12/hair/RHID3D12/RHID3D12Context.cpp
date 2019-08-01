#include "stdafx.h"
#include "RHID3D12ResoucePacket.h"

namespace RHI::RHID3D12
{
	core::error RHID3D12ResourcePacket::Create(const ResourcePacketArgs & args)
	{
		if (!_device)
			return core::e_state;

		HRESULT hr = S_OK;

		auto device = _device->Inner();
		assert(device);

		win32::comptr<ID3D12DescriptorHeap> heap;

		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = 1;
		switch(args.type)
		{
		case ResourcePacketType::Resource:
			heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			break;
		case ResourcePacketType::Sampler:
			heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
			break;
		case ResourcePacketType::RenderTarget:
			heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			break;
		case ResourcePacketType::DepthStencil:
			heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			break;
		default:
			heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			break;
		}
		heapDesc.Flags = FromDescriptorHeapFlags(args.flags);
		hr = device->CreateDescriptorHeap(&heapDesc, __uuidof(ID3D12DescriptorHeap), heap.getvv());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " device->CreateDescriptorHeap failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}

		_heap = heap;
		_unit = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		return core::ok;
	}

	std::shared_ptr<RHIResourceView> RHID3D12ResourcePacket::CreateShaderResourceView(const RHIResource * resource, const ShaderResourceViewArgs & args) const
	{
		return nullptr;
	}
	
	std::shared_ptr<RHIResourceView> RHID3D12ResourcePacket::CreateConstBufferView(const RHIResource * resource, const ConstBufferViewArgs & args) const
	{
		return nullptr;
	}

	core::error RHID3D12ResourcePacket::SetShaderResource(RHIResource * resource, const ShaderResourceViewArgs & args)
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
		return core::ok;
	}
}
