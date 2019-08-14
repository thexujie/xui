#pragma once

#include "RHI/RHI.h"
#include "win32/d3d11.h"

namespace RHI::RHID3D11
{
	const GUID GUID_D3DDebugObjectName = { 0x429b8c22, 0x9188, 0x4b0c, 0x87, 0x42, 0xac, 0xb0, 0xbf, 0x85, 0xc2, 0x00 };
	
	template<typename OT>
	void SetD3D11ObjectName(OT * pObject, const std::string & name)
	{
		if (pObject)
			pObject->SetPrivateData(GUID_D3DDebugObjectName, (uint32_t)name.length(), name.c_str());
	}

	template<typename OT>
	void SetD3D11ObjectName(OT * pObject, const std::u8string & name)
	{
		if (pObject)
			pObject->SetPrivateData(GUID_D3DDebugObjectName, (uint32_t)name.length(), static_cast<const void *>(name.c_str()));
	}
	
	inline std::string_view D3D11BlobMessage(ID3DBlob * blob)
	{
		if (!blob || blob->GetBufferSize() < 2)
			return std::string_view();

		return blob ? std::string_view(static_cast<const char *>(blob->GetBufferPointer()), blob->GetBufferSize() - 1) : std::string_view();
	}

	inline std::string_view D3D11BlobMessage(core::comptr<ID3DBlob> blob)
	{
		if (!blob || blob->GetBufferSize() < 2)
			return std::string_view();

		return blob ? std::string_view(static_cast<const char *>(blob->GetBufferPointer()), blob->GetBufferSize() - 1) : std::string_view();
	}
}
#define RHID3D12_SETNAME(x) RHI::RHID3D12::SetD3D12ObjectName((x).get(), L#x)
