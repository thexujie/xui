#pragma once

#include "RHICore.h"

namespace RHI
{
	class RHICommandQueue;
	class RHICommandList;
	class RHIBuffer;
	class RHIRenderTarget;

	class RHIDevice
	{
	public:
		RHIDevice() = default;
		virtual ~RHIDevice() = default;

		RHIAdapterDesc Desc() const { return _desc; }

	public:
		virtual std::shared_ptr<RHICommandQueue> CreateCommandQueue(CommandType type, CommandQueueFlags flags) const = 0;
		virtual std::shared_ptr<RHICommandList> CreateCommandList(CommandType type) const = 0;
		virtual std::shared_ptr<RHIBuffer> CreateBuffer(const BufferParams & params) const = 0;
		virtual std::shared_ptr<RHIRenderTarget> CreateRenderTargetForHWND(const RenderTargetParams & params) const = 0;

	protected:
		RHIAdapterDesc _desc;
	};
}
