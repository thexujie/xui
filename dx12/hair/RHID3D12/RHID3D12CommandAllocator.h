#pragma once

#include "RHI/RHI.h"
#include "RHID3D12Device.h"

namespace RHI::RHID3D12
{
	class RHID3D12RenderTargetHWND;

	class RHID3D12CommandAllocator : public RHICommandAllocator
	{
	public:
		RHID3D12CommandAllocator(RHID3D12Device * device) : _device(device) {}
		virtual ~RHID3D12CommandAllocator() = default;

		core::error Create(CommandType type, uint32_t count);
		void SetName(const std::u8string & name) override;

	public:
		void Reset(uint32_t index) override;

	public:
		ID3D12CommandAllocator * CommandAllocator(uint32_t index) const { return _cmdallocators[index].get(); }

	private:
		RHID3D12Device * _device = nullptr;
		std::vector<core::comptr<ID3D12CommandAllocator>> _cmdallocators;
	};
}
