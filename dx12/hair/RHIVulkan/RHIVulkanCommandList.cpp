#include "stdafx.h"
#include "RHIVulkanCommandList.h"
#include "RHIVulkanCommandAllocator.h"
//#include "RHIVulkanResource.h"
//#include "RHIVulkanRenderTarget.h"
//#include "RHIVulkanPipelineState.h"
//#include "RHIVulkanResourcePacket.h"

namespace RHI::RHIVulkan
{
	core::error RHIVulkanCommandList::Create(CommandType type, RHIVulkanCommandAllocator * allocator)
	{
		if (!_device)
			return core::e_state;

		auto device = _device->Device();
		auto adapter = _device->Adapter();
		assert(device);
		assert(adapter);

		VkCommandBufferAllocateInfo cmd = {};
		cmd.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmd.pNext = NULL;
		cmd.commandPool = allocator->CommandPool();
		cmd.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cmd.commandBufferCount = 1;

		VkCommandBuffer commandBuffer = nullptr;
		auto result = vkAllocateCommandBuffers(_device->Device(), &cmd, &commandBuffer);
		assert(result == VK_SUCCESS);
		if (result)
		{
			core::err() << __FUNCTION__" vkAllocateCommandBuffers failed, " << result;
			return core::e_inner;
		}

		_allocator = allocator;
		_commandBuffer = commandBuffer;
		return core::ok;
	}

	void RHIVulkanCommandList::SetName(const std::u8string & name)
	{
	}

	void RHIVulkanCommandList::Reset(RHICommandAllocator * allocator, uint32_t index)
	{
		//_commandList.reset();
		//vkBeginCommandBuffer(_commandBuffer£©£»
	}

	void RHIVulkanCommandList::Close()
	{
	}

	void RHIVulkanCommandList::SetRenderTarget(RHIRenderTarget * rendertarget, uint32_t index)
	{
	}

	void RHIVulkanCommandList::ClearRenderTarget(RHIRenderTarget * rendertarget, uint32_t index, core::color color)
	{
	}

	void RHIVulkanCommandList::SetViewPort(const ViewPort & viewport)
	{
	}

	void RHIVulkanCommandList::SetScissorRect(const core::recti & rect)
	{
	}

	void RHIVulkanCommandList::TransitionBarrier(RHIResource * resource, ResourceStates states)
	{
		//static_cast<RHID3D12Resource *>(resource)->TransitionBarrier(this, states);
	}
	
	void RHIVulkanCommandList::TransitionBarrier(RHIRenderTarget * rendertarget, uint32_t index, ResourceStates states)
	{
		//static_cast<RHIRenderTarget *>(rendertarget)->TransitionBarrier(this, states);
	}
	
	void RHIVulkanCommandList::SetPipelineState(RHIPipelineState * pipelinestate)
	{
	}
	
	void RHIVulkanCommandList::SetResourcePacket(RHIResourcePacket * packet)
	{
	}

	void RHIVulkanCommandList::SetGraphicsResources(uint32_t index, uint32_t packetoffset)
	{
	}
	
	void RHIVulkanCommandList::IASetVertexBuffer(RHIResource * resource, uint32_t stride, uint32_t size)
	{
	}
	
	void RHIVulkanCommandList::IASetIndexBuffer(RHIResource * resource, uint32_t stride, uint32_t size)
	{
	}

	void RHIVulkanCommandList::IASetTopologyType(Topology topology)
	{
	}

	void RHIVulkanCommandList::DrawInstanced(uint32_t nvertices, uint32_t ninstance, uint32_t ivertexbase, uint32_t iinstancebase)
	{
	}

	void RHIVulkanCommandList::DrawIndexedInstanced(uint32_t nindices, uint32_t ninstance, uint32_t iindexbase, uint32_t ivertexbase, uint32_t iinstancebase)
	{
	}

	void RHIVulkanCommandList::SetComputeResources(uint32_t index, uint32_t packetoffset)
	{
	}
	
	void RHIVulkanCommandList::Dispatch(core::uint3 ngroups)
	{
	}
	
	void RHIVulkanCommandList::CopyResource(RHIResource * dst, RHIResource * src)
	{
	}
	
	void RHIVulkanCommandList::CopyBuffer(RHIResource * dst, RHIResource * src)
	{
	}
	
	void RHIVulkanCommandList::CopyResource(RHIResource * dst, RHIResource * src, uint32_t offset, uint32_t size)
	{
	}
}
