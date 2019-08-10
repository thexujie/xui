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
		virtual void Reset(RHICommandAllocator * allocator) = 0;
		virtual void Close() = 0;
		virtual void SetRenderTarget(RHIResourceView * rendertarget) = 0;
		virtual void ClearRenderTarget(core::color color) = 0;
		
		virtual void SetViewPort(const ViewPort & viewport) = 0;
		virtual void SetScissorRect(const core::recti & rect) = 0;
		virtual void TransitionBarrier(RHIResource * resource, ResourceStates states) = 0;
		virtual void TransitionBarrier(RHIRenderTarget * rendertarget, ResourceStates states) = 0;

		virtual void SetPipelineState(RHIPipelineState * pipelinestate) = 0;
		virtual void SetResourcePacket(RHIResourcePacket * packet) = 0;
		
		virtual void SetGraphicsResourceView(uint32_t index, RHIResourceView * view) = 0;
		virtual void IASetVertexBuffer(RHIResource * indexbuffer, uint32_t stride, uint32_t size) = 0;
		virtual void IASetIndexBuffer(RHIResource * indexbuffer, uint32_t stride, uint32_t size) = 0;
		virtual void IASetTopologyType(Topology topology) = 0;
		virtual void DrawInstanced(uint32_t nvertices, uint32_t ninstance, uint32_t ivertexbase, uint32_t iinstancebase) = 0;
		virtual void DrawIndexedInstanced(uint32_t nindices, uint32_t ninstance, uint32_t iindexbase, uint32_t ivertexbase, uint32_t iinstancebase) = 0;

		virtual void SetComputeResourceView(uint32_t index, RHIResourceView * view) = 0;
		virtual void Dispatch(core::uint3 ngroups) = 0;
		
		virtual void CopyResource(RHIResource * dst, RHIResource * src) = 0;
		virtual void CopyBuffer(RHIResource * dst, RHIResource * src) = 0;
	};
}
