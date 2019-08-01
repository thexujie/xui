#pragma once

#include "RHICore.h"

namespace RHI
{
	class RHICommandQueue;
	class RHICommandAllocator;
	class RHICommandList;
	class RHIResource;
	class RHIResourceView;
	class RHIRenderTarget;
	class RHIPipelineState;

	class RHIDevice
	{
	public:
		RHIDevice() = default;
		virtual ~RHIDevice() = default;

		RHIAdapterDesc Desc() const { return _desc; }

	public:
		virtual std::shared_ptr<RHICommandQueue> CreateCommandQueue(CommandType type, CommandQueueFlags flags) const = 0;
		virtual std::shared_ptr<RHICommandAllocator> CreateCommandAllocator(CommandType type) const = 0;
		virtual std::shared_ptr<RHICommandList> CreateCommandList(CommandType type) const = 0;
		virtual std::shared_ptr<RHIResource> CreateResource(const ResourceArgs & args) const = 0;
		virtual std::shared_ptr<RHIResourceView> CreateShaderResourceView(const RHIResource * resource, const ShaderResourceViewArgs & args) const = 0;
		virtual std::shared_ptr<RHIResourceView> CreateConstBufferView(const RHIResource * resource, const ConstBufferViewArgs & args) const = 0;
		virtual std::shared_ptr<RHIRenderTarget> CreateRenderTargetForHWND(const RenderTargetArgs & args) const = 0;
		virtual std::shared_ptr<RHIPipelineState> CreatePipelineState(const PipelineStateArgs & args) const = 0;

	protected:
		RHIAdapterDesc _desc;
	};
}
