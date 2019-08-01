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

		core::error SetShaderResource(RHIResource * resource, const ShaderResourceViewArgs & args);

	public:
		std::shared_ptr<RHIResourceView> CreateShaderResourceView(const RHIResource * resource, const ShaderResourceViewArgs & args) const;
		std::shared_ptr<RHIResourceView> CreateConstBufferView(const RHIResource * resource, const ConstBufferViewArgs & args) const;


	public:
		ID3D12DescriptorHeap * DescriptorHeap() { return _heap.get(); }
		D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle() { return _heap->GetCPUDescriptorHandleForHeapStart(); }
		D3D12_GPU_DESCRIPTOR_HANDLE GPUDescriptorHandle() { return _heap->GetGPUDescriptorHandleForHeapStart(); }
		
	private:
		RHID3D12Device * _device = nullptr;
		win32::comptr<ID3D12DescriptorHeap> _heap;

		ResourcePacketArgs _args;
		uint32_t _unit = 0;
	};
}
