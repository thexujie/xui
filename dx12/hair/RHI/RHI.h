#pragma once

#include "core/core.h"
#include "drawing/drawing.h"
#include "RHICore.h"
#include "RHIDevice.h"
#include "RHICommandList.h"
#include "RHICommandQueue.h"
#include "RHIRenderTarget.h"
#include "RHIBuffer.h"

namespace RHI
{
	class RHI
	{
	public:
		RHI() = default;
		virtual ~RHI() = default;

		virtual std::vector<RHIAdapterDesc> AdapterDescs() const = 0;
		virtual std::shared_ptr<RHIDevice> CreateDevice(const std::u8string & uri) const = 0;
	};
}
