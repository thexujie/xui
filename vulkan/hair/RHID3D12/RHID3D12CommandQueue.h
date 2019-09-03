#pragma once

#include "RHI/RHI.h"
#include "RHI/RHICommandQueue.h"
#include "RHID3D12Device.h"

namespace RHI::RHID3D12
{
	class RHID3D12CommandQueue : public RHICommandQueue
	{
	public:
		RHID3D12CommandQueue(RHID3D12Device * device) : _device(device) {}
		virtual ~RHID3D12CommandQueue() = default;

		core::error Create(CommandType type, CommandQueueFlags flags);
		void SetName(const std::u8string & name) override;

	public:
		void Excute(RHICommandList * cmdlist) override;
		void Signal(RHIFence * fence, uint64_t signal) override;
		void Wait(RHIFence * fence, uint64_t value) override;
		void Fence(RHIFence * fence, uint64_t value) override;
		void SignalAndFence(RHIFence * fence, uint64_t signal, uint64_t value) override;

	public:
		ID3D12CommandQueue * CommandQueue() const { return _cmdqueue.get(); }
		
	private:
		RHID3D12Device * _device = nullptr;
		core::comptr<ID3D12CommandQueue> _cmdqueue;
		CommandType _type = CommandType::Direct;
		CommandQueueFlags _flags = nullptr;
		HANDLE _fenceEvent = NULL;
	};
}
