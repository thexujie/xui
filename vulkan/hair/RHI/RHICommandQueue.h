#pragma once

#include "RHICore.h"

namespace RHI
{
	class RHIFence;
	class RHICommandList;

	class RHICommandQueue : public RHIDeviceObject
	{
	public:
		RHICommandQueue() = default;
		virtual ~RHICommandQueue() = default;

	public:
		virtual void Excute(RHICommandList * cmdlist) = 0;
		virtual void Signal(RHIFence * fence, uint64_t signal) = 0;
		virtual void Wait(RHIFence * fence, uint64_t value) = 0;
		virtual void Fence(RHIFence * fence, uint64_t value) = 0;
		virtual void SignalAndFence(RHIFence * fence, uint64_t signal, uint64_t value) = 0;
	};
}


