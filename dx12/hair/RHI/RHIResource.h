#pragma once

#include "RHICore.h"

namespace RHI
{
	class RHIResource
	{
	public:
		RHIResource() = default;
		virtual ~RHIResource() = default;

		virtual void TransitionBarrier(class RHICommandList * cmdlist, ResourceStates states) = 0;
		ResourceStates States() const { return _states; }
	protected:
		ResourceStates _states = ResourceState::None;
	};

	class RHIResourceView
	{
	public:
		RHIResourceView() = default;
		virtual ~RHIResourceView() = default;
	};
}
