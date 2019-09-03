#pragma once

#include "RHI/RHI.h"
#include "RHID3D11Core.h"
#include "RHID3D11Device.h"
#include "RHID3D11RenderTarget.h"

namespace RHI::RHID3D11
{
	class RHID3D11RenderTargetHWND : public RHID3D11RenderTarget
	{
	public:
		RHID3D11RenderTargetHWND(RHID3D11Device * device) : _device(device) {}
		virtual ~RHID3D11RenderTargetHWND() = default;

		core::error Create(RHICommandQueue * cmdqueue, const RenderTargetArgs & args);
		void SetName(const std::u8string & name) override;

	public:
		void Begin() override;
		void Present(uint32_t sync) override;
		uint32_t FrameIndex() const override;
		
	public:
		ID3D11RenderTargetView * RenderTargetView() const override { return _rendertargetView.get(); }
		
	private:
		RHID3D11Device * _device = nullptr;
		RenderTargetArgs _args;

		core::comptr<IDXGISwapChain3> _swapchain;

		uint32_t _frameIndex = 0;

		core::comptr<ID3D11Resource> _backBuffer;
		core::comptr<ID3D11RenderTargetView> _rendertargetView;
	};
}
