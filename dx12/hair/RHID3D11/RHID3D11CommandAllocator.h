#pragma once

#include "RHI/RHI.h"
#include "RHID3D11Device.h"

namespace RHI::RHID3D11
{
	class RHID3D11CommandAllocator : public RHICommandAllocator
	{
	public:
		RHID3D11CommandAllocator(RHID3D11Device * device) : _device(device) {}
		virtual ~RHID3D11CommandAllocator() = default;

		core::error Create(CommandType type);
		void SetName(const std::u8string & name);

	public:
		void Reset() override;

	public:
		ID3D11DeviceContext * DeviceContext() const { return _deferedContext.get(); }

	private:
		RHID3D11Device * _device = nullptr;
		core::comptr<ID3D11DeviceContext> _deferedContext;
	};
}
