#pragma once

#include "RHI/RHI.h"
#include "RHID3D12Device.h"
#include "RHID3D12View.h"

namespace RHI::RHID3D12
{
	class RHID3D12RenderTarget;

	class RHID3D12CommandList : public RHICommandList
	{
	public:
		RHID3D12CommandList(RHID3D12Device * device) : _device(device) {}
		virtual ~RHID3D12CommandList() = default;

		core::error Create(CommandType type);

	public:
		void Reset(RHICommandAllocator * allocator) override;
		void Close() override;
		void SetRenderTarget(RHIResourceView * rendertarget) override;
		void ClearRenderTarget(core::color color) override;
		void SetViewPort(const ViewPort & viewport) override;
		void SetScissorRect(const core::recti & rect) override;
		void TransitionBarrier(RHIResource * resource, ResourceStates states) override;
		void TransitionBarrier(RHIRenderTarget * rendertarget, ResourceStates states) override;

		void SetPipelineState(RHIPipelineState * pipelinestate);
		void SetResourceViews(RHIResourceView ** views, uint32_t nviews);
		void SetGraphicsResourceView(uint32_t index, RHIResourceView * view);
		void IASetVertexBuffer(RHIResource * resource, uint32_t stride, uint32_t size) override;
		void IASetIndexBuffer(RHIResource * resource, uint32_t stride, uint32_t size) override;
		void IASetTopologyType(Topology topology) override;
		void DrawInstanced(uint32_t nvertices, uint32_t ninstance, uint32_t ivertexbase, uint32_t iinstancebase);

	public:
		ID3D12GraphicsCommandList * Ptr() const { return _cmdlist.get(); }

	private:
		RHID3D12Device * _device = nullptr;
		win32::comptr<ID3D12CommandAllocator> _cmdallocator;
		win32::comptr<ID3D12GraphicsCommandList> _cmdlist;

		RHID3D12RenderTargetView * _rendertarget = nullptr;
	};
}
