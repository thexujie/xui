#pragma once

#include "RHI/RHI.h"
#include "RHID3D12Core.h"
#include "RHID3D12Device.h"
#include "RHID3D12RenderTarget.h"

namespace RHI::RHID3D12
{
	class RHID3D12RenderTargetHWND : public RHID3D12RenderTarget
	{
	public:
		RHID3D12RenderTargetHWND(RHID3D12Device * device) : _device(device) {}
		virtual ~RHID3D12RenderTargetHWND() = default;

		core::error Create(RHICommandQueue * cmdqueue, const RenderTargetArgs & params);
		void SetName(const std::u8string & name);

	public:
		void TransitionBarrier(class RHICommandList * cmdlist, ResourceStates states) override;

		D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle() override
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle = _heap->GetCPUDescriptorHandleForHeapStart();
			handle.ptr += _unit * _frameIndex;
			return handle;
		}
		
		D3D12_GPU_DESCRIPTOR_HANDLE GPUDescriptorHandle() override
		{
			D3D12_GPU_DESCRIPTOR_HANDLE handle = _heap->GetGPUDescriptorHandleForHeapStart();
			handle.ptr += _unit * _frameIndex;
			return handle;
		}
	public:
		void Begin() override;
		void Present(uint32_t sync) override;

	private:
		RHID3D12Device * _device = nullptr;
		RenderTargetArgs _params;

		core::comptr<IDXGISwapChain3> _swapchain;

		ResourceStates _states = ResourceState::None;
		uint32_t _frameIndex = 0;

		core::comptr<ID3D12DescriptorHeap> _heap;
		uint32_t _unit = 0;
		std::vector<core::comptr<ID3D12Resource>> _buffers;


	};
}
