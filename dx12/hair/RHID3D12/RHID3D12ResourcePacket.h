#pragma once

#include "RHID3D12Core.h"
#include "RHID3D12Device.h"

namespace RHI::RHID3D12
{
	class RHID3D12ResourcePacket : public RHIResourcePacket
	{
	public:
		RHID3D12ResourcePacket(RHID3D12Device * device) : _device(device) {}
		virtual ~RHID3D12ResourcePacket() = default;

		core::error Create(const ResourcePacketArgs & args);

		void SetResource(uint32_t index, RHIResource * resource, const ResourceViewArgs & args) override;

	public:
		ID3D12DescriptorHeap * DescriptorHeap() { return _heap.get(); }
		D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle(uint32_t offset)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle = _heap->GetCPUDescriptorHandleForHeapStart();
			handle.ptr += _unit * offset;
			return handle;
		}
		D3D12_GPU_DESCRIPTOR_HANDLE GPUDescriptorHandle(uint32_t offset)
		{
			D3D12_GPU_DESCRIPTOR_HANDLE handle = _heap->GetGPUDescriptorHandleForHeapStart();
			handle.ptr += _unit * offset;
			return handle;
		}
		
	private:
		RHID3D12Device * _device = nullptr;
		core::comptr<ID3D12DescriptorHeap> _heap;
		std::vector<ResourceViewArgs> _viewArgs;

		ResourcePacketArgs _args;
		uint32_t _unit = 0;
	};
}
