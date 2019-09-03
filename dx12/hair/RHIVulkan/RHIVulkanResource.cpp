#include "stdafx.h"
#include "RHIVulkanResource.h"
#include "RHIVulkanCommandList.h"

namespace RHI::RHIVulkan
{
	core::error RHIVulkanResource::Create(const ResourceArgs & args)
	{
		if (!_device)
			return core::e_state;

		HRESULT hr = S_OK;

		auto device = _device->Device();
		auto adapter = _device->Adapter();
		assert(device);
		assert(adapter);

		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.pNext = NULL;
		if (args.heap.type == HeapType::Upload)
			bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		else
			bufferInfo.usage = FromResourceUsages(args.usages);
		bufferInfo.size = args.size.cx * args.size.cy;
		bufferInfo.queueFamilyIndexCount = 0;
		bufferInfo.pQueueFamilyIndices = NULL;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferInfo.flags = 0;
		VkBuffer buffer = nullptr;
		auto result = vkCreateBuffer(device, &bufferInfo, NULL, &buffer);
		assert(result == VK_SUCCESS);
		if (result)
		{
			core::err() << __FUNCTION__" vkCreateBuffer failed, " << result;
			return core::e_inner;
		}

		VkMemoryRequirements mem_reqs;
		vkGetBufferMemoryRequirements(device, buffer, &mem_reqs);

		VkMemoryAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.pNext = NULL;
		alloc_info.memoryTypeIndex = 0;

		alloc_info.allocationSize = mem_reqs.size;
		//pass = memory_type_from_properties(info, mem_reqs.memoryTypeBits,
		//	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		//	&alloc_info.memoryTypeIndex);
		//
		
		VkDeviceMemory memory = nullptr;
		result = vkAllocateMemory(device, &alloc_info, NULL, &memory);
		assert(result == VK_SUCCESS);

		uint8_t * pData;
		result = vkMapMemory(device, memory, 0, mem_reqs.size, 0, (void **)& pData);
		assert(result == VK_SUCCESS);

		//memcpy(pData, &info.MVP, sizeof(info.MVP));

		vkUnmapMemory(device, memory);

		result = vkBindBufferMemory(device, buffer, memory, 0);
		assert(result == VK_SUCCESS);
		
		_args = args;
		_buffer = buffer;
		return core::ok;
	}

	void RHIVulkanResource::SetName(const std::u8string & name)
	{
		//SetD3D11ObjectName(_resource.get(), name);
	}
}
