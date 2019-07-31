#pragma once

#include "RHI/RHI.h"
#include "RHID3D12Device.h"
#include "RHID3D12ResourceView.h"

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
		void Reset(RHICommandAllocator * allocator);
		void Close();
		void SetRenderTarget(RHIResourceView * rendertarget);
		void ClearRenderTarget(core::color color);
		void SetViewPort(const ViewPort & viewport);
		void SetScissorRect(const core::recti & rect);
		void TransitionBarrier(RHIResource * resource, ResourceStates states);
		void TransitionBarrier(uint32_t count, RHIResource ** resources, ResourceStates * states);

	public:
		ID3D12GraphicsCommandList * Ptr() const { return _cmdlist.get(); }

	private:
		RHID3D12Device * _device = nullptr;
		win32::comptr<ID3D12CommandAllocator> _cmdallocator;
		win32::comptr<ID3D12GraphicsCommandList> _cmdlist;

		RHID3D12ResourceView * _rendertarget = nullptr;
	};
}
