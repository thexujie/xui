#pragma once

#include "RHICore.h"
#include "RHIResource.h"

namespace RHI
{
	class RHICommandList;
	class RHIResourceView;

	class RHIRenderTarget : public RHIObject
	{
	public:
		RHIRenderTarget() = default;
		virtual ~RHIRenderTarget() = default;
		
	public:
		virtual void TransitionBarrier(RHICommandList * cmdlist, ResourceStates states) = 0;
		virtual RHIResourceView * CurrentRTV() const = 0;
		virtual void Begin() = 0;
		virtual void Present(uint32_t sync) = 0;
	};
}
