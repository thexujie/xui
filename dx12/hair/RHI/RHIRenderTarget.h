#pragma once

#include "RHICore.h"

namespace RHI
{
	class RHIRenderTarget : public RHIResource
	{
	public:
		RHIRenderTarget() = default;
		virtual ~RHIRenderTarget() = default;

	public:
		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual void Excute(RHICommandList * cmdlist) = 0;
		virtual void Present(uint32_t sync) = 0;
	};
}
