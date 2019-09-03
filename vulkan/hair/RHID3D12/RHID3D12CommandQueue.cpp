#include "stdafx.h"
#include "RHID3D12CommandQueue.h"
#include "RHID3D12CommandList.h"
#include "RHID3D12Fence.h"

namespace RHI::RHID3D12
{
	core::error RHID3D12CommandQueue::Create(CommandType type, CommandQueueFlags flags)
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
		core::comptr<ID3D12CommandQueue> cmdqueue;
		HRESULT hr = device->CreateCommandQueue(&queueDesc, __uuidof(ID3D12CommandQueue), cmdqueue.getvv());
		if (FAILED(hr))
		{
			core::war() << __FUNCTION__ " CreateCommandQueue failed: " << win32::winerr_str(hr & 0xFFFF);
			return core::e_inner;
		}

		_cmdqueue = cmdqueue;
		_type = type;
		_flags = flags;
		_fenceEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
		return core::ok;
	}

	void RHID3D12CommandQueue::SetName(const std::u8string & name)
	{
		SetD3D12ObjectName(_cmdqueue.get(), name);
	}
	
	void RHID3D12CommandQueue::Excute(RHICommandList * cmdlist)
	{
		ID3D12CommandList * cmdlists[] = { reinterpret_cast<RHID3D12CommandList *>(cmdlist)->Ptr() };
		_cmdqueue->ExecuteCommandLists(1, cmdlists);
	}

	void RHID3D12CommandQueue::Signal(RHIFence * fence, uint64_t signal)
	{
		auto d3d12fence = static_cast<RHID3D12Fence *>(fence);
		HRESULT hr = S_OK;
		hr = _cmdqueue->Signal(d3d12fence->Fence(), signal);
		win32::throw_if_failed(hr);
	}

	void RHID3D12CommandQueue::Wait(RHIFence * fence, uint64_t value)
	{
		auto d3d12fence = static_cast<RHID3D12Fence *>(fence);
		HRESULT hr = S_OK;
		hr = _cmdqueue->Wait(d3d12fence->Fence(), value);
		win32::throw_if_failed(hr);
	}
	
	void RHID3D12CommandQueue::Fence(RHIFence * fence, uint64_t value)
	{
		auto d3d12fence = static_cast<RHID3D12Fence *>(fence);
		HRESULT hr = S_OK;
		if (d3d12fence->Fence()->GetCompletedValue() < value)
		{
			hr = d3d12fence->Fence()->SetEventOnCompletion(value, _fenceEvent);
			win32::throw_if_failed(hr);
			WaitForSingleObject(_fenceEvent, INFINITE);
		}
	}
	
	void RHID3D12CommandQueue::SignalAndFence(RHIFence * fence, uint64_t signal, uint64_t value)
	{
		auto d3d12fence = static_cast<RHID3D12Fence *>(fence);
		HRESULT hr = S_OK;
		hr = _cmdqueue->Signal(d3d12fence->Fence(), signal);
		win32::throw_if_failed(hr);

		if (d3d12fence->Fence()->GetCompletedValue() < value)
		{
			hr = d3d12fence->Fence()->SetEventOnCompletion(value, _fenceEvent);
			win32::throw_if_failed(hr);
			WaitForSingleObject(_fenceEvent, INFINITE);
		}
	}
}
