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

		virtual void * Map() = 0;
		virtual void Unmap() = 0;
		
	public:
		ResourceStates States() const { return _states; }
	protected:
		ResourceStates _states = ResourceState::None;
	};
}
