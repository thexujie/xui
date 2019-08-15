#pragma once

#include "RHI/RHI.h"
#include "RHID3D12Device.h"

namespace RHI::RHID3D12
{
	class RHID3D12Resource : public RHIResource
	{
	public:
		RHID3D12Resource(RHID3D12Device * device) : _device(device) {}
		RHID3D12Resource(RHID3D12Device * device, core::comptr<ID3D12Resource> resource) : _device(device), _resource(resource){}
		virtual ~RHID3D12Resource() = default;

		core::error Create(const ResourceArgs & args);
		void SetName(const std::u8string & name) override;

	public:
		void * Data() override;
		const void * Data() const override;
		core::sizeu Size() const override;

	public:
		const ResourceArgs & Args() const { return _args; }
		ID3D12Resource * Resource() const { return _resource.get(); }
		ResourceStates State() const { return _state; }
		void SetState(ResourceStates state) { _state = state; }
		D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress() const { return _resource->GetGPUVirtualAddress(); }
		
	private:
		RHID3D12Device * _device = nullptr;
		ResourceArgs _args;
		core::comptr<ID3D12Resource> _resource;
		void * _pointer = nullptr;
		ResourceStates _state = ResourceState::None;
	};
}
