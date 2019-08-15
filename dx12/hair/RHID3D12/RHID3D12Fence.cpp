#include "stdafx.h"
#include "RHID3D12Fence.h"
#include "RHID3D12Device.h"

namespace RHI::RHID3D12
{
	core::error RHID3D12Fence::Create(FenceFlags flags)
	{
		if (!_device)
			return core::e_state;

		HRESULT hr = S_OK;

		auto device = _device->Device();
		auto adapter = _device->Adapter();
		assert(device);
		assert(adapter);

		core::comptr<ID3D12Fence> fence;
		hr = device->CreateFence(0, FromFenceFlags(flags), __uuidof(ID3D12Fence), fence.getvv());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " device->CreateFence failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}
		_fence = fence;
		return core::ok;
	}

	void RHID3D12Fence::SetName(const std::u8string & name)
	{
		SetD3D12ObjectName(_fence.get(), name);
	}
	
	uint64_t RHID3D12Fence::CurrentFence() const
	{
		return _fence->GetCompletedValue();
	}
}
