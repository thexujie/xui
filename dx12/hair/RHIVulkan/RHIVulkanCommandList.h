#pragma once

#include "RHI/RHI.h"
#include "RHIVulkanDevice.h"
#include "RHID3D11/RHID3D11CommandList.h"

namespace RHI::RHIVulkan
{
	class RHIVulkanRenderTarget;
	class RHIVulkanResourcePacket;
	class RHIVulkanPipelineState;
	class RHIVulkanCommandAllocator;

	class RHIVulkanCommandList : public RHICommandList
	{
	public:
		RHIVulkanCommandList(RHIVulkanDevice * device) : _device(device) {}
		virtual ~RHIVulkanCommandList() = default;

		core::error Create(CommandType type, RHIVulkanCommandAllocator * allocator);
		void SetName(const std::u8string & name) override;

	public:
		void Reset(RHICommandAllocator * allocator, uint32_t index) override;
		void Close() override;
		void SetRenderTarget(RHIRenderTarget * rendertarget, uint32_t index) override;
		void ClearRenderTarget(RHIRenderTarget * rendertarget, uint32_t index, core::color color) override;
		void SetViewPort(const ViewPort & viewport) override;
		void SetScissorRect(const core::recti & rect) override;
		void TransitionBarrier(RHIResource * resource, ResourceStates states) override;
		void UnorderedBarrier(RHIResource * resource) override {}
		void TransitionBarrier(RHIRenderTarget * rendertarget, uint32_t index, ResourceStates states) override;

		void SetPipelineState(RHIPipelineState * pipelinestate) override;
		void SetResourcePacket(RHIResourcePacket * packet) override;
		
		void SetGraphicsResources(uint32_t index, uint32_t packetoffset) override;
		void IASetVertexBuffer(RHIResource * resource, uint32_t stride, uint32_t size) override;
		void IASetIndexBuffer(RHIResource * resource, uint32_t stride, uint32_t size) override;
		void IASetTopologyType(Topology topology) override;
		void DrawInstanced(uint32_t nvertices, uint32_t ninstance, uint32_t ivertexbase, uint32_t iinstancebase) override;
		void DrawIndexedInstanced(uint32_t nindices, uint32_t ninstance, uint32_t iindexbase, uint32_t ivertexbase, uint32_t iinstancebase) override;

		void SetComputeResources(uint32_t index, uint32_t packetoffset) override;
		void Dispatch(core::uint3 ngroups) override;
		
		void CopyResource(RHIResource * dst, RHIResource * src) override;
		void CopyResource(RHIResource * dst, RHIResource * src, uint32_t offset, uint32_t size) override;
		void CopyBuffer(RHIResource * dst, RHIResource * src) override;

	private:
		RHIVulkanDevice * _device = nullptr;
		RHIVulkanCommandAllocator * _allocator = nullptr;
		VkCommandBuffer _commandBuffer = nullptr;

		RHIVulkanPipelineState * _pipelineState = nullptr;
		RHIVulkanResourcePacket * _resourcePacket = nullptr;
	};
}
