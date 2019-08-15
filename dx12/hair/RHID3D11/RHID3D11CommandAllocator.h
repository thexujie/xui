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

		core::error Create(CommandType type, uint32_t count);
		void SetName(const std::u8string & name) override;

	public:
		void Reset(uint32_t index) override;

	private:
		RHID3D11Device * _device = nullptr;
	};
}
