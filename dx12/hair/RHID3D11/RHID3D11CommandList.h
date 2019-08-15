#pragma once

#include "RHI/RHI.h"
#include "RHID3D11Device.h"

namespace RHI::RHID3D11
{
	class RHID3D11RenderTarget;
	class RHID3D11ResourcePacket;
	class RHID3D11PipelineState;

	class RHID3D11CommandList : public RHICommandList
	{
	public:
		RHID3D11CommandList(RHID3D11Device * device) : _device(device) {}
		virtual ~RHID3D11CommandList() = default;

		core::error Create(CommandType type, RHICommandAllocator * allocator);
		void SetName(const std::u8string & name);

	public:
		void Reset(RHICommandAllocator * allocator) override;
		void Close() override;
		void SetRenderTarget(RHIRenderTarget * rendertarget) override;
		void ClearRenderTarget(RHIRenderTarget * rendertarget, core::color color) override;
		void SetViewPort(const ViewPort & viewport) override;
		void SetScissorRect(const core::recti & rect) override;
		void TransitionBarrier(RHIResource * resource, ResourceStates states) override;
		void TransitionBarrier(RHIRenderTarget * rendertarget, ResourceStates states) override;

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
		ID3D11DeviceContext * DeviceContext() const { return _deferredContext.get(); }
		ID3D11CommandList * CommandList() const { return _commandList.get(); }

	private:
		RHID3D11Device * _device = nullptr;
		core::comptr<ID3D11DeviceContext> _deferredContext;
		core::comptr<ID3D11CommandList> _commandList;

		RHID3D11PipelineState * _pipelineState = nullptr;
		RHID3D11ResourcePacket * _resourcePacket = nullptr;
	};
}
