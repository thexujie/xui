#pragma once

#include "RHI/RHI.h"
#include "win32/d3d11.h"
#include "RHID3D11Utils.h"

namespace RHI::RHID3D11
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

	inline DXGI_FORMAT FromFormat(core::format pixelformat)
	{
		switch (pixelformat)
		{
		case core::format::bgrx: return DXGI_FORMAT_B8G8R8X8_UNORM;
		case core::format::bgra: return DXGI_FORMAT_B8G8R8A8_UNORM;
		case core::format::nv12: return DXGI_FORMAT_NV12;
		case core::format::yuy2: return DXGI_FORMAT_420_OPAQUE;
		case core::format::p010: return DXGI_FORMAT_P010;
		case core::format::float1: return DXGI_FORMAT_R32_FLOAT;
		case core::format::float2: return DXGI_FORMAT_R32G32_FLOAT;
		case core::format::float3: return DXGI_FORMAT_R32G32B32_FLOAT;
		case core::format::float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case core::format::int1: return DXGI_FORMAT_R32_SINT;
		case core::format::int2: return DXGI_FORMAT_R32G32_SINT;
		case core::format::int3: return DXGI_FORMAT_R32G32B32_SINT;
		case core::format::int4: return DXGI_FORMAT_R32G32B32A32_SINT;
		case core::format::uint1: return DXGI_FORMAT_R32_UINT;
		case core::format::uint2: return DXGI_FORMAT_R32G32_UINT;
		case core::format::uint3: return DXGI_FORMAT_R32G32B32_UINT;
		case core::format::uint4: return DXGI_FORMAT_R32G32B32A32_UINT;
		default: return DXGI_FORMAT_UNKNOWN;
		}
	}

	inline D3D11_PRIMITIVE_TOPOLOGY FromTopology(Topology topology)
	{
		switch (topology)
		{
		case Topology::None:
			return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		case Topology::PointList:
			return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		case Topology::LineList:
			return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		case Topology::LineStrip:
			return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
		case Topology::TriangleList:
			return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case Topology::TriangleStrip:
			return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		case Topology::Point1PatchList: return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST;
		case Topology::Point2PatchList: return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST;
		case Topology::Point3PatchList: return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
		case Topology::Point4PatchList: return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
		case Topology::Point5PatchList: return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST;
		case Topology::Point6PatchList: return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST;
		case Topology::Point7PatchList: return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST;
		case Topology::Point8PatchList: return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST;
		case Topology::Point9PatchList: return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST;
		case Topology::Point10PatchList: return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST;
		case Topology::Point11PatchList: return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST;
		case Topology::Point12PatchList: return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST;
		case Topology::Point13PatchList: return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST;
		case Topology::Point14PatchList: return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST;
		case Topology::Point15PatchList: return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST;
		case Topology::Point16PatchList: return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST;
		default:
			return D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		}
	}

	inline D3D11_BLEND FromBlend(Blend blend)
	{
		switch (blend)
		{
		case Blend::None:
			return D3D11_BLEND::D3D11_BLEND_ZERO;
		case Blend::Zero:
			return D3D11_BLEND::D3D11_BLEND_ZERO;
		case Blend::One:
			return D3D11_BLEND::D3D11_BLEND_ONE;
		case Blend::SrcColor:
			return D3D11_BLEND::D3D11_BLEND_SRC_COLOR;
		case Blend::SrcColorInv:
			return D3D11_BLEND::D3D11_BLEND_INV_SRC_COLOR;
		case Blend::SrcAlpha:
			return D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		case Blend::SrcAlphaInv:
			return D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		case Blend::DestAlpha:
			return D3D11_BLEND::D3D11_BLEND_DEST_ALPHA;
		case Blend::DestAlphaInv:
			return D3D11_BLEND::D3D11_BLEND_INV_DEST_ALPHA;
		case Blend::DestColor:
			return D3D11_BLEND::D3D11_BLEND_DEST_COLOR;
		case Blend::DestColorInv:
			return D3D11_BLEND::D3D11_BLEND_INV_DEST_COLOR;
		case Blend::BlendFactor:
			return D3D11_BLEND::D3D11_BLEND_BLEND_FACTOR;
		case Blend::BlendFactorInv:
			return D3D11_BLEND::D3D11_BLEND_INV_BLEND_FACTOR;
		default:
			return D3D11_BLEND::D3D11_BLEND_ZERO;
		}
	}

	inline D3D11_BLEND_OP FromBlendOP(BlendOP op)
	{
		switch (op)
		{
		case BlendOP::Add:
			return D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		case BlendOP::Subtract:
			return D3D11_BLEND_OP::D3D11_BLEND_OP_SUBTRACT;
		case BlendOP::Min:
			return D3D11_BLEND_OP::D3D11_BLEND_OP_MIN;
		case BlendOP::Max:
			return D3D11_BLEND_OP::D3D11_BLEND_OP_MAX;
		default:
			return D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		}
	}

	inline D3D11_CULL_MODE FromCullMode(CullMode mode)
	{
		switch (mode)
		{
		case CullMode::None:
			return D3D11_CULL_MODE::D3D11_CULL_NONE;
		case CullMode::Front:
			return D3D11_CULL_MODE::D3D11_CULL_FRONT;
		case CullMode::Back:
			return D3D11_CULL_MODE::D3D11_CULL_BACK;
		default:
			return D3D11_CULL_MODE::D3D11_CULL_NONE;
		}
	}

	inline D3D11_COLOR_WRITE_ENABLE FromWriteMasks(WriteMasks masks)
	{
		core::bitflag<D3D11_COLOR_WRITE_ENABLE> result;
		result.set(D3D11_COLOR_WRITE_ENABLE_RED, masks.any(WriteMask::Red));
		result.set(D3D11_COLOR_WRITE_ENABLE_GREEN, masks.any(WriteMask::Green));
		result.set(D3D11_COLOR_WRITE_ENABLE_BLUE, masks.any(WriteMask::Blue));
		result.set(D3D11_COLOR_WRITE_ENABLE_ALPHA, masks.any(WriteMask::Alpha));
		return result;
	}

	inline D3D11_USAGE FromHeapType(HeapType type)
	{
		switch (type)
		{
		case HeapType::Default:
			return D3D11_USAGE_DEFAULT;
		case HeapType::Upload:
			return D3D11_USAGE_STAGING;
		case HeapType::Readback:
			return D3D11_USAGE_DYNAMIC;
		default:
			return D3D11_USAGE_DEFAULT;
		}
	}

	inline D3D11_SRV_DIMENSION FromResourceViewDimension(ResourceViewDimension dimnesion)
	{
		switch (dimnesion)
		{
		case ResourceViewDimension::None:
			return D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_UNKNOWN;
		case ResourceViewDimension::Buffer:
			return D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_BUFFER;
		case ResourceViewDimension::Texture1D:
			return D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE1D;
		case ResourceViewDimension::Texture1DArray:
			return D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
		case ResourceViewDimension::Texture2D:
			return D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
		case ResourceViewDimension::Texture2DArray:
			return D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		case ResourceViewDimension::Texture3D:
			return D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE3D;
		case ResourceViewDimension::TextureCube:
			return D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURECUBE;
		case ResourceViewDimension::TextureCubeArray:
			return D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
		default:
			return D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_UNKNOWN;
		}
	}

	inline D3D11_UAV_DIMENSION FromResourceViewDimension_UAV(ResourceViewDimension dimnesion)
	{
		switch (dimnesion)
		{
		case ResourceViewDimension::None:
			return D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_UNKNOWN;
		case ResourceViewDimension::Buffer:
			return D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_BUFFER;
		case ResourceViewDimension::Texture1D:
			return D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE1D;
		case ResourceViewDimension::Texture1DArray:
			return D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
		case ResourceViewDimension::Texture2D:
			return D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE2D;
		case ResourceViewDimension::Texture2DArray:
			return D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
		case ResourceViewDimension::Texture3D:
			return D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE3D;
		default:
			return D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_UNKNOWN;
		}
	}

	inline D3D11_BIND_FLAG FromResourceFlags(ResourceFlags flags)
	{
		core::bitflag<D3D11_BIND_FLAG> result;
		result.set(D3D11_BIND_RENDER_TARGET, flags.any(ResourceFlag::RenderTarget));
		result.set(D3D11_BIND_DEPTH_STENCIL, flags.any(ResourceFlag::DepthStencial));
		result.set(D3D11_BIND_CONSTANT_BUFFER, flags.any(ResourceFlag::ConstBuffer));
		result.set(D3D11_BIND_SHADER_RESOURCE, flags.any(ResourceFlag::ShaderResource));
		result.set(D3D11_BIND_UNORDERED_ACCESS, flags.any(ResourceFlag::UnorderdResource));
		result.set(D3D11_BIND_VERTEX_BUFFER, flags.any(ResourceFlag::VertexBuffer));
		result.set(D3D11_BIND_INDEX_BUFFER, flags.any(ResourceFlag::IndexBuffer));
		return result.get();
	}
}
