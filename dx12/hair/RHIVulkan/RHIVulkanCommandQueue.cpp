#include "stdafx.h"
#include "RHIVulkanCommandQueue.h"
#include "RHIVulkanCommandList.h"

namespace RHI::RHIVulkan
{
	core::error RHIVulkanCommandQueue::Create(CommandType type, CommandQueueFlags flags)
	{
		if (!_device)
			return core::e_state;


		_type = type;
		_flags = flags;
		_fenceValue = 0;
		_fenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
		return core::ok;
	}

	void RHIVulkanCommandQueue::SetName(const std::u8string & name)
	{
		//SetD3D12ObjectName(_cmdqueue.get(), name);
	}
	
	void RHIVulkanCommandQueue::Excute(RHICommandList * cmdlist)
	{
		//_immediateContext->ExecuteCommandList(reinterpret_cast<RHIVulkanCommandList *>(cmdlist)->CommandList(), false);
	}
}
