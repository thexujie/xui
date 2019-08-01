#pragma once

#include "RHI/RHI.h"
#include "RHI/RHICommandQueue.h"
#include "RHID3D12Device.h"

namespace RHI::RHID3D12
{
	class RHID3D12PipelineState : public RHIPipelineState
	{
	public:
		RHID3D12PipelineState(RHID3D12Device * device) : _device(device) {}
		virtual ~RHID3D12PipelineState() = default;

		core::error Create(const PipelineStateArgs & args);

	private:
		RHID3D12Device * _device = nullptr;
		win32::comptr<ID3D12RootSignature> _rootSignature;
		win32::comptr<ID3D12PipelineState> _pipelineState;

		CommandType _type = CommandType::Direct;
		CommandQueueFlags _flags = nullptr;
	};
}
