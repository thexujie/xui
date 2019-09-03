#pragma once

#include "RHICore.h"
#include "RHIResource.h"

namespace RHI
{
	class RHIResourcePacket : public RHIObject
	{
	public:
		RHIResourcePacket();
		~RHIResourcePacket();

	public:
		virtual std::shared_ptr<RHIResourceView> SetShaderResource(uint32_t index, RHIResource * resource, const ResourceViewArgs & args) = 0;
		
	};
}
