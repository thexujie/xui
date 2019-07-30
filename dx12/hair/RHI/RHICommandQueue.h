#pragma once

#include "RHICore.h"

namespace RHI
{
	class RHICommandQueue : public RHIDeviceObject
	{
	public:
		RHICommandQueue() = default;
		virtual ~RHICommandQueue() = default;
	};
}


