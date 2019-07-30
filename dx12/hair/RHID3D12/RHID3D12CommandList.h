#pragma once

#include "RHI/RHI.h"
#include "RHID3D12Device.h"

namespace RHI::RHID3D12
{
	class RHID3D12RenderTarget;

	class RHID3D12CommandList : public RHICommandList
	{
	public:
		RHID3D12CommandList(RHID3D12Device * device) : _device(device) {}
		virtual ~RHID3D12CommandList() = default;

		core::error Init(CommandType type);

	public:
		void Reset();
		void Close();
		void SetRenderTarget(RHIRenderTarget * rendertarget);
		void ClearRenderTarget(core::color color);
		void SetViewPort(const ViewPort & viewport);
		void SetScissorRect(const core::recti & rect);
		void TransitionBarrier(RHIResource * resource, ResourceStates states);

	public:
		ID3D12GraphicsCommandList * Ptr() const { return _cmdlist.get(); }

	private:
		RHID3D12Device * _device = nullptr;
		win32::comptr<ID3D12CommandAllocator> _cmdallocator;
		win32::comptr<ID3D12GraphicsCommandList> _cmdlist;

		RHID3D12RenderTarget * _rendertarget = nullptr;
	};
}
