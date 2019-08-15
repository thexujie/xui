#pragma once

#include "RHICore.h"

namespace RHI
{
	class RHICommandList;

	class RHICommandQueue : public RHIDeviceObject
	{
	public:
		RHICommandQueue() = default;
		virtual ~RHICommandQueue() = default;

	public:
		virtual void Excute(RHICommandList * cmdlist) = 0;
		virtual void Fence(uint64_t signal, uint64_t fence) = 0;
	};
}


