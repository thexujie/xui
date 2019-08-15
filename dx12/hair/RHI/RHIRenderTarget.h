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
		virtual void Begin() = 0;
		virtual void Present(uint32_t sync) = 0;
		virtual uint32_t FrameIndex() const = 0;
	};
}
