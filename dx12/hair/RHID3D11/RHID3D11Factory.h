#pragma once

#include "RHID3D11Core.h"

namespace RHI::RHID3D11
{
	class RHID3D11Factory : public RHIFactory
	{
	public:
		RHID3D11Factory() = default;
		virtual ~RHID3D11Factory() = default;

		core::error Load();
		std::vector<RHIAdapterDesc> AdapterDescs() const override;
		std::shared_ptr<RHIDevice> CreateDevice(const std::u8string & uri) const override;

	private:
		utils::dll _dll;
		core::comptr<IDXGIFactory3> _dxgi;
	};
}
