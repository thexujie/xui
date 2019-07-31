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
		RHID3D12ResourceView(const D3D12_CPU_DESCRIPTOR_HANDLE & handle) :_handle(handle) {}
		virtual ~RHID3D12ResourceView() = default;

	public:
		D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle() const { return _handle; }
		void SetCPUDescriptorHandle(const D3D12_CPU_DESCRIPTOR_HANDLE & handle) { _handle = handle; }

	protected:
		D3D12_CPU_DESCRIPTOR_HANDLE _handle = {};
	};

	class RHID3D12ResourceView2 : public RHID3D12ResourceView
	{
	public:
		RHID3D12ResourceView2(RHID3D12Device * device) : _device(device) {}
		virtual ~RHID3D12ResourceView2() = default;

		core::error Create(const RHID3D12Resource * resource, const ResourceViewParams & params);

	public:
		D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle();

	private:
		RHID3D12Device * _device = nullptr;
		ResourceViewParams _params;

		win32::comptr<ID3D12DescriptorHeap> _heap;
	};
}
