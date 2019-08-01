#pragma once

#include "RHI/RHI.h"
#include "RHID3D12Device.h"

namespace RHI::RHID3D12
{
	class RHID3D12Resource;

	class RHID3D12RenderTargetView : public RHIResourceView
	{
	public:
		RHID3D12RenderTargetView() = default;
		RHID3D12RenderTargetView(const D3D12_CPU_DESCRIPTOR_HANDLE & handle) :_handle(handle) {}
		virtual ~RHID3D12RenderTargetView() = default;

	public:
		D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle() const { return _handle; }
		void SetCPUDescriptorHandle(const D3D12_CPU_DESCRIPTOR_HANDLE & handle) { _handle = handle; }

	protected:
		D3D12_CPU_DESCRIPTOR_HANDLE _handle = {};
	};
	
	class RHID3D12ResourceView : public RHIResourceView
	{
	public:
		virtual ID3D12DescriptorHeap * DescriptorHeap() = 0;
		virtual D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle() = 0;
		virtual D3D12_GPU_DESCRIPTOR_HANDLE GPUDescriptorHandle() = 0;
	};

	class RHID3D12ShaderResourceView : public RHID3D12ResourceView
	{
	public:
		RHID3D12ShaderResourceView(RHID3D12Device * device) : _device(device) {}
		virtual ~RHID3D12ShaderResourceView() = default;

		core::error Create(const RHID3D12Resource * resource, const ShaderResourceViewArgs & args);

	public:
		ID3D12DescriptorHeap * DescriptorHeap() { return _heap.get(); }
		D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle() { return _heap->GetCPUDescriptorHandleForHeapStart(); }
		D3D12_GPU_DESCRIPTOR_HANDLE GPUDescriptorHandle() { return _heap->GetGPUDescriptorHandleForHeapStart(); }

	private:
		RHID3D12Device * _device = nullptr;
		ShaderResourceViewArgs _args;

		win32::comptr<ID3D12DescriptorHeap> _heap;
	};

	class RHID3D12ConstBufferView : public RHID3D12ResourceView
	{
	public:
		RHID3D12ConstBufferView(RHID3D12Device * device) : _device(device) {}
		virtual ~RHID3D12ConstBufferView() = default;

		core::error Create(const RHID3D12Resource * resource, const ConstBufferViewArgs & args);

	public:
		ID3D12DescriptorHeap * DescriptorHeap() { return _heap.get(); }
		D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle() { return _heap->GetCPUDescriptorHandleForHeapStart(); }
		D3D12_GPU_DESCRIPTOR_HANDLE GPUDescriptorHandle() { return _heap->GetGPUDescriptorHandleForHeapStart(); }

	private:
		RHID3D12Device * _device = nullptr;
		ConstBufferViewArgs _args;

		win32::comptr<ID3D12DescriptorHeap> _heap;
	};
}
