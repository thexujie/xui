#pragma once

#include "RHID3D12Core.h"

namespace RHI::RHID3D12
{
	class RHID3D12Fence: public RHIFence
	{
	public:
		RHID3D12Fence(RHID3D12Device * device) : _device(device) {}
		virtual ~RHID3D12Fence() = default;

		core::error Create(FenceFlags flags);
		void SetName(const std::u8string & name) override;

public:
	uint64_t CurrentFence() const override;
		
public:
	ID3D12Fence * Fence() const { return _fence.get(); }
		
	private:
		RHID3D12Device * _device = nullptr;
		ResourceArgs _args;
		core::comptr<ID3D12Fence> _fence;
		void * _pointer = nullptr;
	};
}
