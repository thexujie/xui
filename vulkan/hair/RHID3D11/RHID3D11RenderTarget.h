#pragma once

#include "RHI/RHI.h"
#include "RHID3D11Core.h"
#include "RHID3D11Device.h"

namespace RHI::RHID3D11
{
	class RHID3D11RenderTarget : public RHIRenderTarget
	{
	public:
		virtual ID3D11RenderTargetView * RenderTargetView() const = 0;
	};
}
