#pragma once

#include "RHI/RHI.h"
#include "RHID3D11Device.h"

namespace RHI::RHID3D11
{
	class RHID3D11Resource : public RHIResource
	{
	public:
		RHID3D11Resource(RHID3D11Device * device) : _device(device) {}
		virtual ~RHID3D11Resource() = default;

		core::error Create(const ResourceArgs & args);
		void SetName(const std::u8string & name);
		void TransitionBarrier(class RHICommandList * cmdlist, ResourceStates states);

	public:
		void * Data() override { return _pointer; }
		const void * Data() const override { return _pointer; }
		core::sizeu Size() const override { return _args.size; }

		void * Map() override { return _pointer; }
		void Unmap() override {}
	public:
		const ResourceArgs & Args() const { return _args; }
		ID3D11Resource * Resource() const { return _resource.get(); }
		
	private:
		RHID3D11Device * _device = nullptr;
		ResourceArgs _args;
		core::comptr<ID3D11Resource> _resource;
		void * _pointer = nullptr;
	};
}
