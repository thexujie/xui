#include "stdafx.h"
#include "RHID3D11Resource.h"
#include "RHID3D11CommandList.h"

namespace RHI::RHID3D11
{
	core::error RHID3D11Resource::Create(const ResourceArgs & args)
	{
		if (!_device)
			return core::e_state;

		HRESULT hr = S_OK;

		auto device = _device->Device();
		auto adapter = _device->Adapter();
		assert(device);
		assert(adapter);

		core::comptr<ID3D11Resource> resource;
		if (args.dimension == ResourceDimension::Buffer)
		{
			D3D11_BUFFER_DESC desc = {};
			desc.ByteWidth = args.size.cx;
			desc.Usage = FromHeapType(args.heap.type);
			desc.MiscFlags = 0;
			desc.BindFlags = FromResourceFlags(args.flags);
			desc.CPUAccessFlags = args.heap.type == HeapType::Upload ? D3D11_CPU_ACCESS_WRITE : 0;
			desc.StructureByteStride = 0;
			core::comptr<ID3D11Buffer> buffer;
			hr = device->CreateBuffer(&desc, NULL, buffer.getpp());
			if (FAILED(hr))
			{
				core::war() << __FUNCTION__ " CreateBuffer failed: " << win32::winerr_str(hr & 0xFFFF);
				return core::e_inner;
			}
			resource = buffer;
		}
		else if (args.dimension == ResourceDimension::Texture2D)
		{
			D3D11_TEXTURE2D_DESC desc = {};
			desc.Width = args.size.cx;
			desc.Height = args.size.cy;
			desc.MipLevels = args.miplevels;
			desc.ArraySize = args.depth;
			desc.Format = FromFormat(args.format);
			desc.SampleDesc.Count = 1;
			desc.Usage = FromHeapType(args.heap.type);
			desc.MiscFlags = 0;
			desc.BindFlags = FromResourceFlags(args.flags);
			desc.CPUAccessFlags = 0;
			core::comptr<ID3D11Texture2D> texture;
			hr = device->CreateTexture2D(&desc, NULL, texture.getpp());
			if (FAILED(hr))
			{
				core::war() << __FUNCTION__ " CreateTexture2D failed: " << win32::winerr_str(hr & 0xFFFF);
				return core::e_inner;
			}
			resource = texture;
		}
		else
		{
			return core::e_bad_format;
		}

		void * pointer = nullptr;
		if (args.heap.type == HeapType::Upload)
		{
			core::comptr<ID3D11DeviceContext> immediateContext;
			device->GetImmediateContext(immediateContext.getpp());
			D3D11_MAPPED_SUBRESOURCE mres = {};
			hr = immediateContext->Map(resource.get(), 0, D3D11_MAP_WRITE, 0, &mres);
			if (FAILED(hr))
			{
				core::war() << __FUNCTION__ " Map failed: " << win32::winerr_str(hr & 0xFFFF);
				return core::e_inner;
			}
			pointer = mres.pData;
		}

		_args = args;
		_resource = resource;
		_states = _args.states;
		_pointer = pointer;
		return core::ok;
	}

	void RHID3D11Resource::SetName(const std::u8string & name)
	{
		SetD3D11ObjectName(_resource.get(), name);
	}
	
	
	void RHID3D11Resource::TransitionBarrier(class RHICommandList * cmdlist, ResourceStates states)
	{
		if (!cmdlist)
			return;
		
		auto d3d12cmdlist = reinterpret_cast<RHID3D11CommandList *>(cmdlist)->CommandList();
		assert(d3d12cmdlist);

		//D3D12_RESOURCE_BARRIER barrier;
		//barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		//barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		//barrier.Transition.pResource = _resource.get();
		//barrier.Transition.StateBefore = FromResourceStates(_states);
		//barrier.Transition.StateAfter = FromResourceStates(states);
		//barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		//if ((barrier.Transition.StateBefore & barrier.Transition.StateAfter) != 0)
		//{
		//	core::war() << __FUNCTION__ " RESOURCE_MANIPULATION";
		//	return;
		//}
		
		//d3d12cmdlist->ResourceBarrier(1, &barrier);
		_states = states;
	}
}
