#pragma once

#include "RHI/RHI.h"
#include "RHID3D12Core.h"
#include "RHID3D12Device.h"

namespace RHI::RHID3D12
{
	class RHID3D12RenderTarget : public RHIRenderTarget
	{
	public:
		virtual D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle() = 0;
		virtual D3D12_GPU_DESCRIPTOR_HANDLE GPUDescriptorHandle() = 0;
	};
}
