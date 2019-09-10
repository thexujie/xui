#pragma once

#include "RHICore.h"
#include "RHIResource.h"
#include "RHIDevice.h"
#include "RHIFence.h"
#include "RHICommandAllocator.h"
#include "RHICommandQueue.h"
#include "RHICommandList.h"
#include "RHICommandQueue.h"
#include "RHIRenderTarget.h"
#include "RHIPipelineState.h"
#include "RHIResourcePacket.h"

namespace RHI
{
	class RHIFactory
	{
	public:
		RHIFactory() = default;
		virtual ~RHIFactory() = default;

		virtual core::error Load() = 0;
		virtual std::vector<RHIAdapterDesc> AdapterDescs() const = 0;
		virtual std::shared_ptr<RHIDevice> CreateDevice(uint64_t id) const = 0;
	};
}
