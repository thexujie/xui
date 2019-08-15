#include "stdafx.h"
#include "RHID3D11ResourcePacket.h"
#include "RHID3D11Resource.h"

namespace RHI::RHID3D11
{
	core::error RHID3D11ResourcePacket::Create(const ResourcePacketArgs & args)
	{
		if (!_device)
			return core::e_state;

		HRESULT hr = S_OK;

		auto device = _device->Device();
		assert(device);

		//core::comptr<ID3D12DescriptorHeap> heap;

		//D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		//heapDesc.NumDescriptors = args.capacity;
		//switch(args.type)
		//{
		//case ResourcePacketType::Resource:
		//	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		//	break;
		//case ResourcePacketType::Sampler:
		//	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		//	break;
		//case ResourcePacketType::RenderTarget:
		//	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		//	break;
		//case ResourcePacketType::DepthStencil:
		//	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		//	break;
		//default:
		//	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		//	break;
		//}
		//heapDesc.Flags = FromDescriptorHeapFlags(args.flags);
		//hr = device->CreateDescriptorHeap(&heapDesc, __uuidof(ID3D12DescriptorHeap), heap.getvv());
		//if (FAILED(hr))
		//{
		//	core::war() << __FUNCTION__ " device->CreateDescriptorHeap failed: " << win32::winerr_str(hr & 0xFFFF);
		//	return core::e_inner;
		//}

		//_heap = heap;
		//_unit = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		_resources.resize(args.capacity);
		_viewArgs.resize(args.capacity);
		_views.resize(args.capacity);
		return core::ok;
	}

	void RHID3D11ResourcePacket::SetResource(uint32_t index, RHIResource * resource, const ResourceViewArgs & args)
	{
		if (!_device)
			return;

		HRESULT hr = S_OK;

		auto device = _device->Device();
		assert(device);

		if (args.type == ResourceType::ConstBuffer)
		{
			//viewDesc.BufferLocation = static_cast<RHID3D12Resource *>(resource)->GPUVirtualAddress();
			//viewDesc.SizeInBytes = (resource->Size().cx + 255) & ~255;
			////viewDesc.SizeInBytes = resource->Size().cx;

			//device->CreateConstantBufferView(&viewDesc, CPUHandle);
		}
		else if(args.type == ResourceType::ShaderResource)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.ViewDimension = FromResourceViewDimension(args.resource.dimension);
			srvDesc.Format = FromFormat(args.resource.format);
			if (args.resource.dimension == ResourceViewDimension::Buffer)
			{
				srvDesc.Buffer.ElementOffset = args.resource.buffer.firstElement;
				srvDesc.Buffer.NumElements = args.resource.buffer.numElements;
			}
			else if (args.resource.dimension == ResourceViewDimension::Texture2D)
			{
				srvDesc.Texture2D.MipLevels = 1;
			}
			else
			{

			}

			core::comptr<ID3D11ShaderResourceView> view;
			hr = device->CreateShaderResourceView(static_cast<RHID3D11Resource *>(resource)->Resource() , &srvDesc, view.getpp());
			if (FAILED(hr))
			{
				core::war() << __FUNCTION__ " CreateShaderResourceView failed: " << win32::winerr_str(hr & 0xFFFF);
				return;
			}

			_views[index] = view;
		}
		else if (args.type == ResourceType::UnorderedAccess)
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.Format = FromFormat(args.resource.format);
			uavDesc.ViewDimension = FromResourceViewDimension_UAV(args.resource.dimension);
			if (args.resource.dimension == ResourceViewDimension::Buffer)
			{
				uavDesc.Buffer.FirstElement = args.resource.buffer.firstElement;
				uavDesc.Buffer.NumElements = args.resource.buffer.numElements;
				uavDesc.Buffer.Flags = 0;
			}
			else if (args.resource.dimension == ResourceViewDimension::Texture2D)
			{
				uavDesc.Texture2D.MipSlice = 0;
			}
			else
			{

			}

			core::comptr<ID3D11UnorderedAccessView> view;
			hr = device->CreateUnorderedAccessView(static_cast<RHID3D11Resource *>(resource)->Resource(), &uavDesc, view.getpp());
			if (FAILED(hr))
			{
				core::war() << __FUNCTION__ " CreateUnorderedAccessView failed: " << win32::winerr_str(hr & 0xFFFF);
				return;
			}

			_views[index] = view;
		}
		else
		{
		}

		_resources[index] = resource;
		_viewArgs[index] = args;
	}
}
