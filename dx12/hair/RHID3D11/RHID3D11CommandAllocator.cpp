#include "stdafx.h"
#include "RHID3D11CommandAllocator.h"

namespace RHI::RHID3D11
{
	core::error RHID3D11CommandAllocator::Create(CommandType type)
	{
		if (!_device)
			return core::e_state;

		return core::ok;
	}

	void RHID3D11CommandAllocator::Reset()
	{
	}
	
	void RHID3D11CommandAllocator::SetName(const std::u8string & name)
	{
	}
}
