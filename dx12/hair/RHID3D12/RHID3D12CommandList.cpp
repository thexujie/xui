#include "stdafx.h"
#include "RHID3D12CommandList.h"
#include "RHID3D12CommandAllocator.h"
#include "RHID3D12Resource.h"
#include "RHID3D12ResourceView.h"
#include "RHID3D12PipelineState.h"
#include "RHID3D12ResourcePacket.h"

namespace RHI::RHID3D12
{
	core::error RHID3D12CommandList::Create(CommandType type)
	{
		if (!_device)
			return core::e_state;

		HRESULT hr = S_OK;

		auto device = _device->Inner();
		auto adapter = _device->InnerAdapter();
		assert(device);
		assert(adapter);

		win32::comptr<ID3D12CommandAllocator> cmdallocator;
		win32::comptr<ID3D12GraphicsCommandList> cmdlist;

		hr = device->CreateCommandAllocator(FromCommandType(type), __uuidof(ID3D12CommandAllocator), cmdallocator.getvv());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " device->CreateCommandAllocator failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}
		SetD3D12ObjectName(cmdallocator.get(), L"cmdallocator");

		hr = device->CreateCommandList(0, FromCommandType(type), cmdallocator.get(), nullptr, __uuidof(ID3D12GraphicsCommandList), cmdlist.getvv());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " device->CreateCommandList failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}
		SetD3D12ObjectName(cmdlist.get(), L"cmdlist");

		cmdlist->Close();
		_cmdallocator = cmdallocator;
		_cmdlist = cmdlist;
		return core::ok;
	}

	void RHID3D12CommandList::Reset(RHICommandAllocator * allocator)
	{
		auto d3d12allocator = reinterpret_cast<RHID3D12CommandAllocator *>(allocator)->Ptr();
		_cmdlist->Reset(d3d12allocator, nullptr);
	}

	void RHID3D12CommandList::Close()
	{
		_cmdlist->Close();
	}

	void RHID3D12CommandList::SetRenderTarget(RHIResourceView * rendertarget)
	{
		_rendertarget = static_cast<RHID3D12ResourceView *>(rendertarget);
		if (_rendertarget)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle = _rendertarget->CPUDescriptorHandle();
			_cmdlist->OMSetRenderTargets(1, &handle, FALSE, nullptr);
		}
		else
			_cmdlist->OMSetRenderTargets(0, nullptr, FALSE, nullptr);
	}

	void RHID3D12CommandList::ClearRenderTarget(core::color color)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = { _rendertarget->CPUDescriptorHandle() };
		const float clearColor[] = { color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };
		_cmdlist->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	}

	void RHID3D12CommandList::SetViewPort(const ViewPort & viewport)
	{
		D3D12_VIEWPORT d3d12viewport = {};
		d3d12viewport.TopLeftX = viewport.left;
		d3d12viewport.TopLeftY = viewport.top;
		d3d12viewport.Width = viewport.width;
		d3d12viewport.Height = viewport.height;
		d3d12viewport.MinDepth = viewport.nearZ;
		d3d12viewport.MaxDepth = viewport.farZ;

		_cmdlist->RSSetViewports(1, &d3d12viewport);
	}

	void RHID3D12CommandList::SetScissorRect(const core::recti & rect)
	{
		D3D12_RECT d3d12rect = {};
		d3d12rect.left = rect.x;
		d3d12rect.top = rect.y;
		d3d12rect.right = rect.right();
		d3d12rect.bottom = rect.bottom();

		_cmdlist->RSSetScissorRects(1, &d3d12rect);
	}

	void RHID3D12CommandList::TransitionBarrier(RHIResource * resource, ResourceStates states)
	{
		static_cast<RHID3D12Resource *>(resource)->TransitionBarrier(this, states);
	}
	
	void RHID3D12CommandList::TransitionBarrier(RHIRenderTarget * rendertarget, ResourceStates states)
	{
		static_cast<RHIRenderTarget *>(rendertarget)->TransitionBarrier(this, states);
	}
	
	void RHID3D12CommandList::SetPipelineState(RHIPipelineState * pipelinestate)
	{
		auto d3d12pipelinestate = static_cast<RHID3D12PipelineState *>(pipelinestate);
		_cmdlist->SetPipelineState(d3d12pipelinestate->PipelineState());
		_cmdlist->SetGraphicsRootSignature(d3d12pipelinestate->RootSignature());
	}
	
	void RHID3D12CommandList::SetResourcePacket(RHIResourcePacket * packet)
	{
		ID3D12DescriptorHeap * heaps[1] = { static_cast<RHID3D12ResourcePacket *>(packet)->DescriptorHeap()};
		_cmdlist->SetDescriptorHeaps(1, heaps);
	}

	void RHID3D12CommandList::SetGraphicsResourceView(uint32_t index, RHIResourceView * view)
	{
		_cmdlist->SetGraphicsRootDescriptorTable(index, static_cast<RHID3D12ResourceView *>(view)->GPUDescriptorHandle());
	}
	
	void RHID3D12CommandList::IASetVertexBuffer(RHIResource * resource, uint32_t stride, uint32_t size)
	{
		D3D12_VERTEX_BUFFER_VIEW vbv = {};
		vbv.BufferLocation = static_cast<RHID3D12Resource *>(resource)->GPUVirtualAddress();
		vbv.StrideInBytes = stride;
		vbv.SizeInBytes = size;
		_cmdlist->IASetVertexBuffers(0, 1, &vbv);
	}
	
	void RHID3D12CommandList::IASetIndexBuffer(RHIResource * resource, uint32_t stride, uint32_t size)
	{
		D3D12_INDEX_BUFFER_VIEW vbv = {};
		vbv.BufferLocation = static_cast<RHID3D12Resource *>(resource)->GPUVirtualAddress();
		vbv.Format = stride == 4 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
		vbv.SizeInBytes = size;
		_cmdlist->IASetIndexBuffer(&vbv);
	}

	void RHID3D12CommandList::IASetTopologyType(Topology topology)
	{
		_cmdlist->IASetPrimitiveTopology(FromTopology(topology));
	}

	void RHID3D12CommandList::DrawInstanced(uint32_t nvertices, uint32_t ninstance, uint32_t ivertexbase, uint32_t iinstancebase)
	{
		_cmdlist->DrawInstanced(nvertices, ninstance, ivertexbase, iinstancebase);
	}

	void RHID3D12CommandList::DrawIndexedInstanced(uint32_t nindices, uint32_t ninstance, uint32_t iindexbase, uint32_t ivertexbase, uint32_t iinstancebase)
	{
		_cmdlist->DrawIndexedInstanced(nindices, ninstance, iindexbase, ivertexbase, iinstancebase);
	}

	void RHID3D12CommandList::CopyResource(RHIResource * dst, RHIResource * src)
	{
		auto device = _device->Inner();
		auto rhid3d12dst = static_cast<RHID3D12Resource *>(dst);
		auto rhid3d12src = static_cast<RHID3D12Resource *>(src);
		if (rhid3d12dst->Args().dimension == ResourceDimension::Texture2D)
		{
			D3D12_TEXTURE_COPY_LOCATION destLocation;
			destLocation.pResource = rhid3d12dst->Resource();
			destLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			destLocation.SubresourceIndex = 0;
			
			D3D12_RESOURCE_DESC descDest = rhid3d12dst->Resource()->GetDesc();
			D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout = {};
			UINT nrows = 0;
			UINT64 stride = 0;
			UINT64 nbtotal = 0;
			device->GetCopyableFootprints(&descDest, 0, 1, 0, &layout, &nrows, &stride, &nbtotal);
			D3D12_TEXTURE_COPY_LOCATION srcLocation;
			srcLocation.pResource = rhid3d12src->Resource();
			srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			srcLocation.PlacedFootprint = layout;
			_cmdlist->CopyTextureRegion(&destLocation, 0, 0, 0, &srcLocation, nullptr);
		}
		else
		{
			_cmdlist->CopyResource(static_cast<RHID3D12Resource *>(dst)->Resource(), static_cast<RHID3D12Resource *>(src)->Resource());
		}
	}
	
	void RHID3D12CommandList::CopyBuffer(RHIResource * dst, RHIResource * src)
	{
		auto size = static_cast<RHID3D12Resource *>(src)->Size().cx * static_cast<RHID3D12Resource *>(src)->Size().cy;
		_cmdlist->CopyBufferRegion(static_cast<RHID3D12Resource *>(dst)->Resource(), 0, static_cast<RHID3D12Resource *>(src)->Resource(), 0, size);
	}
}
