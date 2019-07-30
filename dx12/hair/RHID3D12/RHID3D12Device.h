#pragma once

#include "RHID3D12Core.h"

namespace RHI::RHID3D12
{
	class RHID3D12Device : public RHIDevice
	{
	public:
		RHID3D12Device() = default;
		virtual ~RHID3D12Device() = default;

		core::error Init(win32::comptr<IDXGIAdapter1> adapter);

		win32::comptr<ID3D12Device> Inner() const { return _device; }
		win32::comptr<IDXGIAdapter1> InnerAdapter() const { return _adapter; }

	public:
		std::shared_ptr<RHICommandQueue> CreateCommandQueue(CommandType type, CommandQueueFlags flags) const override;
		std::shared_ptr<RHICommandList> CreateCommandList(CommandType type) const override;
		std::shared_ptr<RHIBuffer> CreateBuffer(const BufferParams & params) const override;
		std::shared_ptr<RHIRenderTarget> CreateRenderTargetForHWND(const RenderTargetParams & params) const override;

	private:
		win32::comptr<IDXGIAdapter1> _adapter;
		win32::comptr<ID3D12Device> _device;
	};
}
