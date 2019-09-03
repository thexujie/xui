#pragma once

#include "RHI/RHI.h"
#include "RHI/RHICommandQueue.h"
#include "RHIVulkanDevice.h"
#include "RHID3D12/RHID3D12.h"

namespace RHI::RHIVulkan
{
	class RHIVulkanCommandQueue : public RHICommandQueue
	{
	public:
		RHIVulkanCommandQueue(RHIVulkanDevice * device) : _device(device) {}
		virtual ~RHIVulkanCommandQueue() = default;

		core::error Create(CommandType type, CommandQueueFlags flags);
		void SetName(const std::u8string & name) override;

	public:
		void Excute(RHICommandList * cmdlist) override;
		void Signal(RHIFence * fence, uint64_t signal) override {}
		void Fence(RHIFence * fence, uint64_t value) override {}
		void Wait(RHIFence * fence, uint64_t value) override {}
		void SignalAndFence(RHIFence * fence, uint64_t signal, uint64_t value) override {}

	private:
		RHIVulkanDevice * _device = nullptr;

		CommandType _type = CommandType::Direct;
		CommandQueueFlags _flags = nullptr;

		uint64_t _fenceValue = 0;
		HANDLE _fenceEvent = NULL;
	};
}
