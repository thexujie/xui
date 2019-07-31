#pragma once

#include "RHICore.h"
#include "RHIResource.h"

namespace RHI
{
	class RHICommandList;
	class RHIResourceView;

	class RHIRenderTarget : public RHIResource
	{
	public:
		RHIRenderTarget() = default;
		virtual ~RHIRenderTarget() = default;

	public:
		virtual RHIResourceView * CurrentRTV() const = 0;
		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual void Excute(RHICommandList * cmdlist) = 0;
		virtual void Present(uint32_t sync) = 0;
	};
}
