#include "stdafx.h"
#include "RHID3D11CommandQueue.h"
#include "RHID3D11CommandList.h"

namespace RHI::RHID3D11
{
	core::error RHID3D11CommandQueue::Create(CommandType type, CommandQueueFlags flags)
	{
		if (!_device)
			return core::e_state;

		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		if (flags.any(CommandQueueFlag::DisableGPUTimeout))
			queueDesc.Flags |= D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT;

		switch (type)
		{
		case CommandType::Direct:
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			break;
		case CommandType::Bundle:
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_BUNDLE;
			break;
		case CommandType::Compute:
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
			break;
		case CommandType::Copy:
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
			break;
		default:
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			break;
		}

		auto device = _device->Device();
		assert(device);
		core::comptr<ID3D11DeviceContext> immediateContext;
		device->GetImmediateContext(immediateContext.getpp());

		_type = type;
		_flags = flags;
		_fenceValue = 0;
		_fenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
		_immediateContext = immediateContext;
		return core::ok;
	}

	void RHID3D11CommandQueue::SetName(const std::u8string & name)
	{
		//SetD3D12ObjectName(_cmdqueue.get(), name);
	}
	
	void RHID3D11CommandQueue::Excute(RHICommandList * cmdlist)
	{
		_immediateContext->ExecuteCommandList(reinterpret_cast<RHID3D11CommandList *>(cmdlist)->CommandList(), false);
	}
}
