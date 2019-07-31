#pragma once

#include "RHI/RHI.h"
#include "RHID3D12Device.h"

namespace RHI::RHID3D12
{
	class RHID3D12Resource : public RHIResource
	{
	public:
		RHID3D12Resource(RHID3D12Device * device) : _device(device) {}
		virtual ~RHID3D12Resource() = default;

		core::error Create(const ResourceParams & params);

	public:
		void TransitionBarrier(class RHICommandList * cmdlist, ResourceStates states) override;

	public:
		ID3D12Resource * Ptr() const { return _resource.get(); }

	private:
		RHID3D12Device * _device = nullptr;
		ResourceParams _params;
		win32::comptr<ID3D12Resource> _resource;
	};
}
