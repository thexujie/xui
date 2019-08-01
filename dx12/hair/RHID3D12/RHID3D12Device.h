#pragma once

#include "RHID3D12Core.h"

namespace RHI::RHID3D12
{
	class RHID3D12Device : public RHIDevice
	{
	public:
		RHID3D12Device() = default;
		virtual ~RHID3D12Device() = default;

		core::error Create(win32::comptr<IDXGIAdapter1> adapter);

		win32::comptr<ID3D12Device> Inner() const { return _device; }
		win32::comptr<IDXGIAdapter1> InnerAdapter() const { return _adapter; }

	public:
		std::shared_ptr<RHICommandQueue> CreateCommandQueue(CommandType type, CommandQueueFlags flags) const override;
		std::shared_ptr<RHICommandAllocator> CreateCommandAllocator(CommandType type) const override;
		std::shared_ptr<RHICommandList> CreateCommandList(CommandType type) const override;
		std::shared_ptr<RHIResource> CreateResource(const ResourceArgs & args) const override;
		std::shared_ptr<RHIResourceView> CreateShaderResourceView(const RHIResource * resource, const ShaderResourceViewArgs & args) const override;
		std::shared_ptr<RHIResourceView> CreateConstBufferView(const RHIResource * resource, const ConstBufferViewArgs & args) const override;
		std::shared_ptr<RHIRenderTarget> CreateRenderTargetForHWND(const RenderTargetArgs & args) const override;
		std::shared_ptr<RHIPipelineState> CreatePipelineState(const PipelineStateArgs & args) const override;

	private:
		win32::comptr<IDXGIAdapter1> _adapter;
		win32::comptr<ID3D12Device> _device;
	};
}
