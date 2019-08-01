#include "stdafx.h"
#include "RHID3D12ResourcePacket.h"
#include "RHID3D12Resource.h"

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
		heapDesc.NumDescriptors = args.capacity;
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
		_views.resize(args.capacity);
		return core::ok;
	}

	std::shared_ptr<RHIResourceView> RHID3D12ResourcePacket::SetShaderResource(uint32_t index, RHIResource * resource, const ResourceViewArgs & args)
	{
		if (!_device)
			return nullptr;

		HRESULT hr = S_OK;

		auto device = _device->Inner();
		assert(device);

		D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle = _heap->GetCPUDescriptorHandleForHeapStart();
		D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle = _heap->GetGPUDescriptorHandleForHeapStart();
		CPUHandle.ptr += index * _unit;
		GPUHandle.ptr += index * _unit;
		
		if (args.type == ResourceType::ConstBuffer)
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC viewDesc = {};
			viewDesc.BufferLocation = static_cast<RHID3D12Resource *>(resource)->GPUVirtualAddress();
			viewDesc.SizeInBytes = (resource->Size().cx + 255) & ~255;
			//viewDesc.SizeInBytes = resource->Size().cx;

			device->CreateConstantBufferView(&viewDesc, CPUHandle);
		}
		else if(args.type == ResourceType::ShaderResource)
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Format = win32::DXGI::FromPixelFormat(args.shaderresource.format);
			srvDesc.ViewDimension = FromResourceViewDimension(args.shaderresource.dimension);
			srvDesc.Texture2D.MipLevels = 1;
			device->CreateShaderResourceView(static_cast<RHID3D12Resource *>(resource)->Resource(), &srvDesc, CPUHandle);
		}
		else
		{
			return nullptr;
		}

		_views[index] = std::make_shared<RHID3D12ResourceView>(CPUHandle, GPUHandle);
		return _views[index];
	}
}
