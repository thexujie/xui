#pragma once

#include "core/core.h"
#include "drawing/drawing.h"

namespace RHI
{
	enum class CommandType
	{
		Direct = 0,
		Bundle,
		Compute,
		Copy,
	};

	enum class CommandQueueFlag
	{
		None = 0,
		DisableGPUTimeout,
	};

	typedef core::bitflag<CommandQueueFlag> CommandQueueFlags;

	enum class SwapEffect
	{
		Discard = 0,
		Sequential,
		FlipSequential,
		FlipDiscard,
	};

	enum class DescriptorHeapType
	{
		Resource = 0,
		Sampler,
		RenderTargetView,
		DepthStencialView,
	};

	enum class HeapType
	{
		Default = 0,
		Upload,
		Readback,
	};

	enum class CPUPageProperty
	{
		None = 0,
		NA,
		WriteCombine,
		WriteBack,
	};

	enum class MemoryPool
	{
		None,
		L0,
		L1,
	};

	enum class BufferDimension
	{
		None = 0,
		Raw,
		Texture1D,
		Texture2D,
		Texture3D,
	};

	enum class ResourceFlag
	{
		None = 0,
		AllowRenderTarget = 0x1,
		AllowDepthStencial = 0x2,
		AllowUnorderdAccess = 0x4,
		AllowCrossAdapter = 0x8,
		DenyShaderResource = 0x10,
	};
	typedef core::bitflag<ResourceFlag> ResourceFlags;

	enum class BufferFlag
	{
		None = 0,
		Shader = 0x1,
		WriteWatch = 0x2,
		CrossAdapter = 0x4,
		DenyBuffers = 0x1000,
	};
	typedef core::bitflag<BufferFlag> HeapFlags;


	struct RHIAdapterDesc
	{
		std::u8string uri;
		std::u8string name;
	};

	enum class ResourceState
	{
		None = 0,
		VertexBuffer = 0x1,
		IndexBuffer = 0x2,
		ConstBuffer = 0x4,
		RenderTarget = 0x8,
		UnorderedAccess = 0x10,
		DepthWrite = 0x20,
		DepthRead = 0x40,

		VertexShaderRerources = 0x100,
		HullShaderRerources = 0x200,
		DomainShaderRerources = 0x400,
		GeometryShaderRerources = 0x800,
		PixelShaderRerources = 0x1000,
		ComputerShaderRerources = 0x2000,

		CopySource = 0x10000,
		CopyDest = 0x20000,
		ResolveSource = 0x40000,
		ResolveDest = 0x80000,

		Present = 0x100000,
	};
	typedef core::bitflag<ResourceState> ResourceStates;

	class RHIDeviceObject
	{
	public:
		RHIDeviceObject() = default;
		virtual ~RHIDeviceObject() = default;
	};

	class RHIResource
	{
	public:
		RHIResource() = default;
		virtual ~RHIResource() = default;

		virtual void TransitionBarrier(class RHICommandList * cmdlist, ResourceStates states) = 0;
		ResourceStates States() const { return _states; }
	protected:
		ResourceStates _states = ResourceState::None;
	};

	struct BufferParams
	{
		struct
		{
			HeapType type = HeapType::Default;
			HeapFlags flags = nullptr;
			CPUPageProperty cpupageproperty = CPUPageProperty::None;
			MemoryPool memorypool = MemoryPool::None;
		}heap;

		BufferDimension dimension = BufferDimension::None;
		ResourceFlags flags = nullptr;
		// ³õÊ¼×´Ì¬
		ResourceStates states = ResourceState::None;
		core::pixelformat format = core::pixelformat::none;

		uint32_t alignment = 0;
		core::sizeu size = {0, 1};
		uint16_t depth = 1;
		uint16_t miplevels = 1;
		uint32_t MSAA = 1;
		uint32_t MSAAQuality = 0;

		union
		{
			core::color color;
			struct
			{
				float depth;
				uint8_t stencial;
			};
		}clear;

	};

	struct RenderTargetParams
	{
		void * hwnd = nullptr;
		CommandQueueFlags queueFlags;
		core::pixelformat format = core::pixelformat::bgra;
		uint32_t nbuffer = 2;
		SwapEffect swapeffect = SwapEffect::FlipDiscard;
		uint32_t MSAA = 1;
		uint32_t MSAAQuality = 0;
	};

	struct ViewPort
	{
		float top = 0;
		float left = 0;
		float width = 0;
		float height = 0;
		float nearZ = 0;
		float farZ = 0;
	};
}
