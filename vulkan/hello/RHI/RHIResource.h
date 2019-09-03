#pragma once

#include "RHICore.h"

namespace RHI
{
	class RHIResource : public RHIObject
	{
	public:
		RHIResource() = default;

	public:
		virtual void * Data() = 0;
		virtual const void * Data() const = 0;
		virtual core::sizeu Size() const = 0;
	};
}
