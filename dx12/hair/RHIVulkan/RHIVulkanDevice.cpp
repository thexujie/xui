#include "stdafx.h"
#include "RHIVulkanDevice.h"
#include "RHIVulkanCommandAllocator.h"
#include "RHIVulkanCommandList.h"
#include "RHIVulkanRenderTargetHWND.h"
#include "RHIVulkanResource.h"
#include "RHIVulkanCommandQueue.h"

//#include "RHIVulkanCommandAllocator.h"
//#include "RHIVulkanFence.h"
//#include "RHIVulkanCommandQueue.h"
//#include "RHIVulkanCommandList.h"
//#include "RHIVulkanResource.h"
//#include "RHIVulkanResourcePacket.h"
//#include "RHIVulkanRenderTargetHWND.h"
//#include "RHIVulkanPipelineState.h"

namespace RHI::RHIVulkan
{
	core::error RHIVulkanDevice::Create(VkPhysicalDevice adapter)
	{
		if (!adapter)
			return core::e_args;

		uint32_t nextensions = 0;
		vkEnumerateDeviceExtensionProperties(adapter, nullptr, &nextensions, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(nextensions);
		vkEnumerateDeviceExtensionProperties(adapter, nullptr, &nextensions, availableExtensions.data());
		
		VkDeviceQueueCreateInfo queue_info = {};

		uint32_t nproperties = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(adapter, &nproperties, NULL);

		std::vector<VkQueueFamilyProperties> properties(nproperties);
		vkGetPhysicalDeviceQueueFamilyProperties(adapter, &nproperties, properties.data());

		bool found = false;
		for (unsigned int iproperty = 0; iproperty < nproperties; iproperty++) 
		{
			if (properties[iproperty].queueFlags & VK_QUEUE_GRAPHICS_BIT) 
			{
				queue_info.queueFamilyIndex = iproperty;
				found = true;
				break;
			}
		}
		assert(found);

		float queue_priorities[1] = { 0.0 };
		queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_info.pNext = NULL;
		queue_info.queueCount = 1;
		queue_info.pQueuePriorities = queue_priorities;

		std::vector<const char *> extensions =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		};

		
		VkDeviceCreateInfo device_info = {};
		device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		device_info.pNext = NULL;
		device_info.queueCreateInfoCount = 1;
		device_info.pQueueCreateInfos = &queue_info;
		device_info.enabledExtensionCount = extensions.size();
		device_info.ppEnabledExtensionNames = extensions.data();
		device_info.enabledLayerCount = 0;
		device_info.ppEnabledLayerNames = NULL;
		device_info.pEnabledFeatures = NULL;

		VkDevice device;
		VkResult res = vkCreateDevice(adapter, &device_info, NULL, &device);
		if (res)
		{
			core::err() << __FUNCTION__" vkCreateDevice failed, " << res;
			return core::e_inner;
		}

		_availableExtensions = availableExtensions;
		_adapter = adapter;
		_device = device;
		return core::ok;
	}

	std::shared_ptr<RHIFence> RHIVulkanDevice::CreateFence(FenceFlags flags) const
	{
		return {};
		//auto fence = std::make_shared<RHIVulkanFence>(const_cast<RHIVulkanDevice *>(this));
		//auto err = fence->Create(flags);
		//if (err)
		//	return nullptr;
		//return fence;
	}

	std::shared_ptr<RHICommandQueue> RHIVulkanDevice::CreateCommandQueue(CommandType type, CommandQueueFlags flags) const
	{
		auto queue = std::make_shared<RHIVulkanCommandQueue>(const_cast<RHIVulkanDevice *>(this));
		auto err = queue->Create(type, flags);
		if (err)
			return nullptr;
		return queue;
	}

	std::shared_ptr<RHICommandAllocator> RHIVulkanDevice::CreateCommandAllocator(CommandType type, uint32_t count) const
	{
		auto allocator = std::make_shared<RHIVulkanCommandAllocator>(const_cast<RHIVulkanDevice *>(this));
		auto err = allocator->Create(type, count);
		if (err)
			return nullptr;
		return allocator;
	}

	std::shared_ptr<RHICommandList> RHIVulkanDevice::CreateCommandList(CommandType type, RHICommandAllocator * allocator) const
	{
		auto list = std::make_shared<RHIVulkanCommandList>(const_cast<RHIVulkanDevice *>(this));
		auto err = list->Create(type, static_cast<RHIVulkanCommandAllocator *>(allocator));
		if (err)
			return nullptr;
		return list;
	}

	std::shared_ptr<RHIResource> RHIVulkanDevice::CreateResource(const ResourceArgs & args) const
	{
		auto resource = std::make_shared<RHIVulkanResource>(const_cast<RHIVulkanDevice *>(this));
		auto err = resource->Create(args);
		if (err)
			return nullptr;
		return resource;
	}

	std::shared_ptr<RHIResourcePacket> RHIVulkanDevice::CreateResourcePacket(const ResourcePacketArgs & args) const
	{
		return {};
	
		//auto packet = std::make_shared<RHIVulkanResourcePacket>(const_cast<RHIVulkanDevice *>(this));
		//auto err = packet->Create(args);
		//if (err)
		//	return nullptr;
		//return packet;
	}
	
	std::shared_ptr<RHIRenderTarget> RHIVulkanDevice::CreateRenderTargetForHWND(RHICommandQueue * cmdqueue, const RenderTargetArgs & args) const
	{
		auto rt = std::make_shared<RHIVulkanRenderTargetHWND>(const_cast<RHIVulkanDevice *>(this));
		auto err = rt->Create(cmdqueue, args);
		if (err)
			return nullptr;
		return rt;
	}

	std::shared_ptr<RHIPipelineState> RHIVulkanDevice::CreatePipelineState(const PipelineStateArgs & args) const
	{
		return {};
		//auto pso = std::make_shared<RHIVulkanPipelineState>(const_cast<RHIVulkanDevice *>(this));
		//auto err = pso->Create(args);
		//if (err)
		//	return nullptr;

		//return pso;
	}
}
