#include "stdafx.h"
#include "RHID3D12CommandList.h"
#include "RHID3D12CommandAllocator.h"
#include "RHID3D12View.h"
#include "RHID3D12Resource.h"
#include "RHID3D12PipelineState.h"

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
		_rendertarget = static_cast<RHID3D12RenderTargetView *>(rendertarget);
		D3D12_CPU_DESCRIPTOR_HANDLE handle = _rendertarget->CPUDescriptorHandle();
		_cmdlist->OMSetRenderTargets(1, &handle, FALSE, nullptr);
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
	
	void RHID3D12CommandList::SetResourceViews(RHIResourceView ** views, uint32_t nviews)
	{
		ID3D12DescriptorHeap * heaps[64] = {};
		nviews = std::min< uint32_t>(nviews, 64);
		for (uint32_t iview = 0; iview < nviews; ++iview)
		{
			auto dp = static_cast<RHID3D12ResourceView *>(views[iview])->DescriptorHeap();
			heaps[iview] = static_cast<RHID3D12ResourceView *>(views[iview])->DescriptorHeap();
		}
		_cmdlist->SetDescriptorHeaps(nviews, heaps);
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
}
