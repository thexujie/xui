#pragma once

#include "RHID3D11Core.h"

namespace RHI::RHID3D11
{
	class RHID3D11Device : public RHIDevice
	{
	public:
		RHID3D11Device() = default;
		virtual ~RHID3D11Device() = default;

		core::error Create(core::comptr<IDXGIAdapter1> adapter);

		ID3D11Device * Device() const { return _device.get(); }
		IDXGIAdapter1 * Adapter() const { return _adapter.get(); }

	public:
		std::shared_ptr<RHIFence> CreateFence(FenceFlags flags) const override;
		std::shared_ptr<RHICommandQueue> CreateCommandQueue(CommandType type, CommandQueueFlags flags) const override;
		std::shared_ptr<RHICommandAllocator> CreateCommandAllocator(CommandType type, uint32_t count) const override;
		std::shared_ptr<RHICommandList> CreateCommandList(CommandType type, RHICommandAllocator * allocator) const override;
		std::shared_ptr<RHIResource> CreateResource(const ResourceArgs & args) const override;
		std::shared_ptr<RHIResourcePacket> CreateResourcePacket(const ResourcePacketArgs & args) const override;
		std::shared_ptr<RHIRenderTarget> CreateRenderTargetForHWND(RHICommandQueue * cmdqueue, const RenderTargetArgs & args) const override;
		std::shared_ptr<RHIPipelineState> CreatePipelineState(const PipelineStateArgs & args) const override;

	private:
		core::comptr<IDXGIAdapter1> _adapter;
		core::comptr<ID3D11Device> _device;
	};
}
