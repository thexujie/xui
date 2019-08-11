#pragma once

#include "RHICore.h"
#include "RHI.h"

namespace RHI
{
	class RHIResourcePacket : public RHIObject
	{
	public:
		virtual std::shared_ptr<RHIResourceView> SetResource(uint32_t index, RHIResource * resource, const ResourceViewArgs & args) = 0;
		
	};
}
