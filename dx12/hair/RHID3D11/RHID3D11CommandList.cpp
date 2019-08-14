#include "stdafx.h"
#include "RHID3D11CommandList.h"
#include "RHID3D11CommandAllocator.h"
//#include "RHID3D11Resource.h"
#include "RHID3D11RenderTarget.h"
#include "RHID3D11Resource.h"

//#include "RHID3D11PipelineState.h"
//#include "RHID3D11ResourcePacket.h"

namespace RHI::RHID3D11
{
	core::error RHID3D11CommandList::Create(CommandType type, RHICommandAllocator * allocator)
	{
		if (!_device)
			return core::e_state;

		HRESULT hr = S_OK;

		auto device = _device->Inner();
		auto adapter = _device->InnerAdapter();
		auto d3d11allocator = static_cast<RHID3D11CommandAllocator *>(allocator);
		assert(device);
		assert(adapter);
		_deferredContext.reset(d3d11allocator->DeviceContext());
		return core::ok;
	}

	void RHID3D11CommandList::SetName(const std::u8string & name)
	{
	}

	void RHID3D11CommandList::Reset(RHICommandAllocator * allocator)
	{
		_deferredContext.reset(reinterpret_cast<RHID3D11CommandAllocator *>(allocator)->DeviceContext());
		_deferredContext->ClearState();
	}

	void RHID3D11CommandList::Close()
	{
		_deferredContext->FinishCommandList(true, _commandList.getpp());
	}

	void RHID3D11CommandList::SetRenderTarget(RHIRenderTarget * rendertarget)
	{
		auto d3d11rendertarget = static_cast<RHID3D11RenderTarget *>(rendertarget);
		ID3D11RenderTargetView * rtvs[] = { d3d11rendertarget->RenderTargetView() };
		_deferredContext->OMSetRenderTargets(1, rtvs, nullptr);
	}

	void RHID3D11CommandList::ClearRenderTarget(RHIRenderTarget * rendertarget, core::color color)
	{
		auto d3d11rendertarget = static_cast<RHID3D11RenderTarget *>(rendertarget);
		const float clearColor[] = { color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };
		_deferredContext->ClearRenderTargetView(d3d11rendertarget->RenderTargetView(), clearColor);
	}

	void RHID3D11CommandList::SetViewPort(const ViewPort & viewport)
	{
		D3D11_VIEWPORT d3d12viewport = {};
		d3d12viewport.TopLeftX = viewport.left;
		d3d12viewport.TopLeftY = viewport.top;
		d3d12viewport.Width = viewport.width;
		d3d12viewport.Height = viewport.height;
		d3d12viewport.MinDepth = viewport.nearZ;
		d3d12viewport.MaxDepth = viewport.farZ;

		_deferredContext->RSSetViewports(1, &d3d12viewport);
	}

	void RHID3D11CommandList::SetScissorRect(const core::recti & rect)
	{
		D3D11_RECT d3d12rect = {};
		d3d12rect.left = rect.x;
		d3d12rect.top = rect.y;
		d3d12rect.right = rect.right();
		d3d12rect.bottom = rect.bottom();

		_deferredContext->RSSetScissorRects(1, &d3d12rect);
	}

	void RHID3D11CommandList::TransitionBarrier(RHIResource * resource, ResourceStates states)
	{
		//static_cast<RHID3D12Resource *>(resource)->TransitionBarrier(this, states);
	}
	
	void RHID3D11CommandList::TransitionBarrier(RHIRenderTarget * rendertarget, ResourceStates states)
	{
		static_cast<RHIRenderTarget *>(rendertarget)->TransitionBarrier(this, states);
	}
	
	void RHID3D11CommandList::SetPipelineState(RHIPipelineState * pipelinestate)
	{
		//auto d3d12pipelinestate = static_cast<RHID3D12PipelineState *>(pipelinestate);
		//_cmdlist->SetPipelineState(d3d12pipelinestate->PipelineState());
		//if (d3d12pipelinestate->Args().CS.empty())
		//	_cmdlist->SetGraphicsRootSignature(d3d12pipelinestate->RootSignature());
		//else
		//	_cmdlist->SetComputeRootSignature(d3d12pipelinestate->RootSignature());
	}
	
	void RHID3D11CommandList::SetResourcePacket(RHIResourcePacket * packet)
	{
		//_resourcepacket = static_cast<RHID3D12ResourcePacket *>(packet);
		//ID3D12DescriptorHeap * heaps[1] = { _resourcepacket->DescriptorHeap()};
		//_cmdlist->SetDescriptorHeaps(1, heaps);
	}

	void RHID3D11CommandList::SetGraphicsResources(uint32_t index, uint32_t packetoffset)
	{
		assert(_resourcepacket);
		if (!_resourcepacket)
			return;
		
		//_cmdlist->SetGraphicsRootDescriptorTable(index, _resourcepacket->GPUDescriptorHandle(packetoffset));
	}
	
	void RHID3D11CommandList::IASetVertexBuffer(RHIResource * resource, uint32_t stride, uint32_t size)
	{
		//D3D12_VERTEX_BUFFER_VIEW vbv = {};
		//vbv.BufferLocation = static_cast<RHID3D12Resource *>(resource)->GPUVirtualAddress();
		//vbv.StrideInBytes = stride;
		//vbv.SizeInBytes = size;
		//_cmdlist->IASetVertexBuffers(0, 1, &vbv);
	}
	
	void RHID3D11CommandList::IASetIndexBuffer(RHIResource * resource, uint32_t stride, uint32_t size)
	{
		//D3D12_INDEX_BUFFER_VIEW vbv = {};
		//vbv.BufferLocation = static_cast<RHID3D12Resource *>(resource)->GPUVirtualAddress();
		//vbv.Format = stride == 4 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
		//vbv.SizeInBytes = size;
		//_cmdlist->IASetIndexBuffer(&vbv);
	}

	void RHID3D11CommandList::IASetTopologyType(Topology topology)
	{
		_deferredContext->IASetPrimitiveTopology(FromTopology(topology));
	}

	void RHID3D11CommandList::DrawInstanced(uint32_t nvertices, uint32_t ninstance, uint32_t ivertexbase, uint32_t iinstancebase)
	{
		_deferredContext->DrawInstanced(nvertices, ninstance, ivertexbase, iinstancebase);
	}

	void RHID3D11CommandList::DrawIndexedInstanced(uint32_t nindices, uint32_t ninstance, uint32_t iindexbase, uint32_t ivertexbase, uint32_t iinstancebase)
	{
		_deferredContext->DrawIndexedInstanced(nindices, ninstance, iindexbase, ivertexbase, iinstancebase);
	}

	void RHID3D11CommandList::SetComputeResources(uint32_t index, uint32_t packetoffset)
	{
		assert(_resourcepacket);
		if (!_resourcepacket)
			return;

		//_cmdlist->SetComputeRootDescriptorTable(index, _resourcepacket->GPUDescriptorHandle(packetoffset));
	}
	
	void RHID3D11CommandList::Dispatch(core::uint3 ngroups)
	{
		_deferredContext->Dispatch(ngroups.x, ngroups.y, ngroups.z);
	}
	
	void RHID3D11CommandList::CopyResource(RHIResource * dst, RHIResource * src)
	{
		auto device = _device->Inner();
		auto rhid3d11dst = static_cast<RHID3D11Resource *>(dst);
		auto rhid3d11src = static_cast<RHID3D11Resource *>(src);
		if (rhid3d11dst->Args().dimension == ResourceDimension::Texture2D)
		{
			//D3D12_TEXTURE_COPY_LOCATION destLocation;
			//destLocation.pResource = rhid3d12dst->Resource();
			//destLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			//destLocation.SubresourceIndex = 0;
			//
			//D3D12_RESOURCE_DESC descDest = rhid3d12dst->Resource()->GetDesc();
			//D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout = {};
			//UINT nrows = 0;
			//UINT64 stride = 0;
			//UINT64 nbtotal = 0;
			//device->GetCopyableFootprints(&descDest, 0, 1, 0, &layout, &nrows, &stride, &nbtotal);
			//D3D12_TEXTURE_COPY_LOCATION srcLocation;
			//srcLocation.pResource = rhid3d12src->Resource();
			//srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			//srcLocation.PlacedFootprint = layout;
			_deferredContext->CopySubresourceRegion(static_cast<RHID3D11Resource *>(dst)->Resource(), 0, 0, 0, 0, static_cast<RHID3D11Resource *>(rhid3d11src)->Resource(), 0, NULL);
		}
		else
		{
			_deferredContext->CopyResource(static_cast<RHID3D11Resource *>(dst)->Resource(), static_cast<RHID3D11Resource *>(src)->Resource());
		}
	}
	
	void RHID3D11CommandList::CopyBuffer(RHIResource * dst, RHIResource * src)
	{
		//auto size = static_cast<RHID3D12Resource *>(src)->Size().cx * static_cast<RHID3D12Resource *>(src)->Size().cy;
		//_cmdlist->CopyBufferRegion(static_cast<RHID3D12Resource *>(dst)->Resource(), 0, static_cast<RHID3D12Resource *>(src)->Resource(), 0, size);
	}
}
