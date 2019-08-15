#include "stdafx.h"
#include "RHID3D11RenderTargetHWND.h"
#include "RHID3D11CommandList.h"
#include "RHID3D11CommandQueue.h"

namespace RHI::RHID3D11
{
	core::error RHID3D11RenderTargetHWND::Create(RHICommandQueue * cmdqueue, const RenderTargetArgs & params)
	{
		if (!_device)
			return core::e_state;

		HRESULT hr = S_OK;

		auto device = _device->Device();
		auto adapter = _device->Adapter();
		assert(device);
		assert(adapter);

		core::comptr<IDXGIFactory3> dxgifactory;
		hr = adapter->GetParent(__uuidof(IDXGIFactory3), dxgifactory.getvv());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " adapter->GetParent failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}

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
			device,
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

		core::comptr<ID3D11Resource> backBuffer;
		hr = swapchain1->GetBuffer(0, __uuidof(ID3D11Texture2D), backBuffer.getvv());
		assert(SUCCEEDED(hr));
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " dxgifactory->MakeWindowAssociation(DXGI_MWA_NO_ALT_ENTER) failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;
		core::comptr<ID3D11RenderTargetView> rendertargetView;
		hr = device->CreateRenderTargetView(backBuffer.get(), &rtvDesc, rendertargetView.getpp());
		assert(SUCCEEDED(hr));
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " device->CreateRenderTargetView failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}

		//---------------------------------------------------------------
		_params = params;
		_swapchain = swapchain1.as<IDXGISwapChain3>();
		_frameIndex = _swapchain->GetCurrentBackBufferIndex();
		_backBuffer = backBuffer;
		_rendertargetView = rendertargetView;
		return core::ok;
	}

	void RHID3D11RenderTargetHWND::SetName(const std::u8string & name)
	{
		SetD3D11ObjectName(_swapchain.get(), name + u8"._swapchain");
		SetD3D11ObjectName(_backBuffer.get(), name + u8"._backBuffer");
	}

	void RHID3D11RenderTargetHWND::Begin()
	{
		_frameIndex = _swapchain->GetCurrentBackBufferIndex();
	}

	void RHID3D11RenderTargetHWND::Present(uint32_t sync)
	{
		if (!_swapchain)
			return ;

		_swapchain->Present(sync, 0);
	}
	
	uint32_t RHID3D11RenderTargetHWND::FrameIndex() const
	{
		return _swapchain->GetCurrentBackBufferIndex();
	}
}
