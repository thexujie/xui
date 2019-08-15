#pragma once

#include "RHI/RHI.h"
#include "RHID3D12Core.h"
#include "RHID3D12Device.h"

namespace RHI::RHID3D12
{
	class RHID3D12RenderTarget : public RHIRenderTarget
	{
	public:
		virtual D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle(uint32_t index) = 0;
		virtual D3D12_GPU_DESCRIPTOR_HANDLE GPUDescriptorHandle(uint32_t index) = 0;
		virtual ID3D12Resource * Buffer(uint32_t index) const = 0;
		virtual ResourceStates State(uint32_t index) const = 0;
		virtual void SetState(uint32_t index, ResourceStates state) = 0;
	};
}
