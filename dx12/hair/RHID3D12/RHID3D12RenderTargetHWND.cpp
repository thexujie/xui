#include "stdafx.h"
#include "RHID3D12RenderTargetHWND.h"
#include "RHID3D12CommandList.h"
#include "RHID3D12CommandQueue.h"
#include "RHID3D12Factory.h"

namespace RHI::RHID3D12
{
	core::error RHID3D12RenderTargetHWND::Create(RHICommandQueue * cmdqueue, const RenderTargetArgs & args)
	{
		if (!_device)
			return core::e_state;

		HRESULT hr = S_OK;

		auto factory = _device->Factory()->Factory();
		auto device = _device->Device();
		auto adapter = _device->Adapter();
		assert(device);
		assert(adapter);

		//core::comptr<IDXGIFactory3> factory;
		//hr = adapter->GetParent(__uuidof(IDXGIFactory3), factory.getvv());
		//if (FAILED(hr))
		//{
		//	core::war() << __FUNCTION__ " adapter->GetParent failed: " << win32::winerr_str(hr & 0xFFFF);
		//	return core::e_inner;
		//}

		auto d3d12cmdqueue = static_cast<RHID3D12CommandQueue *>(cmdqueue)->CommandQueue();

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = args.nbuffers;
		swapChainDesc.Width = args.size.cx;
		swapChainDesc.Height = args.size.cy;
		swapChainDesc.Format = FromFormat(args.format);
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = FromSwapEffect(args.swapeffect);
		swapChainDesc.SampleDesc.Count = args.MSAA;
		swapChainDesc.SampleDesc.Quality = args.MSAAQuality;

		core::comptr<IDXGISwapChain1> swapchain1;
		hr = factory->CreateSwapChainForHwnd(
			d3d12cmdqueue,
			(HWND)args.hwnd,
			&swapChainDesc,
			nullptr,
			nullptr,
			swapchain1.getpp()
		);
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " factory->CreateSwapChainForHwnd failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}
		hr = factory->MakeWindowAssociation((HWND)args.hwnd, DXGI_MWA_NO_ALT_ENTER);
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " dxgifactory->MakeWindowAssociation(DXGI_MWA_NO_ALT_ENTER) failed: " << win32::winerr_str(hr & 0xFFFF);
		}

		core::comptr<ID3D12DescriptorHeap> rtv_heap;
		std::vector<core::comptr<ID3D12Resource>> buffers(args.nbuffers);
		{
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			rtvHeapDesc.NumDescriptors = args.nbuffers;
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
			for (uint32_t ibuffer = 0; ibuffer < args.nbuffers; ibuffer++)
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
		_states.resize(args.nbuffers, ResourceState::Present);
		_args = args;
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

	uint32_t RHID3D12RenderTargetHWND::FrameIndex() const
	{
		return _swapchain->GetCurrentBackBufferIndex();
	}
}
