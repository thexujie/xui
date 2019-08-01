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

		core::error Create(const ResourceArgs & params);
		void TransitionBarrier(class RHICommandList * cmdlist, ResourceStates states);

	public:
		void * Data();
		const void * Data() const;
		core::sizeu Size() const;

		void * Map();
		void Unmap();
	public:
		ID3D12Resource * Ptr() const { return _resource.get(); }
		D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress() const { return _resource->GetGPUVirtualAddress(); }
		
	private:
		RHID3D12Device * _device = nullptr;
		ResourceArgs _params;
		win32::comptr<ID3D12Resource> _resource;
		void * _pointer = nullptr;
	};
}
