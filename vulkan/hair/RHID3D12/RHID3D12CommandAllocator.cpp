#include "stdafx.h"
#include "RHID3D12CommandAllocator.h"

namespace RHI::RHID3D12
{
	core::error RHID3D12CommandAllocator::Create(CommandType type, uint32_t count)
	{
		if (!_device)
			return core::e_state;

		HRESULT hr = S_OK;

		auto device = _device->Device();
		auto adapter = _device->Adapter();
		assert(device);
		assert(adapter);

		std::vector<core::comptr<ID3D12CommandAllocator>> cmdallocators(count);
		for (uint32_t icmdallocator = 0; icmdallocator < count; ++icmdallocator)
		{
			hr = device->CreateCommandAllocator(FromCommandType(type), __uuidof(ID3D12CommandAllocator), cmdallocators[icmdallocator].getvv());
			if (FAILED(hr))
			{
				core::war() << __FUNCTION__ " device->CreateCommandAllocator failed: " << win32::winerr_str(hr & 0xFFFF);
				return core::e_inner;
			}
		}
		_cmdallocators = cmdallocators;
		return core::ok;
	}

	void RHID3D12CommandAllocator::Reset(uint32_t index)
	{
		_cmdallocators[index]->Reset();
	}
	
	void RHID3D12CommandAllocator::SetName(const std::u8string & name)
	{
		for (auto & cmdallocator : _cmdallocators)
			SetD3D12ObjectName(cmdallocator.get(), name);
	}
}
