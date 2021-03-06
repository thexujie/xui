﻿#include "stdafx.h"
#include "x/platform/win32/win32.h"
//#include <ShellScalingAPI.h>
//#pragma comment(lib, "Shcore.lib")

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "main.h"
#include "RHID3D12/RHID3D12Factory.h"
#include "RHID3D11/RHID3D11Factory.h"
//#include "RHIVulkan/RHIVulkanFactory.h"

#pragma comment(lib, "imm32.lib")

const int COORD_JITTER_COUNT_U = 64;
const int COORD_JITTER_COUNT_V = 16;

struct Vertex
{
	core::float3 position;
	core::float1 locate;
	core::float2 uv;
	core::float3 tangentX;
	core::float3 tangentY;
};

struct HairVertex
{
	core::uint4 pointIndex;
	uint32_t strandIndex;
};

const int MAX_COLLISION_SPHERES = 10;
struct SimulateConstantBuffer
{
	core::float4x4 matrWorld;
	float gravityAcceleration;
	float gravityStrength;
	float timeElapse;
	float angularStiffness;
	// 约束计算的积分次数，越多越精确
	int numConstraintIterations;
	core::float3 eyePosition;
	
	uint32_t numSphereImplicits;
	core::uint3 _unused;

	core::float4x4 CollisionSphereTransformations[MAX_COLLISION_SPHERES];
	core::float4x4 CollisionSphereInverseTransformations[MAX_COLLISION_SPHERES];
};

struct SceneConstantBuffer
{
	core::float4x4 worldTransform;
	core::float4x4 viewprojTransform;
	core::float2 tessFactor;
	core::float2 __unused;
	core::float3 eyePosition;
	core::float1 _unused1;
	core::float3 lightVector;
	core::float1 _unused2;
};

class HairRender
{
public:
	HairRender(HWND hwnd) : _hwnd(hwnd)
	{
		
	}

	~HairRender() = default;

	core::float3 _hair_translate_last;
	core::int2 _hair_translate_mousepos;
	bool _hair_trandlate_draging = false;
	void OnMButtonDown(uint32_t wParam, int32_t lParam)
	{
		_hair_translate_last = _hair_translate;
		_hair_translate_mousepos = core::int2(core::i32li16(lParam), core::i32hi16(lParam));
		
	}
	void OnMouseUp(uint32_t wParam, int32_t lParam)
	{
	}
	void OnMouseMove(uint32_t wParam, int32_t lParam)
	{
		core::int2 pos(core::i32li16(lParam), core::i32hi16(lParam));
		static core::int2 pos_last = {core::int32_max, core::int32_max};
		if (pos_last == core::int2{ core::int32_max, core::int32_max })
			pos_last = pos;
		if (wParam & MK_LBUTTON)
		{
			core::int2 offset = pos - pos_last;
			_hair_rotate_z -= offset.x / 200.0f * 3.14f;
		}
		else if (wParam & MK_MBUTTON)
		{
			core::int2 offset = pos - _hair_translate_mousepos;
			_hair_translate.xy = _hair_translate_last.xy + core::float2(offset.to<float>() * core::vec2<float>(1, -1) / 100.0f);
			//_hair_translate.y = _hair_translate_last.y + offset.y / 100.0f;
		}
		else {}

		pos_last = pos;
	}
	
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
		case 'L':
			_rhi_next = _rhi == RHIType::D3D11 ? RHIType::D3D12 : RHIType::D3D11;
			break;
		case 'R':
			_rotating = !_rotating;
			break;
		case 'S':
			_simulate = !_simulate;
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

		UpdateTitle();
	}

	void UpdateTitle()
	{
		std::wstring title = core::wfmt("TessFactor=[", std::setprecision(4), _tessFactor.x, L",", _tessFactor.y, L"] - ", _rhi_next == RHIType::D3D11 ? "D3D11" : "D3D12");
		::SetWindowTextW(_hwnd, title.c_str());
	}
	
	void Start()
	{
		_future = std::async(&HairRender::RenderThreadBase, this);
	}

	void Stop()
	{
		_rendering = false;
		_future.wait();
	}
	void LoadAssets()
	{
		std::u8string path_basic = u8"../data/shaders/basic.hlsl";
		RHI::PipelineStateArgs psoArgs_basic = {};
		{
			psoArgs_basic.topology = RHI::TopologyType::Line;
			psoArgs_basic.VS = path_basic;
			psoArgs_basic.VSMain = "VSMain";
			psoArgs_basic.PS = path_basic;
			psoArgs_basic.PSMain = "PSMain";
			
			psoArgs_basic.elements =
			{
				RHI::InputElement{ "POINT_INDEX", core::format::uint1, 0 },
			};
			psoArgs_basic.tables =
			{
				RHI::PipelineStateTable
				{
					RHI::Shader::All,
					{
						RHI::PipelineStateTableRange { RHI::DescriptorRangeType::ConstBuffer, 0 },
						RHI::PipelineStateTableRange { RHI::DescriptorRangeType::ShaderResource, 0 },
						RHI::PipelineStateTableRange { RHI::DescriptorRangeType::ShaderResource, 1 },
					}
				},
			};
			psoArgs_basic.samplers = { RHI::SamplerArgs{} };
		};
		_pipelinestate_basic = _device->CreatePipelineState(psoArgs_basic);

		std::u8string path_hair = u8"../data/shaders/hair.hlsl";
		RHI::PipelineStateArgs psoArgs_hair = {};
		{
			psoArgs_hair.topology = RHI::TopologyType::Patch;
			psoArgs_hair.VS = path_hair;
			psoArgs_hair.VSMain = "VSMain";
			psoArgs_hair.HS = path_hair;
			psoArgs_hair.HSMain = "HSMain";
			psoArgs_hair.DS = path_hair;
			psoArgs_hair.DSMain = "DSMain";
			psoArgs_hair.GS = path_hair;
			psoArgs_hair.GSMain = "GSMain";
			psoArgs_hair.PS = path_hair;
			psoArgs_hair.PSMain = "PSMain";

			psoArgs_hair.elements =
			{
				RHI::InputElement{ "PATCH_INDEX", core::format::uint4, 0 },
				RHI::InputElement{ "STRAND_INDEX", core::format::uint1, 16 },
			};
			psoArgs_hair.tables =
			{
				RHI::PipelineStateTable
				{
					RHI::Shader::All,
					{
						RHI::PipelineStateTableRange { RHI::DescriptorRangeType::ConstBuffer, 0 },
						RHI::PipelineStateTableRange { RHI::DescriptorRangeType::ShaderResource, 0 },
						RHI::PipelineStateTableRange { RHI::DescriptorRangeType::ShaderResource, 1 },
						RHI::PipelineStateTableRange { RHI::DescriptorRangeType::ShaderResource, 2 },
					}
				},
			};
			psoArgs_hair.samplers = { RHI::SamplerArgs{} };
		}
		_pipelinestate = _device->CreatePipelineState(psoArgs_hair);

		
		std::u8string path_hair_simulate = u8"../data/shaders/hair_simulate.hlsl";
		RHI::PipelineStateArgs psoArgs_hair_simulate = {};
		{
			psoArgs_hair_simulate.CS = path_hair_simulate;
			psoArgs_hair_simulate.CSMain = "CSMain";
			psoArgs_hair_simulate.tables =
			{
				RHI::PipelineStateTable
				{
					RHI::Shader::All,
					{
						RHI::PipelineStateTableRange { RHI::DescriptorRangeType::ConstBuffer, 0 },
						RHI::PipelineStateTableRange { RHI::DescriptorRangeType::ShaderResource, 0 },
						RHI::PipelineStateTableRange { RHI::DescriptorRangeType::ShaderResource, 1 },
						RHI::PipelineStateTableRange { RHI::DescriptorRangeType::ShaderResource, 2 },
						RHI::PipelineStateTableRange { RHI::DescriptorRangeType::UnorderedAccess, 0 },
						RHI::PipelineStateTableRange { RHI::DescriptorRangeType::UnorderedAccess, 1 },
					}
				},
			};
		};
		_pipelinestate_simulate = _device->CreatePipelineState(psoArgs_hair_simulate);

		RHI::ResourcePacketArgs packetArgs_hair = 
		{
			RHI::ResourcePacketType::Resource,
			RenderResourceId_Count,
			RHI::ResourcePacketFlag::ShaderVisible,
		};
		_resourcepacket = _device->CreateResourcePacket(packetArgs_hair);

		RHI::ResourcePacketArgs packetArgs_hair_simulate =
		{
			RHI::ResourcePacketType::Resource,
			SimulateResourceId_Count,
			RHI::ResourcePacketFlag::ShaderVisible,
		};
		_resourcepacket_simulate = _device->CreateResourcePacket(packetArgs_hair_simulate);

		_nvertices = _positions.size();
		_nlines = _lines.size();

		_cmdallocator->Reset(_frameIndex);
		_cmdlist->Reset(_cmdallocator.get(), _frameIndex);


		// const
		_cbuffer_render = _device->CreateResource(RHI::ResourceArgs::Buffer(RHI::HeapType::Default, uint32_t((sizeof(SceneConstantBuffer) + 0xff) & ~0xff),
			RHI::ResourceState::None, RHI::ResourceUsage::ConstBuffer));
		_cbuffer_simulate = _device->CreateResource(RHI::ResourceArgs::Buffer(RHI::HeapType::Default, uint32_t((sizeof(SimulateConstantBuffer) + 0xff) & ~0xff),
			RHI::ResourceState::None, RHI::ResourceUsage::ConstBuffer));
		_cbuffer_staging = _device->CreateResource(RHI::ResourceArgs::Buffer(RHI::HeapType::Upload, uint32_t((sizeof(SimulateConstantBuffer) + sizeof(SimulateConstantBuffer) + 0xff) & ~0xff),
			RHI::ResourceState::GenericRead, RHI::ResourceUsage::None));
		
		// tangents
		auto resource_tangents_UL = _device->CreateResource(RHI::ResourceArgs::Buffer(RHI::HeapType::Upload, sizeof(core::float4) * _tangentYs.size(),
			RHI::ResourceState::GenericRead, RHI::ResourceUsage::None));
		_resource_tangents = _device->CreateResource(RHI::ResourceArgs::Buffer(RHI::HeapType::Default, sizeof(core::float4) * _tangentYs.size(),
			RHI::ResourceState::CopyDest, RHI::ResourceUsage::ShaderResource));
		std::memcpy(resource_tangents_UL->Data(), _tangentYs.data(), sizeof(core::float4) * _tangentYs.size());
		
		// positions
		auto resource_positions_UL = _device->CreateResource(RHI::ResourceArgs::Buffer(RHI::HeapType::Upload, sizeof(core::float4) * _positions.size(),
			RHI::ResourceState::GenericRead, RHI::ResourceUsage::None));
		_resource_init_positions = _device->CreateResource(RHI::ResourceArgs::Buffer(RHI::HeapType::Default, sizeof(core::float4) * _positions.size(),
			RHI::ResourceState::CopyDest, RHI::ResourceUsage::ShaderResource));
		_resource_prev_positions = _device->CreateResource(RHI::ResourceArgs::Buffer(RHI::HeapType::Default, sizeof(core::float4) * _positions.size(),
			RHI::ResourceState::CopyDest, RHI::ResourceUsage::UnorderdResource | RHI::ResourceUsage::ShaderResource));
		_resource_curr_positions = _device->CreateResource(RHI::ResourceArgs::Buffer(RHI::HeapType::Default, sizeof(core::float4) * _positions.size(),
			RHI::ResourceState::CopyDest, RHI::ResourceUsage::UnorderdResource | RHI::ResourceUsage::ShaderResource));
		std::memcpy(resource_positions_UL->Data(), _positions.data(), sizeof(core::float4) * _positions.size());
		
		// vertices patchs
		auto resource_vertices_UL = _device->CreateResource(RHI::ResourceArgs::Buffer(RHI::HeapType::Upload, sizeof(HairVertex) * _vertices.size(),
			RHI::ResourceState::GenericRead, RHI::ResourceUsage::None));
		_resource_vertexbuffer = _device->CreateResource(RHI::ResourceArgs::Buffer(RHI::HeapType::Default, sizeof(HairVertex) * _vertices.size(),
			RHI::ResourceState::CopyDest, RHI::ResourceUsage::VertexBuffer));
		std::memcpy(resource_vertices_UL->Data(), _vertices.data(), sizeof(HairVertex) * _vertices.size());

		// vertices lines
		auto indexLinesbuffer_UL = _device->CreateResource(RHI::ResourceArgs::Buffer(RHI::HeapType::Upload, sizeof(core::uint2) * _lines.size(),
			RHI::ResourceState::GenericRead, RHI::ResourceUsage::None));
		_resource_vertexbuffer_lines = _device->CreateResource(RHI::ResourceArgs::Buffer(RHI::HeapType::Default, sizeof(core::uint2) * _lines.size(),
			RHI::ResourceState::CopyDest, RHI::ResourceUsage::VertexBuffer));
		std::memcpy(indexLinesbuffer_UL->Data(), _lines.data(), sizeof(core::uint2) * _lines.size());
		
		// strandoffsets
		auto strandoffsets_UL = _device->CreateResource(RHI::ResourceArgs::Buffer(RHI::HeapType::Upload, sizeof(uint32_t) * _strandOffsets.size(),
			RHI::ResourceState::GenericRead, RHI::ResourceUsage::None));
		_resource_strandoffsets = _device->CreateResource(RHI::ResourceArgs::Buffer(RHI::HeapType::Default, sizeof(uint32_t) * _strandOffsets.size(),
			RHI::ResourceState::CopyDest, RHI::ResourceUsage::UnorderdResource | RHI::ResourceUsage::ShaderResource));
		std::memcpy(strandoffsets_UL->Data(), _strandOffsets.data(), sizeof(uint32_t) * _strandOffsets.size());

		// coordJitters
		auto resource_coordJitters_UL = _device->CreateResource(RHI::ResourceArgs::Buffer(RHI::HeapType::Upload, sizeof(core::float2) * _coordJitters.size(),
			RHI::ResourceState::GenericRead, RHI::ResourceUsage::None));
		_resource_coord_jitters = _device->CreateResource(RHI::ResourceArgs::Buffer(RHI::HeapType::Default, sizeof(core::float2) * _coordJitters.size(),
			RHI::ResourceState::CopyDest, RHI::ResourceUsage::UnorderdResource | RHI::ResourceUsage::ShaderResource));
		std::memcpy(resource_coordJitters_UL->Data(), _coordJitters.data(), sizeof(core::float2) * _coordJitters.size());
		
		// constraints
		auto constraints_UL = _device->CreateResource(RHI::ResourceArgs::Buffer(RHI::HeapType::Upload, uint32_t(sizeof(core::float4) * _constraints.size()), 
			RHI::ResourceState::GenericRead, RHI::ResourceUsage::None));
		_resource_constraints = _device->CreateResource(RHI::ResourceArgs::Buffer(RHI::HeapType::Default, uint32_t(sizeof(core::float4) * _constraints.size()), 
			RHI::ResourceState::CopyDest, RHI::ResourceUsage::ShaderResource));
		std::memcpy(constraints_UL->Data(), _constraints.data(), sizeof(core::float4) * _constraints.size());

		//--------------------------------------------------------
		_resourcepacket->SetResource(RenderResourceId_CBV_ConstBuffer, _cbuffer_render.get(), RHI::ResourceViewArgs::CBuffer());
		_resourcepacket->SetResource(RenderResourceId_SRV_Positions, _resource_curr_positions.get(), RHI::ResourceViewArgs::Shader(sizeof(core::float4), (uint32_t)_positions.size(), core::format::float4));
		_resourcepacket->SetResource(RenderResourceId_SRV_Tangents, _resource_tangents.get(), RHI::ResourceViewArgs::Shader(sizeof(core::float4), (uint32_t)_positions.size(), core::format::float4));
		_resourcepacket->SetResource(RenderResourceId_SRV_CoordJitters, _resource_coord_jitters.get(), RHI::ResourceViewArgs::Shader(sizeof(core::float2), (uint32_t)_coordJitters.size(), core::format::float2));
		
		_resourcepacket_simulate->SetResource(SimulateResourceId_CBV_ConstBuffer, _cbuffer_simulate.get(), RHI::ResourceViewArgs::CBuffer());

		_resourcepacket_simulate->SetResource(SimulateResourceId_SRV_StrandOffsets, _resource_strandoffsets.get(), RHI::ResourceViewArgs::Shader(sizeof(uint32_t), (uint32_t)_strandOffsets.size(), core::format::uint1));
		_resourcepacket_simulate->SetResource(SimulateResourceId_SRV_Constrains, _resource_constraints.get(), RHI::ResourceViewArgs::Shader(sizeof(core::float4), (uint32_t)_constraints.size(), core::format::float4));
		_resourcepacket_simulate->SetResource(SimulateResourceId_SRV_PositionsInit, _resource_init_positions.get(), RHI::ResourceViewArgs::Shader(sizeof(core::float4), (uint32_t)_positions.size(), core::format::float4));
		
		_resourcepacket_simulate->SetResource(SimulateResourceId_UAV_Positions, _resource_curr_positions.get(), RHI::ResourceViewArgs::Unordered(sizeof(core::float4), (uint32_t)_positions.size(), core::format::float4));
		_resourcepacket_simulate->SetResource(SimulateResourceId_UAV_PositionsPrev, _resource_prev_positions.get(), RHI::ResourceViewArgs::Unordered(sizeof(core::float4), (uint32_t)_positions.size(), core::format::float4));


		_pipelinestate->SetName(u8"_pipelinestate");
		_pipelinestate_basic->SetName(u8"_pipelinestate_basic");
		_pipelinestate_simulate->SetName(u8"_pipelinestate_simulate");

		_cbuffer_render->SetName(u8"_cbuffer_render");
		_resource_curr_positions->SetName(u8"_resource_curr_positions");
		_resource_tangents->SetName(u8"_resource_tangents");
		_resource_coord_jitters->SetName(u8"_resource_coord_jitters");
		_resource_prev_positions->SetName(u8"_resource_prev_positions");
		_resource_vertexbuffer->SetName(u8"_resource_vertexbuffer");
		_resource_vertexbuffer_lines->SetName(u8"_resource_indexbuffer_lines");
		_resource_strandoffsets->SetName(u8"_resource_strandoffsets");
		_cbuffer_simulate->SetName(u8"_cbuffer_simulate");
		
		_cmdlist->CopyResource(_resource_tangents.get(), resource_tangents_UL.get());
		_cmdlist->CopyResource(_resource_init_positions.get(), resource_positions_UL.get());
		_cmdlist->CopyResource(_resource_prev_positions.get(), resource_positions_UL.get());
		_cmdlist->CopyResource(_resource_curr_positions.get(), resource_positions_UL.get());
		_cmdlist->CopyResource(_resource_coord_jitters.get(), resource_coordJitters_UL.get());
		_cmdlist->CopyResource(_resource_vertexbuffer.get(), resource_vertices_UL.get());
		_cmdlist->CopyResource(_resource_vertexbuffer_lines.get(), indexLinesbuffer_UL.get());
		_cmdlist->CopyResource(_resource_strandoffsets.get(), strandoffsets_UL.get());
		_cmdlist->CopyResource(_resource_constraints.get(), constraints_UL.get());
		_cmdlist->TransitionBarrier(_resource_curr_positions.get(), RHI::ResourceState::VertexShaderResource);
		_cmdlist->TransitionBarrier(_resource_prev_positions.get(), RHI::ResourceState::VertexShaderResource);
		_cmdlist->TransitionBarrier(_resource_init_positions.get(), RHI::ResourceState::VertexShaderResource);
		_cmdlist->TransitionBarrier(_resource_tangents.get(), RHI::ResourceState::VertexShaderResource);
		_cmdlist->TransitionBarrier(_resource_coord_jitters.get(), RHI::ResourceState::VertexShaderResource);
		_cmdlist->TransitionBarrier(_resource_vertexbuffer.get(), RHI::ResourceState::VertexBuffer);
		_cmdlist->TransitionBarrier(_resource_vertexbuffer_lines.get(), RHI::ResourceState::VertexBuffer);
		_cmdlist->TransitionBarrier(_resource_strandoffsets.get(), RHI::ResourceState::VertexShaderResource);
		_cmdlist->TransitionBarrier(_resource_constraints.get(), RHI::ResourceState::VertexShaderResource);
		
		_cmdlist->Close();
		_cmdqueue->Excute(_cmdlist.get());

		_fenceValuesRender[_frameIndex] = ++_fenceValueRender;
		_cmdqueue->SignalAndFence(_fence.get(), _fenceValuesRender[_frameIndex], _fenceValuesRender[_frameIndex]);
	}

	void LoadRawResources()
	{
		std::ifstream hair_vertices_file;
		hair_vertices_file.open(L"../data/LongHairResources/hair_vertices.txt", std::ios::in | std::ios::binary);
		while (!hair_vertices_file.eof())
		{
			std::vector<Vertex> vertices;
			std::string line;
			std::getline(hair_vertices_file, line);
			size_t loc = line.find("//", 0);
			if (loc != std::string::npos)
			{
				std::istringstream ss_points(line);
				int npoints = 0;
				ss_points >> npoints;

				//texcoords
				std::string line2;
				std::getline(hair_vertices_file, line2);
				std::istringstream ss_texcoord(line2);

				Vertex vertex;
				char tempChar = 0;
				ss_texcoord >> tempChar >> vertex.uv.x >> vertex.uv.y;
				assert(tempChar == 't');

				uint32_t indexbase = _positions.size();
				for (int ipoint = 0; ipoint < npoints; ipoint++)
				{
					std::string line3;
					std::getline(hair_vertices_file, line3);
					std::istringstream ss_vertex(line3);
					ss_vertex >> vertex.position.x >> vertex.position.y >> vertex.position.z;
					vertex.position.z = -vertex.position.z;
					vertex.locate = static_cast<float>(ipoint) / npoints;
					_cpoints.push_back(vertex);
					vertices.push_back(vertex);
					_positions.push_back(core::float4(vertex.position, vertex.locate));

					if (ipoint > 0)
						_lines.push_back(core::uint2(_positions.size() - 2, _positions.size() - 1));
				}
				_strandOffsets.push_back(_positions.size());

				_vertices.push_back({ { indexbase, indexbase, indexbase + 1, indexbase + 2 }, (uint32_t)_strands.size() });
				
				for (int ipoint = 0; ipoint < npoints - 3; ipoint++)
					_vertices.push_back({ { indexbase + ipoint, indexbase + ipoint + 1, indexbase + ipoint + 2, indexbase + ipoint + 3 }, (uint32_t)_strands.size() });

				_vertices.push_back({{ indexbase + npoints - 3, indexbase + npoints - 2, indexbase + npoints - 1, indexbase + npoints - 1 }, (uint32_t)_strands.size()});
				_strands.push_back(vertices);
			}
		}
		hair_vertices_file.close();

		for (size_t istrand = 0; istrand < _strands.size(); ++istrand)
		{
			std::vector<Vertex> & strand = _strands[istrand];
			for (size_t ivertex = 0; ivertex < strand.size(); ++ivertex)
			{
				core::float3 tangentX;
				Vertex & vertex = strand[ivertex];
				if (ivertex == 0)
				{
					tangentX = strand[ivertex].position - strand[ivertex + 1].position;
				}
				else if(ivertex == strand.size() - 1)
					tangentX = strand[ivertex - 1].position - strand[ivertex].position;
				else
				{
					core::float3 tangent_before = (strand[ivertex - 1].position - strand[ivertex].position).normalize();
					core::float3 tangent_after = (strand[ivertex].position - strand[ivertex + 1].position).normalize();
					tangentX = (tangent_before + tangent_after) / 2;
				}
				tangentX.normalize();

				core::float3 tangentY;
				if (ivertex == 0)
				{
					core::float3 randVector = core::float3(rand(), rand(), rand()).normalize();
					while(randVector.length() < std::numeric_limits<float>::epsilon())
						randVector = core::float3(rand(), rand(), rand()).normalize();
					tangentY = (tangentX ^ randVector).normalize();
				}
				else
				{
					core::float3 rtationVector = strand[ivertex - 1].tangentX ^ tangentX;
					float theta = std::asin(rtationVector.length());
					
					tangentY = strand[ivertex - 1].tangentY * core::float4x4_rotation_axis(rtationVector, theta);
				}
				tangentY.normalize();

				vertex.tangentX = tangentX;
				vertex.tangentY = tangentY;
				_tangentXs.push_back(core::float4(tangentX, 0.0f));
				_tangentYs.push_back(core::float4(tangentY, float(ivertex) / strand.size()));
			}
		}

		auto D3DX_PI = 3.1415926f;
		auto random = []()
		{
			return (float((double)rand() / ((double)(RAND_MAX)+(double)(1))));
		};
		auto pfnBoxMullerTransform = [&](float & var1, float & var2)
		{
			float unifVar1 = random();
			float unifVar2 = random();
			float temp = std::sqrt(-2 * std::log(unifVar1));
			var1 = temp * std::cos(2 * D3DX_PI * unifVar2);
			var2 = temp * std::sin(2 * D3DX_PI * unifVar2);
		};
		for (size_t ijitteru = 0; ijitteru < COORD_JITTER_COUNT_U; ++ijitteru)
		{
			for (size_t ijitterv = 0; ijitterv < COORD_JITTER_COUNT_V; ++ijitterv)
			{
				core::float2 jitter = { random() , random() };
				pfnBoxMullerTransform(jitter.x, jitter.y);
				//float randomChoice = random();
				if (ijitterv < 5)
					jitter *= 1.0f / (ijitterv + 1.0f);
				//	jitter = {};
				//else if (ijitterv + 5 < ijitterv)
				//	jitter *= 0.4f;
				//else
				//	jitter *= 0.3f;
	/*			else if (randomChoice > 0.95f)
					jitter *= 1.2f;
				else if (randomChoice > 0.8f)
					jitter *= 0.8f;
				else
					jitter *= 0.12f;*/
				
				_coordJitters.push_back(jitter);
			}
		}

		core::image_codec_context icc;
		icc.get_format = [](core::image_type type, core::image_format format)
		{
			core::image_format result = format;
			switch (format.format)
			{
			case core::format::b8g8r8: result.format = core::format::b8g8r8a8; break;
			default:
				return core::image_get_format(type, format);
			}
			return result;
		};

		core::image_data_t linear_stiffness_map;
		auto err = core::image_load(icc, u8"../data/LongHairResources/stiffnessMap.dds", linear_stiffness_map);
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
		
		_constraints.resize(_positions.size());
		for(size_t istrand = 0; istrand < _strandOffsets.size(); ++istrand)
		{
			uint32_t ipointfirst = istrand > 0 ? _strandOffsets[istrand - 1] : 0;
			uint32_t npoints = _strandOffsets[istrand] - ipointfirst;
			uint32_t ipointlast = ipointfirst + npoints;

			// linear length
			for (size_t ipoint = 0; ipoint < npoints - 1; ++ipoint)
				_constraints[ipointfirst + ipoint].x = (_positions[ipointfirst + ipoint + 1] - _positions[ipointfirst + ipoint]).xyz.length();

			// linear stiffness
			for (size_t ipoint = 0; ipoint < npoints; ++ipoint)
			{
				uint32_t row = std::clamp<uint32_t>(uint32_t(_cpoints[ipointfirst + ipoint].uv.y * linear_stiffness_map.format.height), 0, linear_stiffness_map.format.height - 1);
				uint32_t col = std::clamp<uint32_t>(uint32_t(_cpoints[ipointfirst + ipoint].uv.x * linear_stiffness_map.format.width), 0, linear_stiffness_map.format.width - 1);

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
				_constraints[ipointfirst + ipoint].z = (_positions[ipointfirst + ipoint + 2] - _positions[ipointfirst + ipoint]).xyz.length();

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
		//_positions = vertices;
		//_indices = indices;
		//_lines = indices_lines;


		// colli

		std::ifstream collision_objects_file(L"../data/LongHairResources/collision_objects.txt", std::ios::in | std::ios::binary);
		std::string line;
		while (std::getline(collision_objects_file, line) && _num_collisionSpheres < MAX_COLLISION_SPHERES)
		{
			if (line.empty() || line[0] =='#')
				continue;

			// Sphere
			if (line.find("\'s\'") != std::string::npos)
			{
				// barycentricObstacle
				std::getline(collision_objects_file, line);
				bool barycentricObstacle = line.find("'barycentricObstacle'", 0) != std::string::npos;
				assert(barycentricObstacle);

				// basePoseToMeshBind
				std::getline(collision_objects_file, line);
				assert(line.find("'basePoseToMeshBind'", 0) != std::string::npos);
				std::string str_basePoseToMeshBind;
				std::getline(collision_objects_file, str_basePoseToMeshBind);
				std::istringstream iss_basePoseToMeshBind(str_basePoseToMeshBind);
				core::float4x4 basePoseToMeshBind;
				for (int index = 0; index < 16; ++index)
					iss_basePoseToMeshBind >> basePoseToMeshBind.af[index];
				
				// Scale
				std::string str_scale;
				std::getline(collision_objects_file, str_scale);
				std::istringstream iss_scale(str_scale);
				core::float3 scale;
				iss_scale >> scale.x >> scale.y >> scale.z;

				// Rotation
				std::string str_rotation;
				std::getline(collision_objects_file, str_rotation);
				std::istringstream iss_rotation(str_rotation);
				core::float3 rotation;
				iss_rotation >> rotation.x >> rotation.y >> rotation.z;

				// Translation
				std::string str_translation;
				std::getline(collision_objects_file, str_translation);
				std::istringstream iss_translation(str_translation);
				core::float3 translation;
				iss_translation >> translation.x >> translation.y >> translation.z;

				_collisionSphereTransforms[_num_collisionSpheres++] = core::float4x4_scale(scale) * core::float4x4_rotate(rotation) * core::float4x4_translate(translation);
			}
		}
	}

	void RenderThreadBase()
	{
		LoadRawResources();
		while(_rendering)
		{
			_rhi = _rhi_next;
			RenderThread();
			Reset();
		}
	}


	void RenderThread()
	{
		switch (_rhi)
		{
		case RHIType::D3D11:
			_factory = std::make_shared<RHI::RHID3D11::RHID3D11Factory>();
			break;
		case RHIType::D3D12:
			_factory = std::make_shared<RHI::RHID3D12::RHID3D12Factory>();
			break;
		case RHIType::Vulkan:
			//_factory = std::make_shared<RHI::RHIVulkan::RHIVulkanFactory>();
			_factory = std::make_shared<RHI::RHID3D12::RHID3D12Factory>();
			break;
		default:
			_factory = std::make_shared<RHI::RHID3D12::RHID3D12Factory>();
			break;
		}
		
		_factory->Load();

		RECT rcClient = {};
		GetClientRect(_hwnd, &rcClient);
		
		RHI::RenderTargetArgs rtparams = {};
		rtparams.hwnd = _hwnd;
		rtparams.nbuffers = _backBufferCount;
		rtparams.size = { uint32_t(rcClient.right - rcClient.left), uint32_t(rcClient.bottom - rcClient.top) };
		
		std::vector<RHI::RHIAdapterDesc> adapters = _factory->AdapterDescs();
		_device = _factory->CreateDevice(adapters[0].id);
		_fenceSimulate = _device->CreateFence(RHI::FenceFlag::None);
		_fence = _device->CreateFence(RHI::FenceFlag::None);
		_cmdqueue = _device->CreateCommandQueue(RHI::CommandType::Direct, RHI::CommandQueueFlag::None);
		_cmdqueue_compute = _device->CreateCommandQueue(RHI::CommandType::Compute, RHI::CommandQueueFlag::None);
		_rendertarget = _device->CreateRenderTargetForHWND(_cmdqueue.get(), rtparams);
		_cmdallocator = _device->CreateCommandAllocator(RHI::CommandType::Direct, rtparams.nbuffers);
		_cmdlist = _device->CreateCommandList(RHI::CommandType::Direct, _cmdallocator.get());
		_cmdallocator_compute = _device->CreateCommandAllocator(RHI::CommandType::Compute, rtparams.nbuffers);
		_cmdlist_compute = _device->CreateCommandList(RHI::CommandType::Compute, _cmdallocator_compute.get());

		_cmdqueue->SetName(u8"_cmdqueue");
		_cmdqueue_compute->SetName(u8"_cmdqueue_compute");
		_cmdallocator->SetName(u8"_cmdallocator");
		_cmdlist->SetName(u8"_cmdlist");
		_cmdallocator_compute->SetName(u8"_cmdallocator_compute");
		_cmdlist_compute->SetName(u8"_cmdlist_compute");

		_fenceValuesComputer.resize(rtparams.nbuffers);
		_fenceValuesRender.resize(rtparams.nbuffers);
		std::fill(_fenceValuesComputer.begin(), _fenceValuesComputer.end(), 0);
		std::fill(_fenceValuesRender.begin(), _fenceValuesRender.end(), 0);
		_frameIndex = 0;
		
		LoadAssets();


		core::counter_fps<float, 1> fps;
		float64_t time_last = core::datetime::system();

		core::sizei windowSize(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);

		core::recti sccisorrect = { 0, 0, windowSize.cx, windowSize.cy };
		RHI::ViewPort viewport = { 0, 0, windowSize.cx, windowSize.cy, 0.0f, 1.0f };

		core::float4x4 matrProj = core::float4x4_perspective_lh(3.14f / 3.0f, 16.0f / 9.0f, 0.1f, 5000.0f);

		uint32_t frameIndexLast = 0;
		while (_rendering && _rhi_next == _rhi)
		{
			auto elapse = core::datetime::system() - time_last;
			time_last += elapse;

			GetClientRect(_hwnd, &rcClient);
			core::sizei windowSize2(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
			if (windowSize2 != windowSize)
			{
				windowSize = windowSize2;
				for (uint32_t iframebuffer = 0; iframebuffer < _backBufferCount; ++iframebuffer)
				{
					_cmdqueue_compute->Fence(_fence.get(), _fenceValuesComputer[iframebuffer]);
					_cmdqueue->Fence(_fence.get(), _fenceValuesRender[iframebuffer]);
				}
				_rendertarget.reset();
				matrProj = core::float4x4_perspective_lh(3.14f / 3.0f, float(windowSize.cx) / windowSize.cy, 0.1f, 5000.0f);
				sccisorrect = { 0, 0, windowSize.cx, windowSize.cy };
				viewport = { 0, 0, (float)windowSize.cx, (float)windowSize.cy, 0.0f, 1.0f };

				rtparams.size = { uint32_t(rcClient.right - rcClient.left), uint32_t(rcClient.bottom - rcClient.top) };
				_rendertarget = _device->CreateRenderTargetForHWND(_cmdqueue.get(), rtparams);

				_fenceValueSimulate = 0;
				_fenceValueRender = 0;
				
				_fenceValuesComputer.resize(rtparams.nbuffers, 0);
				_fenceValuesRender.resize(rtparams.nbuffers, 0);
				std::fill(_fenceValuesComputer.begin(), _fenceValuesComputer.end(), 0);
				std::fill(_fenceValuesRender.begin(), _fenceValuesRender.end(), 0);
				_frameIndex = 0;
			}

			core::float4x4 matrView = core::float4x4_lookat_lh({ 0.0f, 0.0f, _view_z }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
			if (_rotating)
				_hair_rotate_z += elapse;

			core::float4x4 matrWorld = core::float4x4_rotate({ 0.0f, _hair_rotate_z * 3.14f * 0.5f, 0.0f }) * core::float4x4_translate(_hair_translate);

			// Simulate
			if (_simulate)
			{
				// SimulateConstantBuffer
				{
					SimulateConstantBuffer cbuffer = {};
					cbuffer.matrWorld = matrWorld;
					cbuffer.gravityAcceleration = 10.0f;
					cbuffer.gravityStrength = 10;
					cbuffer.timeElapse = std::min(float(elapse), 0.1f);
					cbuffer.angularStiffness = 1;
					cbuffer.numConstraintIterations = 20;
					cbuffer.eyePosition = { 0.0f, 0.0f, _view_z };
					cbuffer.numSphereImplicits = _num_collisionSpheres;
					for (uint32_t isphere = 0; isphere < _num_collisionSpheres; ++isphere)
					{
						cbuffer.CollisionSphereTransformations[isphere] = _collisionSphereTransforms[isphere] * matrWorld;
						cbuffer.CollisionSphereInverseTransformations[isphere] = core::float4x4_invert(cbuffer.CollisionSphereTransformations[isphere], nullptr);
					}
					//std::memcpy(_cbuffer_simulate->Data(), &cbuffer, sizeof(cbuffer));
					//.size = { uint32_t((sizeof(SceneConstantBuffer) + sizeof(SimulateConstantBuffer) + sizeof(SimulateGlobalConstBuffer) + 0xff) & ~0xff), 1 },
					std::memcpy((byte_t *)_cbuffer_staging->Data() + sizeof(SceneConstantBuffer), &cbuffer, sizeof(cbuffer));
				}

				_cmdallocator_compute->Reset(_frameIndex);
				_cmdlist_compute->Reset(_cmdallocator_compute.get(), _frameIndex);

				_cmdlist_compute->TransitionBarrier(_cbuffer_simulate.get(), RHI::ResourceState::CopyDest);
				_cmdlist_compute->CopyResource(_cbuffer_simulate.get(), _cbuffer_staging.get(), sizeof(SceneConstantBuffer), sizeof(SimulateConstantBuffer));
				_cmdlist_compute->TransitionBarrier(_cbuffer_simulate.get(), RHI::ResourceState::ConstBuffer);

				_cmdlist_compute->TransitionBarrier(_resource_curr_positions.get(), RHI::ResourceState::UnorderedAccess);
				_cmdlist_compute->SetPipelineState(_pipelinestate_simulate.get());
				_cmdlist_compute->SetResourcePacket(_resourcepacket_simulate.get());
				_cmdlist_compute->SetComputeResources(0, 0);

				_cmdlist_compute->Dispatch({ uint32_t(_strandOffsets.size() * 64), 1, 1 });
				_cmdlist_compute->TransitionBarrier(_resource_curr_positions.get(), RHI::ResourceState::VertexShaderResource);
				_cmdlist_compute->Close();
				_cmdqueue_compute->Excute(_cmdlist_compute.get());
				_fenceValuesComputer[_frameIndex] = ++_fenceValueSimulate;
				_cmdqueue_compute->Signal(_fenceSimulate.get(), _fenceValuesComputer[_frameIndex]);
			}

			// Render
			if (true)
			{
				if (_simulate)
					_cmdqueue->Wait(_fenceSimulate.get(), _fenceValuesComputer[_frameIndex]);
				
				{
					SceneConstantBuffer cbuffer;
					cbuffer.worldTransform = matrWorld;
					cbuffer.viewprojTransform = _simulate ? (matrView * matrProj) : (matrWorld * matrView * matrProj);
					cbuffer.tessFactor = _tessFactor;
					cbuffer.eyePosition = { 0.0f, 0.0f, _view_z };
					cbuffer.lightVector = core::float3(0.0f, 1.0f, 1.0f).normalize();
					std::memcpy((byte_t *)_cbuffer_staging->Data(), &cbuffer, sizeof(cbuffer));
				}

				_rendertarget->Begin();
				_cmdallocator->Reset(_frameIndex);
				_cmdlist->Reset(_cmdallocator.get(), _frameIndex);

				_cmdlist->SetViewPort(viewport);
				_cmdlist->SetScissorRect(sccisorrect);
				_cmdlist->TransitionBarrier(_rendertarget.get(), _frameIndex, RHI::ResourceState::RenderTarget);
				//_cmdlist->TransitionBarrier(_resource_curr_positions.get(), RHI::ResourceState::VertexShaderResource);
				//_cmdlist->UnorderedBarrier(_resource_curr_positions.get());
				_cmdlist->SetResourcePacket(_resourcepacket.get());
				_cmdlist->SetRenderTarget(_rendertarget.get(), _frameIndex);
				_cmdlist->ClearRenderTarget(_rendertarget.get(), _frameIndex, 0xffdddddd);

				_cmdlist->TransitionBarrier(_cbuffer_render.get(), RHI::ResourceState::CopyDest);
				_cmdlist->CopyResource(_cbuffer_render.get(), _cbuffer_staging.get(), 0, sizeof(SceneConstantBuffer));
				_cmdlist->TransitionBarrier(_cbuffer_render.get(), RHI::ResourceState::ConstBuffer);
				if (_mode.any(content_mode::bsline))
				{
					_cmdlist->SetPipelineState(_pipelinestate.get());
					_cmdlist->SetGraphicsResources(0, 0);
					_cmdlist->IASetVertexBuffer(_resource_vertexbuffer.get(), sizeof(HairVertex), sizeof(HairVertex) * _vertices.size());
					_cmdlist->IASetTopologyType(RHI::Topology::Point1PatchList);
					_cmdlist->DrawInstanced(_vertices.size(), 1, 0, 0);
				}

				if (_mode.any(content_mode::line))
				{
					_cmdlist->SetPipelineState(_pipelinestate_basic.get());
					_cmdlist->SetGraphicsResources(0, 0);
					_cmdlist->IASetVertexBuffer(_resource_vertexbuffer_lines.get(), sizeof(uint32_t), sizeof(core::uint2) * _lines.size());
					_cmdlist->IASetTopologyType(RHI::Topology::LineList);
					_cmdlist->DrawInstanced(_lines.size() * 2, 1, 0, 0);
				}

				_cmdlist->TransitionBarrier(_rendertarget.get(), _frameIndex, RHI::ResourceState::Present);
				_cmdlist->SetRenderTarget(nullptr, 0);
				_cmdlist->Close();
				_cmdqueue->Excute(_cmdlist.get());
				_rendertarget->Present(0);

				//----------------------------------------------------
				_fenceValuesRender[_frameIndex] = ++_fenceValueRender;
				_cmdqueue->Signal(_fence.get(), _fenceValuesRender[_frameIndex]);
				_frameIndex = _rendertarget->FrameIndex();
				_cmdqueue->Fence(_fence.get(), _fenceValuesRender[_frameIndex]);
			}

			fps.acc(1);

			core::logger_fps tt(__FILE__, __LINE__, 10);
			if (tt.ok())
				printf("\rfps = %.6f", fps.fps());
		}
	}
private:
	HWND _hwnd = NULL;

	std::atomic<bool> _rendering = true;

	enum class RHIType
	{
		D3D11 = 0,
		D3D12,
		Vulkan
	};
	RHIType _rhi = RHIType::D3D12;
	RHIType _rhi_next = RHIType::D3D12;

	// raw data
	size_t _nvertices = 0;
	size_t _nlines = 0;
	std::shared_ptr<RHI::RHIResource> _cbuffer_staging;
	std::shared_ptr<RHI::RHIResource> _cbuffer_render;
	std::shared_ptr<RHI::RHIResource> _cbuffer_simulate;

	std::future<void> _future;
	core::float2 _tessFactor = { 32.0f, 4.0f };


	bool _shortHair = false;
	std::vector<std::vector<Vertex>> _strands;
	std::vector<Vertex> _cpoints;
	std::vector<core::float4> _positions;
	std::vector<core::float4> _tangentXs;
	std::vector<core::float4> _tangentYs;
	std::vector<core::float2> _coordJitters;
	std::vector<HairVertex> _vertices;
	std::vector<core::uint2> _lines;
	std::vector<uint32_t> _strandOffsets;
	std::vector<core::float4> _constraints;
	float angularStiffness = 0.025f;
	// viewprojTransform
	bool _rotating = false;
	bool _simulate = true;
	float _hair_rotate_z = 0.0f;
	core::float3 _hair_translate;

	float _view_z = -14.0f;

	enum class content_mode
	{
		bsline = 0x1,
		line = 0x2,
	};
	using content_modes = core::bitflag<content_mode>;
	content_modes _mode = content_mode::bsline;

	uint32_t _num_collisionSpheres = 0;
	core::float4x4 _collisionSphereTransforms[MAX_COLLISION_SPHERES];
	uint32_t _backBufferCount = 3;

	//-------------------------
	std::shared_ptr<RHI::RHIFactory> _factory;
	std::shared_ptr<RHI::RHIDevice> _device;
	
	std::shared_ptr<RHI::RHIFence> _fenceSimulate;
	std::shared_ptr<RHI::RHIFence> _fence;
	std::shared_ptr<RHI::RHICommandQueue> _cmdqueue;
	std::shared_ptr<RHI::RHICommandQueue> _cmdqueue_compute;
	
	std::shared_ptr<RHI::RHIRenderTarget> _rendertarget;
	std::shared_ptr<RHI::RHICommandAllocator> _cmdallocator;
	std::shared_ptr<RHI::RHICommandList> _cmdlist;
	std::shared_ptr<RHI::RHICommandAllocator> _cmdallocator_compute;
	std::shared_ptr<RHI::RHICommandList> _cmdlist_compute;
	uint64_t _fenceValueSimulate = 0;
	uint64_t _fenceValueRender = 0;
	std::vector<uint64_t> _fenceValuesComputer;
	std::vector<uint64_t> _fenceValuesRender;
	uint32_t _frameIndex = 0;

	std::shared_ptr<RHI::RHIPipelineState> _pipelinestate;
	std::shared_ptr<RHI::RHIPipelineState> _pipelinestate_basic;
	std::shared_ptr<RHI::RHIPipelineState> _pipelinestate_simulate;


	enum RenderResourceId
	{
		RenderResourceId_CBV_ConstBuffer = 0,
		RenderResourceId_SRV_Positions,
		RenderResourceId_SRV_Tangents,
		RenderResourceId_SRV_CoordJitters,
		RenderResourceId_Count,
	};
	std::shared_ptr<RHI::RHIResourcePacket> _resourcepacket;

	enum SimulateResourceId
	{
		SimulateResourceId_CBV_ConstBuffer = 0,
		SimulateResourceId_SRV_StrandOffsets,
		SimulateResourceId_SRV_Constrains,
		SimulateResourceId_SRV_PositionsInit,
		SimulateResourceId_UAV_Positions,
		SimulateResourceId_UAV_PositionsPrev,
		SimulateResourceId_Count,
	};
	std::shared_ptr<RHI::RHIResourcePacket> _resourcepacket_simulate;
	
	std::shared_ptr<RHI::RHIResource> _resource_tangents;
	std::shared_ptr<RHI::RHIResource> _resource_init_positions;
	std::shared_ptr<RHI::RHIResource> _resource_prev_positions;
	std::shared_ptr<RHI::RHIResource> _resource_curr_positions;
	std::shared_ptr<RHI::RHIResource> _resource_coord_jitters;
	
	std::shared_ptr<RHI::RHIResource> _resource_vertexbuffer;
	std::shared_ptr<RHI::RHIResource> _resource_vertexbuffer_lines;
	

	std::shared_ptr<RHI::RHIResource> _resource_strandoffsets;
	std::shared_ptr<RHI::RHIResource> _resource_constraints;

	void Reset()
	{
		for (uint32_t iframebuffer = 0; iframebuffer < _backBufferCount; ++iframebuffer)
		{
			_cmdqueue_compute->Fence(_fence.get(), _fenceValuesComputer[iframebuffer]);
			_cmdqueue->Fence(_fence.get(), _fenceValuesRender[iframebuffer]);
		}
		_fenceValueSimulate = 0;
		_fenceValueRender = 0;
		
		_resource_constraints.reset();
		_resource_strandoffsets.reset();
		_resource_vertexbuffer_lines.reset();
		_resource_vertexbuffer.reset();
		_resource_coord_jitters.reset();
		_resource_curr_positions.reset();
		_resource_prev_positions.reset();
		_resource_init_positions.reset();
		_resource_tangents.reset();
		_resourcepacket_simulate.reset();
		_resourcepacket.reset();
		_pipelinestate_simulate.reset();
		_pipelinestate_basic.reset();
		_pipelinestate.reset();
		_cmdlist_compute.reset();
		_cmdallocator_compute.reset();
		_cmdlist.reset();
		_cmdallocator.reset();
		_rendertarget.reset();
		_cmdqueue_compute.reset();
		_cmdqueue.reset();
		_device.reset();
		_factory.reset();
	}
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
	else if (message == WM_MOUSEMOVE)
	{
		if (Render)
			Render->OnMouseMove(wParam, lParam);
	}
	else if (message == WM_MBUTTONDOWN)
	{
		if (Render)
			Render->OnMButtonDown(wParam, lParam);
	}
	else if (message == WM_MOUSEWHEEL)
	{
		int scroll = core::u32hi16(wParam) / WHEEL_DELTA;
		if (Render)
			Render->OnScrollV(scroll);
	}
	else if (message == WM_CLOSE)
	{
		if (Render)
			Render->Stop();
		core::quit(0);
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
	
	HINSTANCE hInstance = (HINSTANCE)core::instance();

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
	core::loop();
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
