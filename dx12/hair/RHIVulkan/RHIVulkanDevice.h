#pragma once

#include "RHIVulkanCore.h"

namespace RHI::RHIVulkan
{
	class RHIVulkanDevice : public RHIDevice
	{
	public:
		RHIVulkanDevice(RHIVulkanFactory * factory) : _factory(factory) {}
		virtual ~RHIVulkanDevice() = default;

		core::error Create(VkPhysicalDevice adapter);

	public:
		RHIVulkanFactory * Factory() const { return _factory; }
		VkDevice Device() const { return _device; }
		VkPhysicalDevice Adapter() const { return _adapter; }
		uint32_t QueueFamilyIndex() const { return _queueFamilyIndex; }
		
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
		RHIVulkanFactory * _factory = nullptr;
		std::vector<VkExtensionProperties> _availableExtensions;
		VkPhysicalDevice _adapter;
		VkDevice _device;
		uint32_t _queueFamilyIndex = 0;
	};
}
