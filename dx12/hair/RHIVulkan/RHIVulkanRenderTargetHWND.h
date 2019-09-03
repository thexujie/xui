#pragma once

#include "RHI/RHI.h"
#include "RHIVulkanCore.h"
#include "RHIVulkanDevice.h"
#include "RHIVulkanRenderTarget.h"

namespace RHI::RHIVulkan
{
	class RHIVulkanRenderTargetHWND : public RHIVulkanRenderTarget
	{
	public:
		RHIVulkanRenderTargetHWND(RHIVulkanDevice * device) : _device(device) {}
		virtual ~RHIVulkanRenderTargetHWND() = default;

		core::error Create(RHICommandQueue * cmdqueue, const RenderTargetArgs & args);
		void SetName(const std::u8string & name) override;

	public:
		void Begin() override;
		void Present(uint32_t sync) override;
		uint32_t FrameIndex() const override;
		
	private:
		RHIVulkanDevice * _device = nullptr;
		RenderTargetArgs _args;

		VkSurfaceKHR _surface = nullptr;
		VkSwapchainKHR _swapchain = nullptr;
		std::vector<VkImage> _buffers;
		std::vector<VkImageView> _views;
		uint32_t _frameIndex = 0;
	};
}
