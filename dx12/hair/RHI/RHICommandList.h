#pragma once

#include "RHICore.h"

namespace RHI
{
	class RHIPipelineState;
	class RHIResource;
	class RHICommandAllocator;
	class RHIResourceView;
	class RHIRenderTarget;
	class RHIResourcePacket;

	class RHICommandList : public RHIDeviceObject
	{
	public:
		RHICommandList() = default;
		virtual ~RHICommandList() = default;

	public:
		virtual void Reset(RHICommandAllocator * allocator, uint32_t index) = 0;
		virtual void Close() = 0;
		virtual void SetRenderTarget(RHIRenderTarget * rendertarget, uint32_t index) = 0;
		virtual void ClearRenderTarget(RHIRenderTarget * rendertarget, uint32_t index, core::color color) = 0;
		
		virtual void SetViewPort(const ViewPort & viewport) = 0;
		virtual void SetScissorRect(const core::recti & rect) = 0;
		virtual void TransitionBarrier(RHIResource * resource, ResourceStates states) = 0;
		virtual void UnorderedBarrier(RHIResource * resource) = 0;
		virtual void TransitionBarrier(RHIRenderTarget * rendertarget, uint32_t index, ResourceStates states) = 0;

		virtual void SetPipelineState(RHIPipelineState * pipelinestate) = 0;
		virtual void SetResourcePacket(RHIResourcePacket * packet) = 0;
		
		virtual void SetGraphicsResources(uint32_t index, uint32_t packetoffset) = 0;
		virtual void IASetVertexBuffer(RHIResource * indexbuffer, uint32_t stride, uint32_t size) = 0;
		virtual void IASetIndexBuffer(RHIResource * indexbuffer, uint32_t stride, uint32_t size) = 0;
		virtual void IASetTopologyType(Topology topology) = 0;
		virtual void DrawInstanced(uint32_t nvertices, uint32_t ninstance, uint32_t ivertexbase, uint32_t iinstancebase) = 0;
		virtual void DrawIndexedInstanced(uint32_t nindices, uint32_t ninstance, uint32_t iindexbase, uint32_t ivertexbase, uint32_t iinstancebase) = 0;

		virtual void SetComputeResources(uint32_t index, uint32_t packetoffset) = 0;
		virtual void Dispatch(core::uint3 ngroups) = 0;
		
		virtual void CopyResource(RHIResource * dst, RHIResource * src) = 0;
		virtual void CopyResource(RHIResource * dst, RHIResource * src, uint32_t offset, uint32_t size) = 0;
		virtual void CopyBuffer(RHIResource * dst, RHIResource * src) = 0;
	};
}
