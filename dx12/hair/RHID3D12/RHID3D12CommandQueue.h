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
		void SetName(const std::u8string & name);

	public:
		void Excute(RHICommandList * cmdlist);
		void Wait() override;

	public:
		ID3D12CommandQueue * CommandQueue() const { return _cmdqueue.get(); }
		
	private:
		RHID3D12Device * _device = nullptr;
		win32::comptr<ID3D12CommandQueue> _cmdqueue;
		win32::comptr<ID3D12Fence> _fence;

		CommandType _type = CommandType::Direct;
		CommandQueueFlags _flags = nullptr;

		uint64_t _fenceValue = 0;
		HANDLE _fenceEvent = NULL;
	};
}
