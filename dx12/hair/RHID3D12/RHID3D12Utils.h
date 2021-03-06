#pragma once

#include "RHI/RHI.h"
#include "win32/d3d12.h"

namespace RHI::RHID3D12
{
	inline void SetD3D12ObjectName(ID3D12Object * pObject, LPCWSTR name)
	{
		pObject->SetName(name);
	}
	
	inline void SetD3D12ObjectName(ID3D12Object * pObject, const std::wstring & name)
	{
		pObject->SetName(name.c_str());
	}

	inline void SetD3D12ObjectName(ID3D12Object * pObject, const std::u8string & u8name)
	{
		pObject->SetName(core::u8str_wstr(u8name).c_str());
	}

	inline std::string_view D3D12BlobMessage(ID3DBlob * blob)
	{
		if (!blob || blob->GetBufferSize() < 2)
			return std::string_view();

		return blob ? std::string_view(static_cast<const char *>(blob->GetBufferPointer()), blob->GetBufferSize() - 1) : std::string_view();
	}

	inline std::string_view D3D12BlobMessage(core::comptr<ID3DBlob> blob)
	{
		if (!blob || blob->GetBufferSize() < 2)
			return std::string_view();

		return blob ? std::string_view(static_cast<const char *>(blob->GetBufferPointer()), blob->GetBufferSize() - 1) : std::string_view();
	}
}
#define RHID3D12_SETNAME(x) RHI::RHID3D12::SetD3D12ObjectName((x).get(), L#x)
