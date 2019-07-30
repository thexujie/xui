#include "stdafx.h"
#include "RHID3D12CommandQueue.h"

namespace RHI::RHID3D12
{
	core::error RHID3D12CommandQueue::Init(CommandType type, CommandQueueFlags flags)
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

		auto deviceptr = _device->Inner();
		assert(deviceptr);
		win32::comptr<ID3D12CommandQueue> queue;
		HRESULT hr = deviceptr->CreateCommandQueue(&queueDesc, __uuidof(ID3D12CommandQueue), queue.getvv());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " CreateCommandQueue failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}
		_queue = queue;
		_type = type;
		_flags = flags;
		return core::ok;
	}
}
