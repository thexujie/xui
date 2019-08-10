﻿#include "stdafx.h"

//#include <ShellScalingAPI.h>
//#pragma comment(lib, "Shcore.lib")

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
//#pragma comment(lib, "E:/github/google/skia/out/x64d/skia.dll.lib")
#pragma comment(lib, "../../externals/skia/bin/x64/skia.dll.lib")
#include "main.h"

#include "main.h"
#include "RHID3D12/RHID3D12.h"
#include "RHID3D12/RHID3D12Factory.h"

#pragma comment(lib, "imm32.lib")
#include <DirectXMath.h>
static std::atomic<bool> rendering = true;

struct Vertex
{
	core::float4 position;
	core::float2 uv;
};

struct SimulateConstantBuffer
{
	core::float4x4 transform;
	float gravityStrength;
	float timeElapse;
	float angularStiffness;
	// 约束计算的积分次数，越多越精确
	int numConstraintIterations;
};

struct SceneConstantBuffer
{
	core::float4x4 transform;
	core::float2 tessFactor;
};

class HairRender
{
public:
	HairRender(HWND hwnd) : _hwnd(hwnd)
	{
		
	}

	~HairRender() = default;


	void OnScrollV(int32_t scroll)
	{
		_view_z += scroll;
	}
	
	void OnKeyDown(uint32_t vkey, uint32_t modify)
	{
		if (vkey == VK_UP)
			_tessFactor.y = std::clamp(_tessFactor.y + 1.0f, 1.0f, 100.0f);
		else if (vkey == VK_DOWN)
			_tessFactor.y = std::clamp(_tessFactor.y - 1.0f, 1.0f, 100.0f);
		else if (vkey == VK_LEFT)
			_tessFactor.x = std::clamp(_tessFactor.x - 1.0f, 1.0f, 100.0f);
		else if (vkey == VK_RIGHT)
			_tessFactor.x = std::clamp(_tessFactor.x + 1.0f, 1.0f, 100.0f);

		switch (vkey)
		{
		case 'R':
			_rotating = !_rotating;
			break;

		case VK_PROCESSKEY:
		case VK_OEM_3:
			_mode = content_mode::bsline | content_mode::line;
			break;
		case '1':
			_mode = content_mode::bsline;
			break;
		case '2':
			_mode = content_mode::line;
			break;
		default:
			break;
		}

		std::wstring title = core::wformat("TessFactor=[", std::setprecision(4), _tessFactor.x, L",", _tessFactor.y, L"]");
		::SetWindowTextW(_hwnd, title.c_str());
	}
	
	void Start()
	{
		_future = std::async(&HairRender::RenderThread, this);
	}
	
	void LoadPipeline()
	{
		RHI.Load();
		std::vector<RHI::RHIAdapterDesc> adapters = RHI.AdapterDescs();
		_device = RHI.CreateDevice(adapters[0].uri);
		_cmdqueue = _device->CreateCommandQueue(RHI::CommandType::Direct, RHI::CommandQueueFlag::None);
		RHI::RenderTargetArgs rtparams = {};
		rtparams.hwnd = _hwnd;
		_rendertarget = _device->CreateRenderTargetForHWND(_cmdqueue.get(), rtparams);
		_cmdallocator = _device->CreateCommandAllocator(RHI::CommandType::Direct);
		_cmdlist = _device->CreateCommandList(RHI::CommandType::Direct);
	}

	void LoadAssets()
	{
		std::u8string path_basic = u8"../data/shaders/basic.hlsl";
		RHI::PipelineStateArgs psoArgs_basic =
		{
			.topology = RHI::TopologyType::Line,
			.VS = path_basic,
			.VSMain = "VSMain",
			.PS = path_basic,
			.PSMain = "PSMain",
			
			.tables =
			{
				RHI::PipelineStateTable
				{
					.shader = RHI::Shader::All,
					.ranges =
					{
						RHI::PipelineStateTableRange
						{
							.type = RHI::DescripteorRangeType::ConstBuffer,
							.shaderRegister = 0
						},
					}
				},
			},
			.samplers = { RHI::SamplerArgs{}},
		};
		_pipelinestate_basic = _device->CreatePipelineState(psoArgs_basic);

		std::u8string path_hair = u8"../data/shaders/hair.hlsl";
		RHI::PipelineStateArgs psoArgs_hair =
		{
			.topology = RHI::TopologyType::Patch,
			.VS = path_hair,
			.VSMain = "VSMain",
			.HS = path_hair,
			.HSMain = "HSMain",
			.DS = path_hair,
			.DSMain = "DSMain",
			//.GS = path,
			//.GSMain = "GSMain",
			.PS = path_hair,
			.PSMain = "PSMain",
			
			.tables =
			{
				RHI::PipelineStateTable
				{
					.shader = RHI::Shader::All,
					.ranges =
					{
						RHI::PipelineStateTableRange
						{
							.type = RHI::DescripteorRangeType::ConstBuffer,
							.shaderRegister = 0
						},
					}
				},
			},
			.samplers = { RHI::SamplerArgs{}},
		};
		_pipelinestate = _device->CreatePipelineState(psoArgs_hair);

		
		std::u8string path_hair_simulate = u8"../data/shaders/hair_simulate.hlsl";
		RHI::PipelineStateArgs psoArgs_hair_simulate =
		{
			.topology = RHI::TopologyType::Point,
			.CS = path_hair_simulate,
			.CSMain = "CSMain",
			
			.tables =
			{
				RHI::PipelineStateTable
				{
					.shader = RHI::Shader::All,
					.ranges =
					{
						RHI::PipelineStateTableRange
						{
							.type = RHI::DescripteorRangeType::ConstBuffer,
							.shaderRegister = 0
						},
						RHI::PipelineStateTableRange
						{
							.type = RHI::DescripteorRangeType::ShaderResource,
							.shaderRegister = 0
						},
						RHI::PipelineStateTableRange
						{
							.type = RHI::DescripteorRangeType::ShaderResource,
							.shaderRegister = 1
						},
						RHI::PipelineStateTableRange
						{
							.type = RHI::DescripteorRangeType::UnorderedAccess,
							.shaderRegister = 0
						},
						RHI::PipelineStateTableRange
						{
							.type = RHI::DescripteorRangeType::UnorderedAccess,
							.shaderRegister = 1
						},
					}
				},
			},
		};
		_pipelinestate_simulate = _device->CreatePipelineState(psoArgs_hair_simulate);

		RHI::ResourcePacketArgs packetArgs_hair = 
		{
			.type = RHI::ResourcePacketType::Resource,
			.capacity = 2,
			.flags = RHI::ResourcePacketFlag::ShaderVisible,
		};
		_resourcepacket = _device->CreateResourcePacket(packetArgs_hair);

		RHI::ResourcePacketArgs packetArgs_hair_simulate =
		{
			.type = RHI::ResourcePacketType::Resource,
			.capacity = SimulateResourceId_Count,
			.flags = RHI::ResourcePacketFlag::ShaderVisible,
		};
		_resourcepacket_simulate = _device->CreateResourcePacket(packetArgs_hair_simulate);

		_nvertices = _vertices.size();
		_nindices = _indices.size();
		_nindices_lines = _indices_lines.size();

		_cmdallocator->Reset();
		_cmdlist->Reset(_cmdallocator.get());

		// vertices
		RHI::ResourceArgs verticesParams_UL = {};
		verticesParams_UL.heap.type = RHI::HeapType::Upload;
		verticesParams_UL.size.cx = sizeof(Vertex) * _vertices.size();
		verticesParams_UL.dimension = RHI::ResourceDimension::Raw;
		verticesParams_UL.states = RHI::ResourceState::GenericRead;
		auto vetexbuffer_UL = _device->CreateResource(verticesParams_UL);
		std::memcpy(vetexbuffer_UL->Data(), _vertices.data(), sizeof(Vertex) * _vertices.size());

		RHI::ResourceArgs verticesParams = {};
		verticesParams.heap.type = RHI::HeapType::Default;
		verticesParams.size.cx = sizeof(Vertex) * _vertices.size();
		verticesParams.dimension = RHI::ResourceDimension::Raw;
		verticesParams.states = RHI::ResourceState::CopyDest;
		verticesParams.flags = RHI::ResourceFlag::AllowUnorderdAccess;
		_vetexbuffer = _device->CreateResource(verticesParams);


		_vetexbuffer_uav = _resourcepacket_simulate->SetShaderResource(SimulateResourceId_Positions, _vetexbuffer.get(),
			RHI::ResourceViewArgs
			{
				.type = RHI::ResourceType::UnorderedAccess,
				.shaderresource =
			{
				.format = core::pixelformat::none,
				.dimension = RHI::ResourceViewDimension::Buffer,
				.buffer =
			{
				.numElements = (uint32_t)_vertices.size(),
				.stride = sizeof(Vertex)
			}
			},
			});
		
		// vetexbuffer prev
		auto vetexbuffer_prev_UL = _device->CreateResource(
			RHI::ResourceArgs
			{
				.heap =
				{
					.type = RHI::HeapType::Upload,
				},
				.dimension = RHI::ResourceDimension::Raw,
				.size = { uint32_t(sizeof(core::float4) * _vertices.size()), 1 },
				.states = RHI::ResourceState::GenericRead,
			}
		);

		_vetexbuffer_prev = _device->CreateResource(
			RHI::ResourceArgs
			{
				.heap =
				{
					.type = RHI::HeapType::Default,
				},
				.dimension = RHI::ResourceDimension::Raw,
				.flags = RHI::ResourceFlag::AllowUnorderdAccess,
				.size = { uint32_t(sizeof(core::float4) * _vertices.size()), 1 },
				.states = RHI::ResourceState::CopyDest,
			}
		);
		core::float4 * vetexbuffer_prev_UL_ptr = static_cast<core::float4 *>(vetexbuffer_prev_UL->Data());
		for (size_t ivertex = 0; ivertex < _vertices.size(); ++ivertex)
			vetexbuffer_prev_UL_ptr[ivertex] = _vertices[ivertex].position;

		_vetexbuffer_prev_uav = _resourcepacket_simulate->SetShaderResource(SimulateResourceId_PositionsPrev, _vetexbuffer_prev.get(),
			RHI::ResourceViewArgs
			{
				.type = RHI::ResourceType::UnorderedAccess,
				.shaderresource =
				{
					.format = core::pixelformat::none,
					.dimension = RHI::ResourceViewDimension::Buffer,
					.buffer =
					{
						.numElements = (uint32_t)_vertices.size(),
						.stride = sizeof(core::float4)
					}
				},
			});

		
		
		_vetexbuffer_srv = _resourcepacket->SetShaderResource(1, _vetexbuffer.get(),
			RHI::ResourceViewArgs
			{
				.type = RHI::ResourceType::ShaderResource,
				.shaderresource =
				{
					.format = core::pixelformat::none,
					.dimension = RHI::ResourceViewDimension::Buffer,
					.buffer =
					{
						.numElements = (uint32_t)_vertices.size(),
						.stride = sizeof(Vertex)
					}
				},
			});

		// indices
		RHI::ResourceArgs indicesParams_UL = {};
		indicesParams_UL.heap.type = RHI::HeapType::Upload;
		indicesParams_UL.size.cx = sizeof(uint16_t) * _indices.size();
		indicesParams_UL.dimension = RHI::ResourceDimension::Raw;
		indicesParams_UL.states = RHI::ResourceState::GenericRead;
		auto indexbuffer_UL = _device->CreateResource(indicesParams_UL);
		std::memcpy(indexbuffer_UL->Data(), _indices.data(), sizeof(uint16_t) * _indices.size());

		RHI::ResourceArgs indicesParams = {};
		indicesParams.heap.type = RHI::HeapType::Default;
		indicesParams.size.cx = sizeof(uint16_t) * _indices.size();
		indicesParams.dimension = RHI::ResourceDimension::Raw;
		indicesParams.states = RHI::ResourceState::CopyDest;
		_indexbuffer = _device->CreateResource(indicesParams);

		// indices line
		RHI::ResourceArgs indicesLinesParams_UL = {};
		indicesLinesParams_UL.heap.type = RHI::HeapType::Upload;
		indicesLinesParams_UL.size.cx = sizeof(uint16_t) * _indices_lines.size();
		indicesLinesParams_UL.dimension = RHI::ResourceDimension::Raw;
		indicesLinesParams_UL.states = RHI::ResourceState::GenericRead;
		auto indexLinesbuffer_UL = _device->CreateResource(indicesLinesParams_UL);
		std::memcpy(indexLinesbuffer_UL->Data(), _indices_lines.data(), sizeof(uint16_t) * _indices_lines.size());

		RHI::ResourceArgs indicesLinesParams = {};
		indicesLinesParams.heap.type = RHI::HeapType::Default;
		indicesLinesParams.size.cx = sizeof(uint16_t) * _indices_lines.size();
		indicesLinesParams.dimension = RHI::ResourceDimension::Raw;
		indicesLinesParams.states = RHI::ResourceState::CopyDest;
		_indexbuffer_lines = _device->CreateResource(indicesLinesParams);
		
		RHI::ResourceArgs constParams = {};
		constParams.heap.type = RHI::HeapType::Upload;
		constParams.size.cx = (sizeof(SceneConstantBuffer) + 0xff) & ~0xff;
		constParams.dimension = RHI::ResourceDimension::Raw;
		constParams.states = RHI::ResourceState::GenericRead;
		_constbuffer = _device->CreateResource(constParams);

		_constbuffer_simulate = _device->CreateResource(
			RHI::ResourceArgs
			{
				.heap =
				{
					.type = RHI::HeapType::Upload
				},
			.dimension = RHI::ResourceDimension::Raw,
			.size = { uint32_t((sizeof(SimulateConstantBuffer) + 0xff) & ~0xff), 1 },
			.states = RHI::ResourceState::GenericRead,
				
			}
		);
		_constbuffer_simulate_view = _resourcepacket_simulate->SetShaderResource(SimulateResourceId_ConstBuffer, _constbuffer_simulate.get(),
			RHI::ResourceViewArgs
			{
				.type = RHI::ResourceType::ConstBuffer,
			}
			);

		RHI::ResourceViewArgs viewArgs_constbuffer = {};
		viewArgs_constbuffer.type = RHI::ResourceType::ConstBuffer;
		_constbuffer_view = _resourcepacket->SetShaderResource(0, _constbuffer.get(), viewArgs_constbuffer);

		// strandoffsets
		auto strandoffsets_UL = _device->CreateResource(
			RHI::ResourceArgs
			{
				.heap =
				{
					.type = RHI::HeapType::Upload,
				},
				.dimension = RHI::ResourceDimension::Raw,
				.size = { uint32_t(sizeof(uint32_t) * _strandOffsets.size()), 1},
				.states = RHI::ResourceState::GenericRead
			}
		);
		_res_strandoffsets = _device->CreateResource(
			RHI::ResourceArgs
			{
				.heap =
				{
					.type = RHI::HeapType::Default,
				},
				.dimension = RHI::ResourceDimension::Raw,
				.size = { uint32_t(sizeof(uint32_t) * _strandOffsets.size()), 1},
				.states = RHI::ResourceState::CopyDest
			}
		);
		std::memcpy(strandoffsets_UL->Data(), _strandOffsets.data(), sizeof(uint32_t) * _strandOffsets.size());

		_strandOffsets_srv = _resourcepacket_simulate->SetShaderResource(SimulateResourceId_StrandOffsets, _res_strandoffsets.get(),
			RHI::ResourceViewArgs
			{
				.type = RHI::ResourceType::ShaderResource,
				.shaderresource =
				{
					.format = core::pixelformat::none,
					.dimension = RHI::ResourceViewDimension::Buffer,
					.buffer =
					{
						.numElements = (uint32_t)_strandOffsets.size(),
						.stride = sizeof(uint32_t)
					}
				},
			});

		// constraints
		auto constraints_UL = _device->CreateResource(
			RHI::ResourceArgs
			{
				.heap =
			{
				.type = RHI::HeapType::Upload,
			},
			.dimension = RHI::ResourceDimension::Raw,
			.size = { uint32_t(sizeof(core::float4) * _constraints.size()), 1 },
			.states = RHI::ResourceState::GenericRead
			}
		);
		_res_constraints = _device->CreateResource(
			RHI::ResourceArgs
			{
				.heap =
			{
				.type = RHI::HeapType::Default,
			},
			.dimension = RHI::ResourceDimension::Raw,
			.size = { uint32_t(sizeof(core::float4) * _constraints.size()), 1 },
			.states = RHI::ResourceState::CopyDest
			}
		);
		std::memcpy(constraints_UL->Data(), _constraints.data(), sizeof(core::float4) * _constraints.size());
		_constraints_srv = _resourcepacket_simulate->SetShaderResource(SimulateResourceId_Constrains, _res_constraints.get(),
			RHI::ResourceViewArgs
			{
				.type = RHI::ResourceType::ShaderResource,
				.shaderresource =
				{
					.format = core::pixelformat::none,
					.dimension = RHI::ResourceViewDimension::Buffer,
					.buffer =
					{
						.numElements = (uint32_t)_constraints.size(),
						.stride = sizeof(core::float4)
					}
				},
			});

		_cmdlist->CopyResource(_vetexbuffer.get(), vetexbuffer_UL.get());
		_cmdlist->CopyResource(_vetexbuffer_prev.get(), vetexbuffer_prev_UL.get());
		_cmdlist->CopyResource(_indexbuffer.get(), indexbuffer_UL.get());
		_cmdlist->CopyResource(_indexbuffer_lines.get(), indexLinesbuffer_UL.get());
		_cmdlist->CopyResource(_res_strandoffsets.get(), strandoffsets_UL.get());
		_cmdlist->CopyResource(_res_constraints.get(), constraints_UL.get());
		_cmdlist->TransitionBarrier(_vetexbuffer.get(), RHI::ResourceState::VertexBuffer);
		_cmdlist->TransitionBarrier(_indexbuffer.get(), RHI::ResourceState::IndexBuffer);
		_cmdlist->TransitionBarrier(_indexbuffer_lines.get(), RHI::ResourceState::IndexBuffer);
		_cmdlist->TransitionBarrier(_indexbuffer_lines.get(), RHI::ResourceState::IndexBuffer);
		_cmdlist->TransitionBarrier(_res_strandoffsets.get(), RHI::ResourceState::ComputerShaderRerources);
		_cmdlist->TransitionBarrier(_res_constraints.get(), RHI::ResourceState::ComputerShaderRerources);
		
		_cmdlist->Close();
		_cmdqueue->Excute(_cmdlist.get());

		_cmdqueue->Wait();
	}

	void LoadRawResources()
	{
		std::ifstream file;
		file.open(L"../data/LongHairResources/hair_vertices.txt", std::ios::in | std::ios::binary);
		while (!file.eof())
		{
			std::vector<Vertex> vertices;
			std::string line;
			std::getline(file, line);
			size_t loc = line.find("//", 0);
			if (loc != std::string::npos)
			{
				std::istringstream ss_points(line);
				int npoints = 0;
				ss_points >> npoints;

				//texcoords
				std::string line2;
				std::getline(file, line2);
				std::istringstream ss_texcoord(line2);

				Vertex vertex;
				char tempChar = 0;
				ss_texcoord >> tempChar >> vertex.uv.x >> vertex.uv.y;
				assert(tempChar == 't');

				int indexbase = _vertices.size();
				for (int ipoint = 0; ipoint < npoints; ipoint++)
				{
					std::string line3;
					std::getline(file, line3);
					std::istringstream ss_vertex(line3);
					ss_vertex >> vertex.position.x >> vertex.position.y >> vertex.position.z;
					vertex.position.z = -vertex.position.z;
					vertex.position.w = static_cast<float>(ipoint) / npoints;
					vertices.push_back(vertex);
					_vertices.push_back(vertex);

					if (ipoint > 0)
					{
						_indices_lines.push_back(_vertices.size() - 2);
						_indices_lines.push_back(_vertices.size() - 1);
					}
				}
				_strandOffsets.push_back(_vertices.size());

				_indices.push_back(indexbase + 0);
				_indices.push_back(indexbase + 0);
				_indices.push_back(indexbase + 1);
				_indices.push_back(indexbase + 2);
				
				for (int ipoint = 0; ipoint < npoints - 3; ipoint++)
				{
					_indices.push_back(indexbase + ipoint + 0);
					_indices.push_back(indexbase + ipoint + 1);
					_indices.push_back(indexbase + ipoint + 2);
					_indices.push_back(indexbase + ipoint + 3);
				}

				_indices.push_back(indexbase + npoints - 3);
				_indices.push_back(indexbase + npoints - 2);
				_indices.push_back(indexbase + npoints - 1);
				_indices.push_back(indexbase + npoints - 1);
			}
			_controlPoints.push_back(vertices);
		}
		file.close();


		drawing::image::image_codec_context  icc;
		icc.get_format = [](drawing::image::image_type type, drawing::image::image_format format)
		{
			drawing::image::image_format result = format;
			switch (format.format)
			{
			case drawing::image::format_b8g8r8: result.format = drawing::image::format_b8g8r8a8; break;
			default:
				return drawing::image::image_get_format(type, format);
			}
			return result;
		};

		drawing::image::image_data_t linear_stiffness_map;
		auto err = drawing::image::image_load(icc, u8"../data/LongHairResources/stiffnessMap.dds", linear_stiffness_map);
		if (err)
		{
			core::war() << __FUNCTION__ << " image_load(stiffnessMap.dds) failed!";
		}

		float hairStiffnessMultiplier = 1.0f;
		int shortStiffHairLength = 15;
		float flex = 0.0;
		float stiff = 1.0;
		int blendRegion = 8;
		int stiffRegion = 9;
		if (_shortHair)
		{
			shortStiffHairLength = 10;
			stiffRegion = 5;
			hairStiffnessMultiplier = 0.15f;
		}

		int blendBegin = stiffRegion - blendRegion / 2.0;
		int blendEnd = stiffRegion + blendRegion / 2.0;
		
		_constraints.resize(_vertices.size());
		for(size_t istrand = 0; istrand < _strandOffsets.size(); ++istrand)
		{
			uint32_t ipointfirst = istrand > 0 ? _strandOffsets[istrand - 1] : 0;
			uint32_t npoints = _strandOffsets[istrand] - ipointfirst;
			uint32_t ipointlast = ipointfirst + npoints;

			// linear length
			for (size_t ipoint = 0; ipoint < npoints - 1; ++ipoint)
				_constraints[ipointfirst + ipoint].x = (_vertices[ipointfirst + ipoint + 1].position - _vertices[ipointfirst + ipoint].position).xyz.length();

			// linear stiffness
			for (size_t ipoint = 0; ipoint < npoints; ++ipoint)
			{
				uint32_t row = std::clamp<uint32_t>(uint32_t(_vertices[ipointfirst + ipoint].uv.y * linear_stiffness_map.format.height), 0, linear_stiffness_map.format.height - 1);
				uint32_t col = std::clamp<uint32_t>(uint32_t(_vertices[ipointfirst + ipoint].uv.x * linear_stiffness_map.format.width), 0, linear_stiffness_map.format.width - 1);

				float hairTexStiffness = uint8_t(linear_stiffness_map.data[row * linear_stiffness_map.pitch + col * 4]) / 255.0f * hairStiffnessMultiplier;
				if (npoints < shortStiffHairLength && hairTexStiffness > 0.1)
				{
					hairTexStiffness = 1.0f;
				}
				else
				{
					float localStiff = hairTexStiffness * stiff + (1 - hairTexStiffness) * 0.1;

					if (ipoint < blendBegin)
					{
						hairTexStiffness = localStiff;
					}
					else if (ipoint >= blendEnd)
					{
						hairTexStiffness = flex;
					}
					else
					{
						float blend = float(ipoint - blendBegin) / blendRegion;
						float stif = blend * flex + (1 - blend) * localStiff;
						hairTexStiffness = stif;
					}
				}
				_constraints[ipointfirst + ipoint].y = hairTexStiffness;
			}

			// angular length
			for (size_t ipoint = 0; ipoint < npoints - 2; ++ipoint)
				_constraints[ipointfirst + ipoint].z = (_vertices[ipointfirst + ipoint + 2].position - _vertices[ipointfirst + ipoint].position).xyz.length();

			// angular stiffness
			float rootPos = 0.25f;
			float hairPos = 0.4f;
			for (size_t ipoint = 0; ipoint < npoints; ++ipoint)
			{
				float ratio = float(ipoint) / (npoints - 1);

				if (npoints - 1 < 5)
					_constraints[ipointfirst + ipoint].w = 0.0;
				else if (ratio < rootPos)
					_constraints[ipointfirst + ipoint].w = angularStiffness;
				else if (ratio > hairPos)
					_constraints[ipointfirst + ipoint].w = 0.0;
				else
					_constraints[ipointfirst + ipoint].w = angularStiffness * (1.0 - ((ratio - rootPos) / (hairPos - rootPos)));

				//maximum stiffness for long demo hair
				if (!_shortHair)
					_constraints[ipointfirst + ipoint].w = 1.0;
			}
		}
		angularStiffness = 0;
		

		//-- for test
		//std::vector<Vertex> vertices =
		//{
		//	//{ { -1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
		//	//{ { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
		//	//{ { 1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
		//	//{ { -1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
		//	//
		//	{ { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
		//	{ { 1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
		//	{ { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f } },
		//	//
		//	//{ { -25.f, +25.f, 0.0f }, { 0.0f, 0.0f } },
		//	//{ { +25.f, +25.f, 0.0f }, { 1.0f, 0.0f } },
		//	//{ { +25.f, -25.f, 0.0f }, { 1.0f, 1.0f } },
		//	//{ { -25.f, -25.f, 0.0f }, { 1.0f, 1.0f } },
		//};
		//std::vector<uint16_t> indices = { 0, 1, 2};
		//std::vector<uint16_t> indices_lines = {0, 1, 2};
		//_vertices = vertices;
		//_indices = indices;
		//_indices_lines = indices_lines;

	}

	void RenderThread()
	{
		LoadRawResources();
		LoadPipeline();
		LoadAssets();
		
		core::counter_fps<float, 3> fps;
		float64_t time_last = core::datetime::system();
		SceneConstantBuffer cbuffer;

		RECT rcClient;
		GetClientRect(_hwnd, &rcClient);

		core::sizei windowSize(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);

		core::recti sccisorrect = { 0, 0, windowSize.cx, windowSize.cy };
		RHI::ViewPort viewport = { 0, 0, windowSize.cx, windowSize.cy, 0.0f, 1.0f };

		core::float4x4 matrProj = core::float4x4_perspective_lh(3.14f / 3.0f, 16.0f / 9.0f, 0.1f, 5000.0f);

		while (rendering)
		{
			auto elapse = core::datetime::system() - time_last;
			time_last += elapse;
			
			GetClientRect(_hwnd, &rcClient);
			core::sizei windowSize2(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
			if (windowSize2 != windowSize)
			{
				windowSize = windowSize2;
				_rendertarget.reset();
				matrProj = core::float4x4_perspective_lh(3.14f / 3.0f, 16.0f / 9.0f, 0.1f, 5000.0f);
				sccisorrect = { 0, 0, windowSize.cx, windowSize.cy };
				viewport = { 0, 0, (float)windowSize.cx, (float)windowSize.cy, 0.0f, 1.0f };
				RHI::RenderTargetArgs rtparams = {};
				rtparams.hwnd = _hwnd;
				_rendertarget = _device->CreateRenderTargetForHWND(_cmdqueue.get(), rtparams);
			}

			core::float4x4 matrView = core::float4x4_lookat_lh({ 0.0f, 0.0f, _view_z }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
			if (_rotating)
				_rotate += elapse;

			{
				core::float4x4 matrWorld = core::float4x4_rotate({ 0.0f, _rotate * 3.14f * 0.5f, 0.0f });
				cbuffer.transform = matrWorld * matrView * matrProj;
				cbuffer.tessFactor = _tessFactor;
				std::memcpy(_constbuffer->Data(), &cbuffer, sizeof(cbuffer));
			}

			// SimulateConstantBuffer
			{
				SimulateConstantBuffer scb = {};
				scb.transform.identity();
				scb.gravityStrength = 1;
				scb.timeElapse = elapse;
				scb.angularStiffness = 1;
				scb.numConstraintIterations = 20;
				std::memcpy(_constbuffer_simulate->Data(), &scb, sizeof(scb));
			}

			// Simulate
			{
				_cmdallocator->Reset();
				_cmdlist->Reset(_cmdallocator.get());
				_cmdlist->TransitionBarrier(_vetexbuffer.get(), RHI::ResourceState::UnorderedAccess);
				_cmdlist->SetPipelineState(_pipelinestate_simulate.get());
				_cmdlist->SetResourcePacket(_resourcepacket_simulate.get());
				_cmdlist->SetGraphicsResourceView(0, _constbuffer_simulate_view.get());

				_cmdlist->Dispatch({ uint32_t(_strandOffsets.size()), 1, 1 });

				_cmdlist->TransitionBarrier(_vetexbuffer.get(), RHI::ResourceState::VertexShaderRerources | RHI::ResourceState::VertexBuffer);
				_cmdqueue->Excute(_cmdlist.get());
				_cmdqueue->Wait();
			}

			_rendertarget->Begin();
			_cmdallocator->Reset();
			_cmdlist->Reset(_cmdallocator.get());

			_cmdlist->SetViewPort(viewport);
			_cmdlist->SetScissorRect(sccisorrect);
			_cmdlist->TransitionBarrier(_rendertarget.get(), RHI::ResourceState::RenderTarget);
			_cmdlist->SetRenderTarget(_rendertarget->CurrentRTV());
			_cmdlist->ClearRenderTarget(0xffcccccc);

			_cmdlist->SetResourcePacket(_resourcepacket.get());

			if (_mode.any(content_mode::bsline))
			{
				_cmdlist->SetPipelineState(_pipelinestate.get());
				_cmdlist->SetGraphicsResourceView(0, _constbuffer_view.get());
				_cmdlist->IASetVertexBuffer(_vetexbuffer.get(), sizeof(Vertex), sizeof(Vertex) * _nvertices);
				_cmdlist->IASetIndexBuffer(_indexbuffer.get(), sizeof(uint16_t), sizeof(uint16_t) * _nindices);
				_cmdlist->SetGraphicsResourceView(0, _constbuffer_view.get());
				_cmdlist->IASetTopologyType(RHI::Topology::Point4PatchList);
				_cmdlist->DrawIndexedInstanced(_nindices, 1, 0, 0, 0);
			}

			if (_mode.any(content_mode::line))
			{
				_cmdlist->SetPipelineState(_pipelinestate_basic.get());
				_cmdlist->SetGraphicsResourceView(0, _constbuffer_view.get());
				_cmdlist->IASetVertexBuffer(_vetexbuffer.get(), sizeof(Vertex), sizeof(Vertex) * _nvertices);
				_cmdlist->IASetIndexBuffer(_indexbuffer_lines.get(), sizeof(uint16_t), sizeof(uint16_t) * _nindices_lines);
				_cmdlist->IASetTopologyType(RHI::Topology::LineList);
				_cmdlist->DrawIndexedInstanced(_nindices_lines, 1, 0, 0, 0);
			}

			_cmdlist->TransitionBarrier(_rendertarget.get(), RHI::ResourceState::Present);
			_cmdlist->SetRenderTarget(nullptr);
			_cmdlist->Close();
			_cmdqueue->Excute(_cmdlist.get());
			_rendertarget->Present(0);
			_cmdqueue->Wait();
			fps.acc(1);

			core::logger_fps tt(__FILE__, __LINE__, 10);
			if (tt.ok())
				printf("\rfps = %.6f", fps.fps());
		}
	}
	
private:
	HWND _hwnd = NULL;

	std::atomic<bool> _rendering = true;
	RHI::RHID3D12::RHID3D12Factory RHI;

	std::shared_ptr<RHI::RHIDevice> _device;
	std::shared_ptr<RHI::RHICommandQueue> _cmdqueue;
	
	std::shared_ptr<RHI::RHIRenderTarget> _rendertarget;
	std::shared_ptr<RHI::RHICommandAllocator> _cmdallocator;
	std::shared_ptr<RHI::RHICommandList> _cmdlist;

	std::shared_ptr<RHI::RHIPipelineState> _pipelinestate;
	std::shared_ptr<RHI::RHIPipelineState> _pipelinestate_basic;
	std::shared_ptr<RHI::RHIPipelineState> _pipelinestate_simulate;
	std::shared_ptr<RHI::RHIResourcePacket> _resourcepacket;

	enum SimulateResourceId
	{
		SimulateResourceId_ConstBuffer = 0,
		SimulateResourceId_StrandOffsets,
		SimulateResourceId_Constrains,
		SimulateResourceId_Positions,
		SimulateResourceId_PositionsPrev,
		SimulateResourceId_Count,
	};
	
	std::shared_ptr<RHI::RHIResourcePacket> _resourcepacket_simulate;
	
	std::shared_ptr<RHI::RHIResource> _vetexbuffer;
	std::shared_ptr<RHI::RHIResource> _vetexbuffer_prev;
	std::shared_ptr<RHI::RHIResource> _indexbuffer;
	std::shared_ptr<RHI::RHIResource> _indexbuffer_lines;
	
	std::shared_ptr<RHI::RHIResourceView> _vetexbuffer_uav;
	std::shared_ptr<RHI::RHIResourceView> _vetexbuffer_prev_uav;
	std::shared_ptr<RHI::RHIResourceView> _vetexbuffer_srv;

	
	size_t _nvertices = 0;
	size_t _nindices = 0;
	size_t _nindices_lines = 0;
	std::shared_ptr<RHI::RHIResource> _constbuffer;
	std::shared_ptr<RHI::RHIResourceView> _constbuffer_view;
	std::shared_ptr<RHI::RHIResource> _constbuffer_simulate;
	std::shared_ptr<RHI::RHIResourceView> _constbuffer_simulate_view;

	std::future<void> _future;
	core::float2 _tessFactor = { 1.0f, 10.0f };


	bool _shortHair = false;
	std::vector<std::vector<Vertex>> _controlPoints;
	std::vector<Vertex> _vertices;
	std::vector<uint16_t> _indices;
	std::vector<uint16_t> _indices_lines;
	std::vector<uint32_t> _strandOffsets;
	std::vector<core::float4> _constraints;
	float angularStiffness = 0.025f;

	std::shared_ptr<RHI::RHIResource> _res_strandoffsets;
	std::shared_ptr<RHI::RHIResourceView> _strandOffsets_srv;
	std::shared_ptr<RHI::RHIResource> _res_constraints;
	std::shared_ptr<RHI::RHIResourceView> _constraints_srv;

	// transform
	bool _rotating = true;
	float _rotate = 0.0f;

	float _view_z = -14.0f;

	enum class content_mode
	{
		bsline = 0x1,
		line = 0x2,
	};
	using content_modes = core::bitflag<content_mode>;
	content_modes _mode = content_mode::bsline;
};


HairRender * Render = nullptr;
static LRESULT CALLBACK DefaultWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//if(message == WM_CREATE)
	//{
	//	CREATESTRUCT * cs = reinterpret_cast<CREATESTRUCT *>(lParam);
	//	Window * window = static_cast<Window *>(cs->lpCreateParams);
	//	if(window)
	//		window->attatch(hWnd);
	//}

	if (message == WM_PAINT)
	{
		PAINTSTRUCT pt;
		HDC hdc;
		hdc = BeginPaint(hWnd, &pt);
		EndPaint(hWnd, &pt);
	}
	else if (message == WM_KEYDOWN)
	{
		if (Render)
			Render->OnKeyDown(wParam, lParam);
	}
	else if (message == WM_MOUSEWHEEL)
	{
		int scroll = core::u32hi16(wParam) / WHEEL_DELTA;
		if (Render)
			Render->OnScrollV(scroll);
	}
	else if (message == WM_CLOSE)
	{
		win32::endLoop(0);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

int main()
{
#if defined _DEBUG
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    core::App app;

	core::global_logger::start(u8"log.log", core::log_dbg);
	
	HINSTANCE hInstance = (HINSTANCE)win32::instance();

	WNDCLASSEXW wcex = { sizeof(WNDCLASSEXW) };

	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.style = 0;
	wcex.lpfnWndProc = DefaultWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = /*::LoadCursor(NULL, IDC_ARROW)*/NULL;
	//wcex.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = win32::WINDOW_CLASS_NAME;
	wcex.hIconSm = NULL;

	RegisterClassExW(&wcex);

	HWND hwnd = CreateWindowExW(
		0, win32::WINDOW_CLASS_NAME, L"Title", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080,
		NULL, NULL, hInstance, NULL);

	::ImmAssociateContext(hwnd, NULL);
	::ShowWindow(hwnd, SW_NORMAL);
	::UpdateWindow(hwnd);

	HairRender hr(hwnd);
	hr.Start();
	
	Render = &hr;
    win32::runLoop();
	rendering = false;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    //https://docs.microsoft.com/en-us/windows/desktop/api/shellscalingapi/nf-shellscalingapi-setprocessdpiawareness
    // 使用 manifest 而不是直接设置
    //HMODULE Shcore = GetModuleHandleW(L"Shcore.dll");
    //auto pfn_SetProcessDpiAwareness = reinterpret_cast<decltype(SetProcessDpiAwareness) *>(GetProcAddress(Shcore, "SetProcessDpiAwareness"));
    //if(pfn_SetProcessDpiAwareness)
    //	pfn_SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);

    main();
    return 0;
}
