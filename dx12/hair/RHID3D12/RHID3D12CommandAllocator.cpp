#include "stdafx.h"
#include "RHID3D12CommandAllocator.h"

namespace RHI::RHID3D12
{
	core::error RHID3D12CommandAllocator::Create(CommandType type)
	{
		if (!_device)
			return core::e_state;

		HRESULT hr = S_OK;

		auto device = _device->Inner();
		auto adapter = _device->InnerAdapter();
		assert(device);
		assert(adapter);

		win32::comptr<ID3D12CommandAllocator> cmdallocator;
		win32::comptr<ID3D12GraphicsCommandList> cmdlist;

		hr = device->CreateCommandAllocator(FromCommandType(type), __uuidof(ID3D12CommandAllocator), cmdallocator.getvv());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " device->CreateCommandAllocator failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}

		_cmdallocator = cmdallocator;
		return core::ok;
	}

	void RHID3D12CommandAllocator::Reset()
	{
		_cmdallocator->Reset();
	}

}
