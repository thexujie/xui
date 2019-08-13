#pragma once

#include "RHI/RHI.h"
#include "d3d12/d3d12.h"

namespace RHI::RHID3D12
{
	inline void SetD3D12ObjectName(ID3D12Object * pObject, LPCWSTR name)
	{
		pObject->SetName(name);
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
