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

		core::error Init(CommandType type, CommandQueueFlags flags);

	private:
		RHID3D12Device * _device = nullptr;
		win32::comptr<ID3D12CommandQueue> _queue;

		CommandType _type = CommandType::Direct;
		CommandQueueFlags _flags = nullptr;
	};
}
