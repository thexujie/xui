#pragma once

#include "RHI/RHI.h"
#include "RHID3D12Device.h"

namespace RHI::RHID3D12
{
	class RHID3D12Resource;

	class RHID3D12ResourceView : public RHIResourceView
	{
	public:
		RHID3D12ResourceView() = default;
		RHID3D12ResourceView(const D3D12_CPU_DESCRIPTOR_HANDLE & handle) :_CPUHandle(handle) {}
		RHID3D12ResourceView(const D3D12_GPU_DESCRIPTOR_HANDLE & handle) :_GPUHandle(handle) {}
		RHID3D12ResourceView(const D3D12_CPU_DESCRIPTOR_HANDLE & CPUHandle, const D3D12_GPU_DESCRIPTOR_HANDLE & GPUHandle) : _CPUHandle(CPUHandle), _GPUHandle(GPUHandle) {}
		virtual ~RHID3D12ResourceView() = default;

	public:
		D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle() const { return _CPUHandle; }
		D3D12_GPU_DESCRIPTOR_HANDLE GPUDescriptorHandle() const { return _GPUHandle; }
		void SetCPUDescriptorHandle(const D3D12_CPU_DESCRIPTOR_HANDLE & handle) { _CPUHandle = handle; }
		void SetGPUDescriptorHandle(const D3D12_GPU_DESCRIPTOR_HANDLE & handle) { _GPUHandle = handle; }

	protected:
		D3D12_CPU_DESCRIPTOR_HANDLE _CPUHandle = {};
		D3D12_GPU_DESCRIPTOR_HANDLE _GPUHandle = {};
	};
	
}
