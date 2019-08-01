#pragma once

#include "RHI/RHI.h"
#include "d3d12/d3d12.h"
#include "RHID3D12Utils.h"

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

	inline D3D12_RESOURCE_DIMENSION FromResourceDimension(ResourceDimension dimension)
	{
		switch (dimension)
		{
		case ResourceDimension::None:
			return D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_UNKNOWN;
		case ResourceDimension::Raw:
			return D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
		case ResourceDimension::Texture1D:
			return D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE1D;
		case ResourceDimension::Texture2D:
			return D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		case ResourceDimension::Texture3D:
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
		result.set(D3D12_HEAP_FLAG_SHARED, flags.any(HeapFlag::Shader));
		result.set(D3D12_HEAP_FLAG_ALLOW_WRITE_WATCH, flags.any(HeapFlag::WriteWatch));
		result.set(D3D12_HEAP_FLAG_SHARED_CROSS_ADAPTER, flags.any(HeapFlag::CrossAdapter));
		result.set(D3D12_HEAP_FLAG_DENY_BUFFERS, flags.any(HeapFlag::DenyBuffers));
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

	inline D3D12_SRV_DIMENSION FromResourceViewDimension(ResourceViewDimension dimnesion)
	{
		switch(dimnesion)
		{
		case ResourceViewDimension::None: 
			return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_UNKNOWN;
		case ResourceViewDimension::Buffer: 
			return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_BUFFER;
		case ResourceViewDimension::Texture1D: 
			return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE1D;
		case ResourceViewDimension::Texture1DArray: 
			return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
		case ResourceViewDimension::Texture2D: 
			return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
		case ResourceViewDimension::Texture2DArray: 
			return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		case ResourceViewDimension::Texture3D: 
			return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE3D;
		case ResourceViewDimension::TextureCube: 
			return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURECUBE;
		case ResourceViewDimension::TextureCubeArray: 
			return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
		default:
			return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_UNKNOWN;
		}
	}

	inline D3D12_DESCRIPTOR_HEAP_TYPE FromDescriptorHeapType(DescriptorHeapType type)
	{
		switch (type)
		{
		case DescriptorHeapType::None: 
		case DescriptorHeapType::ConstBuffer:
		case DescriptorHeapType::ShaderResource:
		case DescriptorHeapType::UnorderedAccess:
			return D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		case DescriptorHeapType::Sampler:
			return D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		case DescriptorHeapType::RenderTarget:
			return D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		case DescriptorHeapType::DepthStencial:
			return D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		default:
			return D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		}
	}

	inline D3D12_DESCRIPTOR_HEAP_FLAGS FromDescriptorHeapFlags(DescriptorHeapFlags flags)
	{
		core::bitflag<D3D12_DESCRIPTOR_HEAP_FLAGS> result;
		result.set(D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, flags.any(DescriptorHeapFlag::ShaderVisible));
		return result;
	}

	inline D3D12_DESCRIPTOR_RANGE_TYPE FromDescripteorRangeType(DescripteorRangeType type)
	{
		switch(type)
		{
		//case DescripteorRangeType::Table:
			//return D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		case DescripteorRangeType::ConstBuffer: 
			return D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		case DescripteorRangeType::ShaderResource:
			return D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		case DescripteorRangeType::UnorderedAccess:
			return D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		case DescripteorRangeType::Sampler:
			return D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		default:
			return D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		}
	}

	//inline D3D12_ROOT_PARAMETER_TYPE FromDescripteorRangeType(DescripteorRangeType type)
	//{
	//	switch (type)
	//	{
	//		//case DescripteorRangeType::Table:
	//			//return D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//	case DescripteorRangeType::ConstBuffer:
	//		return D3D12_ROOT_PARAMETER_TYPE_CBV;
	//	case DescripteorRangeType::ShaderResource:
	//		return D3D12_ROOT_PARAMETER_TYPE_SRV;
	//	case DescripteorRangeType::UnorderedAccess:
	//		return D3D12_ROOT_PARAMETER_TYPE_UAV;
	//	default:
	//		return D3D12_ROOT_PARAMETER_TYPE_CBV;
	//	}
	//}

	inline D3D12_SHADER_VISIBILITY FromShader(Shader shader)
	{
		switch(shader)
		{
		case All: 
			return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;
		case Vertex: 
			return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_VERTEX;
		case Hull: 
			return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_HULL;
		case Domain: 
			return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_DOMAIN;
		case Geoetry: 
			return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_GEOMETRY;
		case Pixel: 
			return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL;
		default:
			return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;
		}
	}

	inline D3D12_BLEND FromBlend(Blend blend)
	{
		switch(blend)
		{
		case Blend::None:
			return D3D12_BLEND::D3D12_BLEND_ZERO;
		case Blend::Zero: 
			return D3D12_BLEND::D3D12_BLEND_ZERO;
		case Blend::One: 
			return D3D12_BLEND::D3D12_BLEND_ONE;
		case Blend::SrcColor: 
			return D3D12_BLEND::D3D12_BLEND_SRC_COLOR;
		case Blend::SrcColorInv: 
			return D3D12_BLEND::D3D12_BLEND_INV_SRC_COLOR;
		case Blend::SrcAlpha: 
			return D3D12_BLEND::D3D12_BLEND_SRC_ALPHA;
		case Blend::SrcAlphaInv: 
			return D3D12_BLEND::D3D12_BLEND_INV_SRC_ALPHA;
		case Blend::DestAlpha:
			return D3D12_BLEND::D3D12_BLEND_DEST_ALPHA;
		case Blend::DestAlphaInv:
			return D3D12_BLEND::D3D12_BLEND_INV_DEST_ALPHA;
		case Blend::DestColor: 
			return D3D12_BLEND::D3D12_BLEND_DEST_COLOR;
		case Blend::DestColorInv: 
			return D3D12_BLEND::D3D12_BLEND_INV_DEST_COLOR;
		case Blend::BlendFactor: 
			return D3D12_BLEND::D3D12_BLEND_BLEND_FACTOR;
		case Blend::BlendFactorInv: 
			return D3D12_BLEND::D3D12_BLEND_INV_BLEND_FACTOR;
		default:
			return D3D12_BLEND::D3D12_BLEND_ZERO;
		}
	}

	inline D3D12_BLEND_OP FromBlendOP(BlendOP op)
	{
		switch(op)
		{
		case BlendOP::Add: 
			return D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
		case BlendOP::Subtract: 
			return D3D12_BLEND_OP::D3D12_BLEND_OP_SUBTRACT;
		case BlendOP::Min: 
			return D3D12_BLEND_OP::D3D12_BLEND_OP_MIN;
		case BlendOP::Max: 
			return D3D12_BLEND_OP::D3D12_BLEND_OP_MAX;
		default:
			return D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
		}
	}

	inline D3D12_PRIMITIVE_TOPOLOGY_TYPE FromTopologyType(TopologyType type)
	{
		switch(type)
		{
		case TopologyType::None:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
		case TopologyType::Point:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		case TopologyType::Line:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		case TopologyType::Triangle:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		case TopologyType::Patch:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
		default:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
		}
	}

	inline DXGI_FORMAT FromPixelFormat(core::pixelformat pixelformat)
	{
		switch (pixelformat)
		{
		case core::pixelformat::bgrx: return DXGI_FORMAT_B8G8R8X8_UNORM;
		case core::pixelformat::bgra: return DXGI_FORMAT_B8G8R8A8_UNORM;
		case core::pixelformat::nv12: return DXGI_FORMAT_NV12;
		case core::pixelformat::yuy2: return DXGI_FORMAT_420_OPAQUE;
		case core::pixelformat::p010: return DXGI_FORMAT_P010;
		default: return DXGI_FORMAT_UNKNOWN;
		}
	}

	inline D3D12_FILTER FromFilter(Filter filter)
	{
		switch (filter)
		{
		case Filter::Point: 
			return D3D12_FILTER::D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
		case Filter::Liner: 
			return D3D12_FILTER::D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		default:
			return D3D12_FILTER::D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
		}
	}

	inline D3D12_TEXTURE_ADDRESS_MODE FromAddressMode(AddressMode mode)
	{
		switch(mode)
		{
		case AddressMode::Clamp: 
			return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		case AddressMode::Wrap: 
			return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		case AddressMode::Mirror: 
			return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		case AddressMode::Border: 
			return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		default:
			return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		}
	}

	inline D3D12_COMPARISON_FUNC FromComparison(Comparison cmp)
	{
		switch(cmp)
		{
		case Comparison::None: 
			return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NEVER;
		case Comparison::Always: 
			return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_ALWAYS;
		case Comparison::Less: 
			return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS;
		case Comparison::LessEqual: 
			return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS_EQUAL;
		case Comparison::Equal: 
			return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_EQUAL;
		case Comparison::GreaterEqual: 
			return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		case Comparison::Greater: 
			return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER;
		case Comparison::NotEqual: 
			return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NOT_EQUAL;
		default:
			return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NEVER;
		}
	}
}
