#include "stdafx.h"
#include "RHIVulkanCommandAllocator.h"

namespace RHI::RHIVulkan
{
	core::error RHIVulkanCommandAllocator::Create(CommandType type, uint32_t count)
	{
		if (!_device)
			return core::e_state;

		VkCommandPoolCreateInfo poolDesc = {};
		poolDesc.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolDesc.pNext = NULL;
		poolDesc.queueFamilyIndex = _device->QueueFamilyIndex();
		poolDesc.flags = 0;

		VkCommandPool pool = nullptr;
		VkResult result = vkCreateCommandPool(_device->Device(), &poolDesc, NULL, &pool);
		assert(result == VK_SUCCESS);
		if (result)
		{
			core::err() << __FUNCTION__" vkCreateDevice failed, " << result;
			return core::e_inner;
		}

		_pool = pool;
		return core::ok;
	}

	void RHIVulkanCommandAllocator::Reset(uint32_t index)
	{
	}
	
	void RHIVulkanCommandAllocator::SetName(const std::u8string & name)
	{
	}
}
