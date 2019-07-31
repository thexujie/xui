#pragma once

#include "RHI/RHI.h"
#include "dxgi/dxgi.h"

namespace RHI::RHID3D12
{
	class RHID3D12Factory : public RHIFactory
	{
	public:
		RHID3D12Factory() = default;
		virtual ~RHID3D12Factory() = default;

		core::error Load();
		std::vector<RHIAdapterDesc> AdapterDescs() const override;
		std::shared_ptr<RHIDevice> CreateDevice(const std::u8string & uri) const override;

	private:
		utils::dll _dll;
		win32::comptr<IDXGIFactory3> _dxgi;
	};
}
