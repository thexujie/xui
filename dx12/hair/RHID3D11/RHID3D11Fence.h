#pragma once

#include "RHID3D11Core.h"

namespace RHI::RHID3D11
{
	class RHID3D11Fence : public RHIFence
	{
	public:
		RHID3D11Fence(RHID3D11Device * device) : _device(device) {}
		virtual ~RHID3D11Fence() = default;

		core::error Create(FenceFlags flags) { return core::ok; }
		void SetName(const std::u8string & name) override {}

	public:
		uint64_t CurrentFence() const override { return 0; }
		
	private:
		RHID3D11Device * _device = nullptr;
	};
}
