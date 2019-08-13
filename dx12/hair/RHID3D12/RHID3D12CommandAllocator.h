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

		core::error Create(CommandType type);
		void SetName(const std::u8string & name);

	public:
		void Reset() override;

	public:
		ID3D12CommandAllocator * CommandAllocator() const { return _cmdallocator.get(); }

	private:
		RHID3D12Device * _device = nullptr;
		core::comptr<ID3D12CommandAllocator> _cmdallocator;
	};
}
