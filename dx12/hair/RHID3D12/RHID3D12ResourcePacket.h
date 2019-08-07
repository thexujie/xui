#pragma once

#include "RHID3D12Core.h"
#include "RHID3D12Device.h"
#include "RHID3D12ResourceView.h"

namespace RHI::RHID3D12
{
	class RHID3D12ResourcePacket : public RHIResourcePacket
	{
	public:
		RHID3D12ResourcePacket(RHID3D12Device * device) : _device(device) {}
		virtual ~RHID3D12ResourcePacket() = default;

		core::error Create(const ResourcePacketArgs & args);

		std::shared_ptr<RHIResourceView> SetShaderResource(uint32_t index, RHIResource * resource, const ResourceViewArgs & args) override;

	public:
		ID3D12DescriptorHeap * DescriptorHeap() { return _heap.get(); }
		D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle() { return _heap->GetCPUDescriptorHandleForHeapStart(); }
		D3D12_GPU_DESCRIPTOR_HANDLE GPUDescriptorHandle() { return _heap->GetGPUDescriptorHandleForHeapStart(); }
		
	private:
		RHID3D12Device * _device = nullptr;
		win32::comptr<ID3D12DescriptorHeap> _heap;
		std::vector<std::shared_ptr<RHID3D12ResourceView>> _views;

		ResourcePacketArgs _args;
		uint32_t _unit = 0;
	};
}
