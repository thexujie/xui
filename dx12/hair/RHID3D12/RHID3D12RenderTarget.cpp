#include "stdafx.h"
#include "RHID3D12RenderTarget.h"
#include "RHID3D12CommandList.h"

namespace RHI::RHID3D12
{
	core::error RHID3D12RenderTarget::Init(const RenderTargetParams & params)
	{
		if (!_device)
			return core::e_state;

		HRESULT hr = S_OK;

		auto device = _device->Inner();
		auto adapter = _device->InnerAdapter();
		assert(device);
		assert(adapter);

		win32::comptr<IDXGIFactory3> dxgifactory;
		hr = adapter->GetParent(__uuidof(IDXGIFactory3), dxgifactory.getvv());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " adapter->GetParent failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}

		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		if (params.queueFlags.any(CommandQueueFlag::DisableGPUTimeout))
			queueDesc.Flags |= D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT;
		win32::comptr<ID3D12CommandQueue> queue;
		hr = device->CreateCommandQueue(&queueDesc, __uuidof(ID3D12CommandQueue), queue.getvv());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " CreateCommandQueue failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}

		RECT rcClient = {};
		GetClientRect((HWND)params.hwnd, &rcClient);

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = params.nbuffer;
		swapChainDesc.Width = rcClient.right - rcClient.left;
		swapChainDesc.Height = rcClient.bottom - rcClient.top;
		swapChainDesc.Format = win32::DXGI::FromPixelFormat(params.format);
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = FromSwapEffect(params.swapeffect);
		swapChainDesc.SampleDesc.Count = params.MSAA;
		swapChainDesc.SampleDesc.Quality = params.MSAAQuality;

		win32::comptr<IDXGISwapChain1> swapchain1;
		hr = dxgifactory->CreateSwapChainForHwnd(
			queue.get(),
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

		win32::comptr<ID3D12DescriptorHeap> rtv_heapdesc;
		std::vector<win32::comptr<ID3D12Resource>> rendertargets(params.nbuffer);
		{
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			rtvHeapDesc.NumDescriptors = params.nbuffer;
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			hr = device->CreateDescriptorHeap(&rtvHeapDesc, __uuidof(ID3D12DescriptorHeap), rtv_heapdesc.getvv());
			if (FAILED(hr))
			{
				core::war() << __FUNCTION__ " device->CreateDescriptorHeap failed: " << win32::winerr_str(hr & 0xFFFF);
				return core::e_inner;
			}

			D3D12_CPU_DESCRIPTOR_HANDLE handle(rtv_heapdesc->GetCPUDescriptorHandleForHeapStart());

			uint32_t rtv_desc_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			for (uint32_t ibuffer = 0; ibuffer < params.nbuffer; ibuffer++)
			{
				hr = swapchain1->GetBuffer(ibuffer, __uuidof(ID3D12Resource), rendertargets[ibuffer].getvv());
				if (FAILED(hr))
				{
					core::war() << __FUNCTION__ " swapchain1->GetBuffer failed: " << win32::winerr_str(hr & 0xFFFF);
					return core::e_inner;
				}

				device->CreateRenderTargetView(rendertargets[ibuffer].get(), nullptr, handle);
				handle.ptr += rtv_desc_size;
			}
		}

		hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), _fence.getvv());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " device->CreateFence failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}


		//---------------------------------------------------------------
		_states = ResourceState::Present;
		_params = params;
		_queue = queue;
		_swapchain = swapchain1.as<IDXGISwapChain3>();
		_frameIndex = _swapchain->GetCurrentBackBufferIndex();
		_rtv_heapdesc = rtv_heapdesc;
		_rtv_desc_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		_rendertargets = rendertargets;
		_fenceValue = 1;
		_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		return core::ok;
	}

	void RHID3D12RenderTarget::TransitionBarrier(class RHICommandList * cmdlist, ResourceStates states)
	{
		if (!cmdlist)
			return;

		auto d3d12cmdlist = reinterpret_cast<RHID3D12CommandList *>(cmdlist)->Ptr();
		assert(d3d12cmdlist);

		D3D12_RESOURCE_BARRIER barrier;
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = _rendertargets[_frameIndex].get();
		barrier.Transition.StateBefore = FromResourceStates(_states);
		barrier.Transition.StateAfter = FromResourceStates(states);
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		d3d12cmdlist->ResourceBarrier(1, &barrier);
		_states = states;
	}

	void RHID3D12RenderTarget::Begin()
	{
		_frameIndex = _swapchain->GetCurrentBackBufferIndex();
	}

	void RHID3D12RenderTarget::End()
	{
		HRESULT hr = S_OK;
		const UINT64 fence = _fenceValue;
		hr = _queue->Signal(_fence.get(), _fenceValue);
		win32::throw_if_failed(hr);
		_fenceValue++;

		if (_fence->GetCompletedValue() < fence)
		{
			hr = _fence->SetEventOnCompletion(fence, _fenceEvent);
			win32::throw_if_failed(hr);
			WaitForSingleObject(_fenceEvent, INFINITE);
		}
	}

	void RHID3D12RenderTarget::Excute(RHICommandList * cmdlist)
	{
		ID3D12CommandList * cmdlists[] = { reinterpret_cast<RHID3D12CommandList *>(cmdlist)->Ptr() };
		_queue->ExecuteCommandLists(1, cmdlists);
	}

	void RHID3D12RenderTarget::Present(uint32_t sync)
	{
		if (!_swapchain)
			return ;

		_swapchain->Present(sync, 0);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE RHID3D12RenderTarget::InnerCurrentHeapPointer() const
	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(_rtv_heapdesc->GetCPUDescriptorHandleForHeapStart());
		rtvHandle.ptr += _frameIndex * _rtv_desc_size;
		return rtvHandle;
	}
}
