#pragma once

#include "RHICore.h"
#include "RHI.h"

namespace RHI
{
	class RHIResourcePacket : public RHIObject
	{
	public:
		virtual void SetResource(uint32_t index, RHIResource * resource, const ResourceViewArgs & args) = 0;
		
	};
}
