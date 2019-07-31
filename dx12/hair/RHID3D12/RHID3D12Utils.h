#pragma once

#include "RHI/RHI.h"
#include "d3d12/d3d12.h"

namespace RHI::RHID3D12
{
	inline void SetD3D12ObjectName(ID3D12Object * pObject, LPCWSTR name)
	{
		pObject->SetName(name);
	}
}
#define RHID3D12_SETNAME(x) RHI::RHID3D12::SetD3D12ObjectName((x).get(), L#x)
