#pragma once

#include "RHICore.h"

namespace RHI
{
	class RHICommandList : public RHIDeviceObject
	{
	public:
		RHICommandList() = default;
		virtual ~RHICommandList() = default;

	public:
		virtual void Reset() = 0;
		virtual void Close() = 0;
		virtual void SetRenderTarget(class RHIRenderTarget * rendertarget) = 0;
		virtual void ClearRenderTarget(core::color color) = 0;
		virtual void SetViewPort(const ViewPort & viewport) = 0;
		virtual void SetScissorRect(const core::recti & rect) = 0;
		virtual void TransitionBarrier(RHIResource * resource, ResourceStates states) = 0;
	};
}
