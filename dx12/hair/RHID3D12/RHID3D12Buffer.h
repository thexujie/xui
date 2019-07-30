#pragma once

#include "RHI/RHI.h"
#include "RHID3D12Device.h"

namespace RHI::RHID3D12
{
	class RHID3D12Buffer : public RHIBuffer
	{
	public:
		RHID3D12Buffer(RHID3D12Device * device) : _device(device) {}
		virtual ~RHID3D12Buffer() = default;

		core::error Init(const BufferParams & params);

	public:
		void TransitionBarrier(class RHICommandList * cmdlist, ResourceStates states) override;

	private:
		RHID3D12Device * _device = nullptr;
		BufferParams _params;
		win32::comptr<ID3D12Resource> _buffer;
	};
}
