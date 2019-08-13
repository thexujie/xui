#pragma once

#include "core/core.h"
#include "drawing/drawing.h"

namespace RHI
{
	class RHIObject
	{
	public:
		RHIObject() = default;
		virtual ~RHIObject() = default;
		virtual void SetName(const std::u8string & name) {}
	};
	
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

	using CommandQueueFlags = core::bitflag<CommandQueueFlag>;

	enum class SwapEffect
	{
		Discard = 0,
		Sequential,
		FlipSequential,
		FlipDiscard,
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

	enum class ResourceDimension
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
	using ResourceFlags = core::bitflag<ResourceFlag>;

	enum class HeapFlag
	{
		None = 0,
		Shader = 0x1,
		WriteWatch = 0x2,
		CrossAdapter = 0x4,
		DenyBuffers = 0x1000,
	};
	using HeapFlags = core::bitflag<HeapFlag>;


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
		GenericRead = 0x200000,
	};
	
	using ResourceStates = core::bitflag<ResourceState>;

	class RHIDeviceObject : public RHIObject
	{
	public:
		RHIDeviceObject() = default;
		virtual ~RHIDeviceObject() = default;
	};

	struct ResourceArgs
	{
		struct
		{
			HeapType type = HeapType::Default;
			HeapFlags flags = nullptr;
			CPUPageProperty cpupageproperty = CPUPageProperty::None;
			MemoryPool memorypool = MemoryPool::None;
		}heap;

		ResourceDimension dimension = ResourceDimension::None;
		ResourceFlags flags = nullptr;
		core::format format = core::format::none;

		uint32_t alignment = 0;
		core::sizeu size = {0, 1};
		uint16_t depth = 1;
		uint16_t miplevels = 1;
		uint32_t MSAA = 1;
		uint32_t MSAAQuality = 0;

		// ³õÊ¼×´Ì¬
		ResourceStates states = ResourceState::None;
		
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

	enum class ResourceViewDimension
	{
		None = 0,
		Buffer,
		Texture1D,
		Texture1DArray,
		Texture2D,
		Texture2DArray,
		Texture3D,
		TextureCube,
		TextureCubeArray,
	};

	enum class DescriptorHeapType
	{
		None = 0,
		ConstBuffer,
		ShaderResource,
		UnorderedAccess,
		Sampler,
		RenderTarget,
		DepthStencial,
	};

	enum class ResourcePacketType
	{
		Resource = 0,
		Sampler,
		RenderTarget,
		DepthStencil,
	};
	
	enum class ResourcePacketFlag
	{
		None = 0,
		ShaderVisible,
	};
	using ResourcePacketFlags = core::bitflag<ResourcePacketFlag>;

	enum class ResourceType
	{
		None= 0,
		ConstBuffer,
		ShaderResource,
		UnorderedAccess,
	};
	
	struct ResourceViewArgs
	{
		ResourceType type = ResourceType::None;
		struct
		{
			core::format format = core::format::none;
			ResourceViewDimension dimension = ResourceViewDimension::None;
			uint32_t miplevels = 1;
			struct
			{
				uint32_t firstElement = 0;
				uint32_t numElements = 0;
				uint32_t stride = 0;
			}buffer;
		}resource;

		static ResourceViewArgs CBuffer()
		{
			ResourceViewArgs args = {};
			args.type = ResourceType::ConstBuffer;
			return args;
		}
		
		static ResourceViewArgs Unordered(uint32_t stride, uint32_t numElements)
		{
			ResourceViewArgs args = {};
			args.type = ResourceType::UnorderedAccess;
			args.resource.dimension = ResourceViewDimension::Buffer;
			args.resource.buffer.numElements = numElements;
			args.resource.buffer.stride = stride;
			return args;
		}

		static ResourceViewArgs Shader(uint32_t stride, uint32_t numElements)
		{
			ResourceViewArgs args = {};
			args.type = ResourceType::ShaderResource;
			args.resource.dimension = ResourceViewDimension::Buffer;
			args.resource.buffer.numElements = numElements;
			args.resource.buffer.stride = stride;
			return args;
		}
	};
	
	struct ResourcePacketArgs
	{
		ResourcePacketType type = ResourcePacketType::Resource;
		uint32_t capacity = 0;
		ResourcePacketFlags flags = nullptr;
	};

	struct RenderTargetArgs
	{
		void * hwnd = nullptr;
		CommandQueueFlags queueFlags;
		core::format format = core::format::bgra;
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

	enum Shader
	{
		All = 0,
		Vertex,
		Hull,
		Domain,
		Geoetry,
		Pixel,
		Compute
	};


	struct PipelineState
	{
		
	};

	enum class DescriptorRangeType
	{
		ConstBuffer = 0,
		ShaderResource,
		UnorderedAccess,
		Sampler,
	};

	enum class DescriptorFlag
	{
		None = 0,
		Static,
		Volatile,
	};
	using DescriptorFlags = core::bitflag<DescriptorFlag>;

	enum class Filter
	{
		Point = 0,
		Liner,
	};

	enum class AddressMode
	{
		Clamp = 0,
		Wrap,
		Mirror,
		Border,
	};

	enum class Comparison
	{
		None = 0,
		Always,
		Less,
		LessEqual,
		Equal,
		GreaterEqual,
		Greater,
		NotEqual,
	};

	enum class Blend
	{
		None = 0,
		Zero,
		One,
		SrcColor,
		SrcColorInv,
		SrcAlpha,
		SrcAlphaInv,
		DestColor,
		DestColorInv,
		DestAlpha,
		DestAlphaInv,
		BlendFactor,
		BlendFactorInv,
	};

	enum class BlendOP
	{
		Add = 0,
		Subtract,
		Min,
		Max,
	};

	enum class CullMode
	{
		None = 0,
		Front,
		Back
	};

	enum class WriteMask
	{
		None = 0,
		Red = 0x1,
		Green = 0x2,
		Blue = 0x4,
		Alpha = 0x8,
		All = Red | Green | Blue | Alpha,
	};
	using WriteMasks = core::bitflag<WriteMask>;
	
	enum class Topology
	{
		None = 0,
		PointList,
		LineList,
		LineStrip,
		TriangleList,
		TriangleStrip,
		Point1PatchList,
		Point2PatchList,
		Point3PatchList,
		Point4PatchList,
		Point5PatchList,
		Point6PatchList,
		Point7PatchList,
		Point8PatchList,
		Point9PatchList,
		Point10PatchList,
		Point11PatchList,
		Point12PatchList,
		Point13PatchList,
		Point14PatchList,
		Point15PatchList,
		Point16PatchList,
	};
	
	enum class TopologyType
	{
		None = 0,
		Point,
		Line,
		Triangle,
		Patch,
	};

	struct SamplerArgs
	{
		uint32_t shaderRegister = 0;
		uint32_t registerSpace = 0;

		Filter filter = Filter::Point;
		AddressMode addressU = AddressMode::Clamp;
		AddressMode addressV = AddressMode::Clamp;
		AddressMode addressW = AddressMode::Clamp;
		float mipLODBias = 0;
		uint32_t anisotropy = 0;
		Comparison comparison = Comparison::None;
		float minLOD = 0;
		float maxLOD = 0;
	};

	struct PipelineStateTableRange
	{
		DescriptorRangeType type = DescriptorRangeType::ConstBuffer;
		uint32_t numDescriptor = 1;
		uint32_t shaderRegister = 0;
		uint32_t registerSpace = 0;
		uint32_t packetOffset = core::uint32_max;
		DescriptorFlags flags = DescriptorFlag::None;
	};

	struct PipelineStateTable
	{
		Shader shader = Shader::All;
		std::vector<PipelineStateTableRange> ranges;
	};

	constexpr uint32_t RenderTargetMax = 8;

	struct InputElement
	{
		std::string name;
		core::format format = core::format::abgr;
		uint32_t offset = 0;
	};
	
	struct PipelineStateArgs
	{
		TopologyType topology = TopologyType::Triangle;
		
		std::u8string VS;
		std::string VSMain;
		std::u8string HS;
		std::string HSMain;
		std::u8string DS;
		std::string DSMain;
		std::u8string GS;
		std::string GSMain;
		std::u8string PS;
		std::string PSMain;
		std::u8string CS;
		std::string CSMain;

		std::vector<InputElement> elements;
		std::vector<PipelineStateTable> tables;
		std::vector<SamplerArgs> samplers;
		
		uint32_t ntargets = 1;
		core::format formats[RenderTargetMax] = { core::format::bgra };

		uint32_t SampleCount = 1;
		uint32_t SampleQuality = 0;

		struct
		{
			bool alphaToCoverage = false;
			bool independentBlend = false;

			struct
			{
				bool enable = false;
				Blend srcColor= Blend::One;
				Blend destColor= Blend::Zero;
				BlendOP colorOP = BlendOP::Add;
				Blend srcAlpha = Blend::One;
				Blend destAlpha = Blend::Zero;
				BlendOP alphaOP = BlendOP::Add;
				WriteMasks writeMasks = WriteMask::All;
			}targets[RenderTargetMax];
		}blend;

		struct
		{
			bool wireframe = false;
			CullMode cullmode = CullMode::None;
			bool CCW = false;
			int depthBias = 0;
			bool depthClip = true;
			bool MSAA = false;
		}rasterize;

		struct
		{
			bool depth = false;
			bool stencil = false;
		}depthstencil;

		uint32_t samplemask = 0xffffffff;

	};
}
