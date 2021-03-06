#pragma once

#include "RHICore.h"

namespace RHI
{
	class RHICommandAllocator: public RHIDeviceObject
	{
	public:
		RHICommandAllocator() = default;
		virtual ~RHICommandAllocator() = default;

	public:
		virtual void Reset(uint32_t index) = 0;
	};
}
