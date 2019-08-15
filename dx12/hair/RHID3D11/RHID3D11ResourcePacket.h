#pragma once

#include "RHID3D11Core.h"
#include "RHID3D11Device.h"

namespace RHI::RHID3D11
{
	class RHID3D11ResourcePacket : public RHIResourcePacket
	{
	public:
		RHID3D11ResourcePacket(RHID3D11Device * device) : _device(device) {}
		virtual ~RHID3D11ResourcePacket() = default;

		core::error Create(const ResourcePacketArgs & args);

		void SetResource(uint32_t index, RHIResource * resource, const ResourceViewArgs & args) override;

	public:
		const std::vector<RHIResource *> & Resources() { return _resources; }
		const std::vector<ResourceViewArgs> & ViewArgs() { return _viewArgs; }
		const std::vector<core::comptr<ID3D11View>> & Views() { return _views; }
		
	public:
		RHID3D11Device * _device = nullptr;
		std::vector<RHIResource *> _resources;
		std::vector<ResourceViewArgs> _viewArgs;
		std::vector<core::comptr<ID3D11View>> _views;

		ResourcePacketArgs _args;
		uint32_t _unit = 0;
	};
}
