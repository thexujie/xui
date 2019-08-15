#pragma once

#include "RHI/RHI.h"
#include "RHI/RHICommandQueue.h"
#include "RHID3D11Device.h"
#include "RHID3D12/RHID3D12.h"

namespace RHI::RHID3D11
{
	class RHID3D11CommandQueue : public RHICommandQueue
	{
	public:
		RHID3D11CommandQueue(RHID3D11Device * device) : _device(device) {}
		virtual ~RHID3D11CommandQueue() = default;

		core::error Create(CommandType type, CommandQueueFlags flags);
		void SetName(const std::u8string & name) override;

	public:
		void Excute(RHICommandList * cmdlist) override;
		void Fence(uint64_t signal, uint64_t fence) override;

	public:
		ID3D11DeviceContext * DeviceContext() const { return _immediateContext.get(); }
		
	private:
		RHID3D11Device * _device = nullptr;
		core::comptr<ID3D11DeviceContext> _immediateContext;

		CommandType _type = CommandType::Direct;
		CommandQueueFlags _flags = nullptr;

		uint64_t _fenceValue = 0;
		HANDLE _fenceEvent = NULL;
	};
}
