#pragma once

#include "RHICore.h"

namespace RHI
{
	class RHIBuffer: public RHIResource
	{
	public:
		RHIBuffer() = default;
		virtual ~RHIBuffer() = default;
	};
}
