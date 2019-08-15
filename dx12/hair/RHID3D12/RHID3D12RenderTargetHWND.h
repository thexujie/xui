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

		core::error Create(RHICommandQueue * cmdqueue, const RenderTargetArgs & args);
		void SetName(const std::u8string & name) override;

	public:
		void Begin() override;
		void Present(uint32_t sync) override;
		uint32_t FrameIndex() const override;
		
	public:
		D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle(uint32_t index) override
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle = _heap->GetCPUDescriptorHandleForHeapStart();
			handle.ptr += _unit * index;
			return handle;
		}
		
		D3D12_GPU_DESCRIPTOR_HANDLE GPUDescriptorHandle(uint32_t index) override
		{
			D3D12_GPU_DESCRIPTOR_HANDLE handle = _heap->GetGPUDescriptorHandleForHeapStart();
			handle.ptr += _unit * index;
			return handle;
		}

		ID3D12Resource * Buffer(uint32_t index) const { return _buffers[index].get(); }
		ResourceStates State(uint32_t index) const { return _states[index]; }
		void SetState(uint32_t index, ResourceStates state) { _states[index] = state; }

	private:
		RHID3D12Device * _device = nullptr;
		RenderTargetArgs _args;

		core::comptr<IDXGISwapChain3> _swapchain;

		uint32_t _frameIndex = 0;

		core::comptr<ID3D12DescriptorHeap> _heap;
		uint32_t _unit = 0;
		std::vector<core::comptr<ID3D12Resource>> _buffers;
		std::vector<ResourceStates> _states;
	};
}
