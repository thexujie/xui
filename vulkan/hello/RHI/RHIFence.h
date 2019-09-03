#pragma once

#include "RHICore.h"

namespace RHI
{
	class RHIFence : public RHIObject
	{
	public:
		virtual uint64_t CurrentFence() const = 0;
	};

}
