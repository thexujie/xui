#pragma once

#include "RHI/RHI.h"
#include "RHID3D12Core.h"
#include "RHID3D12Device.h"

namespace RHI::RHID3D12
{
	class RHID3D12RenderTarget : public RHIRenderTarget
	{
	public:
		RHID3D12RenderTarget(RHID3D12Device * device) : _device(device) {}
		virtual ~RHID3D12RenderTarget() = default;

		core::error Init(const RenderTargetParams & params);

	public:
		void TransitionBarrier(class RHICommandList * cmdlist, ResourceStates states) override;

	public:
		void Begin();
		void End();
		void Excute(RHICommandList * cmdlist);

		void Present(uint32_t sync) override;

	public:
		D3D12_CPU_DESCRIPTOR_HANDLE InnerCurrentHeapPointer() const;

	public:
		ID3D12DescriptorHeap * RtvHeapDescPtr() const { return _rtv_heapdesc.get(); }
		uint32_t FrameIndex() const { return _frameIndex; }
		uint32_t RtvDescSize() const { return _rtv_desc_size; }

	private:
		RHID3D12Device * _device = nullptr;
		RenderTargetParams _params;

		win32::comptr<ID3D12CommandQueue> _queue;
		win32::comptr<IDXGISwapChain3> _swapchain;

		uint32_t _frameIndex = 0;
		uint64_t _fenceValue = 0;
		HANDLE _fenceEvent = NULL;

		win32::comptr<ID3D12DescriptorHeap> _rtv_heapdesc;
		uint32_t _rtv_desc_size = 0;
		std::vector<win32::comptr<ID3D12Resource>> _rendertargets;

		win32::comptr<ID3D12Fence> _fence;
	};
}
