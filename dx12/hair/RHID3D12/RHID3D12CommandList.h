#pragma once

#include "RHI/RHI.h"
#include "RHID3D12Device.h"

namespace RHI::RHID3D12
{
	class RHID3D12RenderTarget;
	class RHID3D12ResourcePacket;
	class RHID3D12ResourceView;

	class RHID3D12CommandList : public RHICommandList
	{
	public:
		RHID3D12CommandList(RHID3D12Device * device) : _device(device) {}
		virtual ~RHID3D12CommandList() = default;

		core::error Create(CommandType type, RHICommandAllocator * allocator);
		void SetName(const std::u8string & name) override;

	public:
		void Reset(RHICommandAllocator * allocator, uint32_t index) override;
		void Close() override;
		void SetRenderTarget(RHIRenderTarget * rendertarget, uint32_t index) override;
		void ClearRenderTarget(RHIRenderTarget * rendertarget, uint32_t index, core::color color) override;
		void SetViewPort(const ViewPort & viewport) override;
		void SetScissorRect(const core::recti & rect) override;
		void TransitionBarrier(RHIResource * resource, ResourceStates state) override;
		void UnorderedBarrier(RHIResource * resource) override;
		void TransitionBarrier(RHIRenderTarget * rendertarget, uint32_t index, ResourceStates state) override;

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

	public:
		ID3D12GraphicsCommandList * Ptr() const { return _cmdlist.get(); }

	private:
		RHID3D12Device * _device = nullptr;
		core::comptr<ID3D12GraphicsCommandList> _cmdlist;

		RHID3D12ResourcePacket * _resourcepacket = nullptr;
	};
}
