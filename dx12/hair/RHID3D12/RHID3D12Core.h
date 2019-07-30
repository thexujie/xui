#pragma once

#include "RHI/RHI.h"
#include "d3d12/d3d12.h"
#include "RHI/RHI.h"

namespace RHI::RHID3D12
{
	inline DXGI_SWAP_EFFECT FromSwapEffect(SwapEffect effect)
	{
		switch(effect)
		{
		case SwapEffect::Discard:
			return DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;
		case SwapEffect::Sequential:
			return DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_SEQUENTIAL;
		case SwapEffect::FlipSequential:
			return DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		case SwapEffect::FlipDiscard:
			return DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD;
		default:
			return DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;
		}
	}

	inline D3D12_DESCRIPTOR_HEAP_TYPE FromDescriptorHeapType(DescriptorHeapType type)
	{
		switch(type)
		{
		case DescriptorHeapType::Resource:
			return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		case DescriptorHeapType::Sampler:
			return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		case DescriptorHeapType::RenderTargetView:
			return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		case DescriptorHeapType::DepthStencialView: 
			return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		default:
			return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		}
	}

	inline D3D12_HEAP_TYPE FromBufferType(HeapType type)
	{
		switch (type)
		{
		case HeapType::Default:
			return D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;
		case HeapType::Upload:
			return D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
		case HeapType::Readback:
			return D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_READBACK;
		default:
			return D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;
		}
	}

	inline D3D12_COMMAND_LIST_TYPE FromCommandType(CommandType type)
	{
		switch (type)
		{
		case CommandType::Direct:
			return D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;
		case CommandType::Bundle:
			return D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_BUNDLE;
		case CommandType::Compute:
			return D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COMPUTE;
		case CommandType::Copy:
			return D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COPY;
		default:
			return D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;
		}
	}

	inline D3D12_RESOURCE_DIMENSION FromBufferDimension(BufferDimension dimension)
	{
		switch (dimension)
		{
		case BufferDimension::None:
			return D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_UNKNOWN;
		case BufferDimension::Raw:
			return D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
		case BufferDimension::Texture1D:
			return D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE1D;
		case BufferDimension::Texture2D:
			return D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		case BufferDimension::Texture3D:
			return D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE3D;
		default:
			return D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_UNKNOWN;
		}
	}

	inline D3D12_RESOURCE_FLAGS FromResourceFlags(ResourceFlags flags)
	{
		core::bitflag<D3D12_RESOURCE_FLAGS> result;
		result.set(D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, flags.any(ResourceFlag::AllowRenderTarget));
		result.set(D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, flags.any(ResourceFlag::AllowDepthStencial));
		result.set(D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, flags.any(ResourceFlag::AllowUnorderdAccess));
		result.set(D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER, flags.any(ResourceFlag::AllowCrossAdapter));
		result.set(D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE, flags.any(ResourceFlag::DenyShaderResource));
		return result.get();
	}

	inline D3D12_HEAP_FLAGS FromHeapFlags(HeapFlags flags)
	{
		core::bitflag<D3D12_HEAP_FLAGS> result;
		result.set(D3D12_HEAP_FLAG_SHARED, flags.any(BufferFlag::Shader));
		result.set(D3D12_HEAP_FLAG_ALLOW_WRITE_WATCH, flags.any(BufferFlag::WriteWatch));
		result.set(D3D12_HEAP_FLAG_SHARED_CROSS_ADAPTER, flags.any(BufferFlag::CrossAdapter));
		result.set(D3D12_HEAP_FLAG_DENY_BUFFERS, flags.any(BufferFlag::DenyBuffers));
		return result.get();
	}

	inline D3D12_CPU_PAGE_PROPERTY FromCPUPageProperty(CPUPageProperty prop)
	{
		switch(prop)
		{
		case CPUPageProperty::None:
			return D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		case CPUPageProperty::NA:
			return D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE;
		case CPUPageProperty::WriteCombine:
			return D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE;
		case CPUPageProperty::WriteBack:
			return D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		default:
			return D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		}
	}

	inline D3D12_MEMORY_POOL FromMemoryPool(MemoryPool pool)
	{
		switch(pool)
		{
		case MemoryPool::None:
			return D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
		case MemoryPool::L0:
			return D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_L0;
		case MemoryPool::L1:
			return D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_L1;
		default:
			return D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
		}
	}

	inline D3D12_RESOURCE_STATES FromResourceStates(ResourceStates states)
	{
		core::bitflag<D3D12_RESOURCE_STATES> result;
		result.set(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, states.any(ResourceState::VertexBuffer | ResourceState::ConstBuffer));
		result.set(D3D12_RESOURCE_STATE_INDEX_BUFFER, states.any(ResourceState::IndexBuffer));
		result.set(D3D12_RESOURCE_STATE_RENDER_TARGET, states.any(ResourceState::RenderTarget));
		result.set(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, states.any(ResourceState::UnorderedAccess));
		result.set(D3D12_RESOURCE_STATE_DEPTH_WRITE, states.any(ResourceState::DepthWrite));
		result.set(D3D12_RESOURCE_STATE_DEPTH_READ, states.any(ResourceState::DepthRead));
		result.set(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, states.any(
			ResourceState::VertexShaderRerources |
			ResourceState::HullShaderRerources |
			ResourceState::DomainShaderRerources |
			ResourceState::GeometryShaderRerources |
			ResourceState::ComputerShaderRerources));
		result.set(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, states.any(ResourceState::PixelShaderRerources));
		result.set(D3D12_RESOURCE_STATE_COPY_SOURCE, states.any(ResourceState::CopySource));
		result.set(D3D12_RESOURCE_STATE_COPY_DEST, states.any(ResourceState::CopyDest));
		result.set(D3D12_RESOURCE_STATE_RESOLVE_SOURCE, states.any(ResourceState::ResolveSource));
		result.set(D3D12_RESOURCE_STATE_RESOLVE_DEST, states.any(ResourceState::ResolveDest));
		result.set(D3D12_RESOURCE_STATE_PRESENT, states.any(ResourceState::Present));
		return result.get();
	}
}
