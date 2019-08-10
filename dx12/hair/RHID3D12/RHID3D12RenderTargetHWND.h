#pragma once

#include "RHI/RHI.h"
#include "RHID3D12Core.h"
#include "RHID3D12Device.h"
#include "RHID3D12ResourceView.h"

namespace RHI::RHID3D12
{
	class RHID3D12RenderTargetHWND : public RHIRenderTarget
	{
	public:
		RHID3D12RenderTargetHWND(RHID3D12Device * device) : _device(device) {}
		virtual ~RHID3D12RenderTargetHWND() = default;

		core::error Create(RHICommandQueue * cmdqueue, const RenderTargetArgs & params);

	public:
		void TransitionBarrier(class RHICommandList * cmdlist, ResourceStates states) override;
		RHIResourceView * CurrentRTV() const override { return _views[_frameIndex].get(); }

	public:
		void Begin() override;
		void Present(uint32_t sync) override;

	private:
		RHID3D12Device * _device = nullptr;
		RenderTargetArgs _params;

		win32::comptr<IDXGISwapChain3> _swapchain;

		ResourceStates _states = ResourceState::None;
		uint32_t _frameIndex = 0;

		win32::comptr<ID3D12DescriptorHeap> _rtv_heap;
		uint32_t _rtv_size = 0;
		std::vector<win32::comptr<ID3D12Resource>> _buffers;
		std::vector<std::shared_ptr<RHID3D12ResourceView>> _views;


	};
}