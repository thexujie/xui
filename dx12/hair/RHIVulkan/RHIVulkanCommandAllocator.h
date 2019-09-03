#pragma once

#include "RHI/RHI.h"
#include "RHIVulkanDevice.h"

namespace RHI::RHIVulkan
{
	class RHIVulkanCommandAllocator : public RHICommandAllocator
	{
	public:
		RHIVulkanCommandAllocator(RHIVulkanDevice * device) : _device(device) {}
		virtual ~RHIVulkanCommandAllocator() = default;

		core::error Create(CommandType type, uint32_t count);
		void SetName(const std::u8string & name) override;

	public:
		void Reset(uint32_t index) override;

	public:
		VkCommandPool CommandPool() const { return _pool; }
		
	private:
		RHIVulkanDevice * _device = nullptr;
		VkCommandPool _pool = nullptr;
	};
}
