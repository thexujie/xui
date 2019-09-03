#pragma once

#include "RHI/RHI.h"
#include "win32/win32.h"
#include <vulkan.h>
#include <vulkan_win32.h>

namespace RHI::RHIVulkan
{
	class RHIVulkanFactory;
	class RHIVulkanDevice;

	inline VkFormat FromFormat(core::format format)
	{
		switch(format)
		{
		case core::format::bgra: return VK_FORMAT_B8G8R8A8_UNORM;
		case core::format::float1: return VK_FORMAT_R32_SFLOAT;
		case core::format::float2: return VK_FORMAT_R32G32_SFLOAT;
		case core::format::float3: return VK_FORMAT_R32G32B32_SFLOAT;
		case core::format::float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
		case core::format::int1: return VK_FORMAT_R32_SINT;
		case core::format::int2: return VK_FORMAT_R32G32_SINT;
		case core::format::int3: return VK_FORMAT_R32G32B32_SINT;
		case core::format::int4: return VK_FORMAT_R32G32B32A32_SINT;
		case core::format::uint1: return VK_FORMAT_R32_UINT;
		case core::format::uint2: return VK_FORMAT_R32G32_UINT;
		case core::format::uint3: return VK_FORMAT_R32G32B32_UINT;
		case core::format::uint4: return VK_FORMAT_R32G32B32A32_UINT;
		default: return VK_FORMAT_UNDEFINED;
		}
	}

	inline VkBufferUsageFlagBits FromResourceUsages(ResourceUsages usages)
	{
		core::bitflag<VkBufferUsageFlagBits> result;
		result.set(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, usages.any(ResourceUsage::ConstBuffer));
		result.set(VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT, usages.any(ResourceUsage::ShaderResource));
		result.set(VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT, usages.any(ResourceUsage::UnorderdResource));
		result.set(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, usages.any(ResourceUsage::VertexBuffer));
		result.set(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, usages.any(ResourceUsage::IndexBuffer));
		return result;
	}
}
