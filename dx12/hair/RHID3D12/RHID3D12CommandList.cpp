#include "stdafx.h"
#include "RHID3D12CommandList.h"
#include "RHID3D12RenderTarget.h"

namespace RHI::RHID3D12
{
	core::error RHID3D12CommandList::Init(CommandType type)
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

		hr = device->CreateCommandList(0, FromCommandType(type), cmdallocator.get(), nullptr, __uuidof(ID3D12GraphicsCommandList), cmdlist.getvv());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " device->CreateCommandList failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}

		cmdlist->Close();
		_cmdallocator = cmdallocator;
		_cmdlist = cmdlist;
		return core::ok;
	}

	void RHID3D12CommandList::Reset()
	{
		_cmdallocator->Reset();
		_cmdlist->Reset(_cmdallocator.get(), nullptr);
	}

	void RHID3D12CommandList::Close()
	{
		_cmdlist->Close();
	}

	void RHID3D12CommandList::SetRenderTarget(RHIRenderTarget * rendertarget)
	{
		_rendertarget = static_cast<RHID3D12RenderTarget *>(rendertarget);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = { _rendertarget->InnerCurrentHeapPointer() };
		_cmdlist->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
	}

	void RHID3D12CommandList::ClearRenderTarget(core::color color)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = { _rendertarget->InnerCurrentHeapPointer() };
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
		resource->TransitionBarrier(this, states);
	}
}
