#include "stdafx.h"
#include "RHID3D11CommandAllocator.h"

namespace RHI::RHID3D11
{
	core::error RHID3D11CommandAllocator::Create(CommandType type)
	{
		if (!_device)
			return core::e_state;

		HRESULT hr = S_OK;

		auto device = _device->Inner();
		auto adapter = _device->InnerAdapter();
		assert(device);
		assert(adapter);

		uint32_t flags = 0;
		core::comptr<ID3D11DeviceContext> deferedContext;
		hr = device->CreateDeferredContext(flags, deferedContext.getpp());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " device->CreateDeferredContext failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}

		_deferedContext = deferedContext;
		return core::ok;
	}

	void RHID3D11CommandAllocator::Reset()
	{
	}
	
	void RHID3D11CommandAllocator::SetName(const std::u8string & name)
	{
		SetD3D11ObjectName(_deferedContext.get(), name);
	}
}
