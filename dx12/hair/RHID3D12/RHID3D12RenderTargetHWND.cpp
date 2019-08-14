#include "stdafx.h"
#include "RHID3D12RenderTargetHWND.h"
#include "RHID3D12CommandList.h"
#include "RHID3D12CommandQueue.h"

namespace RHI::RHID3D12
{
	core::error RHID3D12RenderTargetHWND::Create(RHICommandQueue * cmdqueue, const RenderTargetArgs & params)
	{
		if (!_device)
			return core::e_state;

		HRESULT hr = S_OK;

		auto device = _device->Inner();
		auto adapter = _device->InnerAdapter();
		assert(device);
		assert(adapter);

		core::comptr<IDXGIFactory3> dxgifactory;
		hr = adapter->GetParent(__uuidof(IDXGIFactory3), dxgifactory.getvv());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " adapter->GetParent failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}

		auto d3d12cmdqueue = static_cast<RHID3D12CommandQueue *>(cmdqueue)->CommandQueue();

		RECT rcClient = {};
		GetClientRect((HWND)params.hwnd, &rcClient);

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = params.nbuffer;
		swapChainDesc.Width = rcClient.right - rcClient.left;
		swapChainDesc.Height = rcClient.bottom - rcClient.top;
		swapChainDesc.Format = FromFormat(params.format);
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = FromSwapEffect(params.swapeffect);
		swapChainDesc.SampleDesc.Count = params.MSAA;
		swapChainDesc.SampleDesc.Quality = params.MSAAQuality;

		core::comptr<IDXGISwapChain1> swapchain1;
		hr = dxgifactory->CreateSwapChainForHwnd(
			d3d12cmdqueue,
			(HWND)params.hwnd,
			&swapChainDesc,
			nullptr,
			nullptr,
			swapchain1.getpp()
		);
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " dxgifactory->CreateSwapChainForHwnd failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}
		hr = dxgifactory->MakeWindowAssociation((HWND)params.hwnd, DXGI_MWA_NO_ALT_ENTER);
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " dxgifactory->MakeWindowAssociation(DXGI_MWA_NO_ALT_ENTER) failed: " << win32::winerr_str(hr & 0xFFFF);
		}

		core::comptr<ID3D12DescriptorHeap> rtv_heap;
		std::vector<core::comptr<ID3D12Resource>> buffers(params.nbuffer);
		{
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			rtvHeapDesc.NumDescriptors = params.nbuffer;
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			hr = device->CreateDescriptorHeap(&rtvHeapDesc, __uuidof(ID3D12DescriptorHeap), rtv_heap.getvv());
			if (FAILED(hr))
			{
				core::war() << __FUNCTION__ " device->CreateDescriptorHeap failed: " << win32::winerr_str(hr & 0xFFFF);
				return core::e_inner;
			}
			SetD3D12ObjectName(rtv_heap.get(), L"rtv_heap");

			D3D12_CPU_DESCRIPTOR_HANDLE handle(rtv_heap->GetCPUDescriptorHandleForHeapStart());
			uint32_t rtv_desc_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			for (uint32_t ibuffer = 0; ibuffer < params.nbuffer; ibuffer++)
			{
				hr = swapchain1->GetBuffer(ibuffer, __uuidof(ID3D12Resource), buffers[ibuffer].getvv());
				if (FAILED(hr))
				{
					core::war() << __FUNCTION__ " swapchain1->GetBuffer failed: " << win32::winerr_str(hr & 0xFFFF);
					return core::e_inner;
				}

				device->CreateRenderTargetView(buffers[ibuffer].get(), nullptr, handle);
				handle.ptr += rtv_desc_size;
			}
		}

		//---------------------------------------------------------------
		_states = ResourceState::Present;
		_params = params;
		_swapchain = swapchain1.as<IDXGISwapChain3>();
		_frameIndex = _swapchain->GetCurrentBackBufferIndex();
		_heap = rtv_heap;
		_unit = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		_buffers = buffers;

		return core::ok;
	}

	void RHID3D12RenderTargetHWND::SetName(const std::u8string & name)
	{
		SetD3D12ObjectName(_heap.get(), name + u8"._heap");
	}

	void RHID3D12RenderTargetHWND::TransitionBarrier(class RHICommandList * cmdlist, ResourceStates states)
	{
		if (!cmdlist)
			return;

		auto d3d12cmdlist = reinterpret_cast<RHID3D12CommandList *>(cmdlist)->Ptr();
		assert(d3d12cmdlist);

		D3D12_RESOURCE_BARRIER barrier;
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = _buffers[_frameIndex].get();
		barrier.Transition.StateBefore = FromResourceStates(_states);
		barrier.Transition.StateAfter = FromResourceStates(states);
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		d3d12cmdlist->ResourceBarrier(1, &barrier);
		_states = states;
	}

	void RHID3D12RenderTargetHWND::Begin()
	{
		_frameIndex = _swapchain->GetCurrentBackBufferIndex();
	}

	void RHID3D12RenderTargetHWND::Present(uint32_t sync)
	{
		if (!_swapchain)
			return ;

		_swapchain->Present(sync, 0);
	}
}
