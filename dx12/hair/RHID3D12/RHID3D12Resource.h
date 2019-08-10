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
		void SetName(const std::u8string & name);
		void TransitionBarrier(class RHICommandList * cmdlist, ResourceStates states);

	public:
		void * Data() override;
		const void * Data() const override;
		core::sizeu Size() const override;

		void * Map() override;
		void Unmap() override;
	public:
		const ResourceArgs & Args() const { return _args; }
		ID3D12Resource * Resource() const { return _resource.get(); }
		D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress() const { return _resource->GetGPUVirtualAddress(); }
		
	private:
		RHID3D12Device * _device = nullptr;
		ResourceArgs _args;
		win32::comptr<ID3D12Resource> _resource;
		void * _pointer = nullptr;
	};
}
