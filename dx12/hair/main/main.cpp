#include "stdafx.h"

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

#include <DirectXMath.h>
static std::atomic<bool> rendering = true;

struct Vertex
{
	core::float3 position;
	core::float2 uv;
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
			_rotate_time_last = core::datetime::system();
			break;
		default:
			break;
		}

		std::wstring title = core::wformat("TessFactor=[", std::setprecision(4), _tessFactor.x, L",", _tessFactor.y, L"]");
		::SetWindowTextW(_hwnd, title.c_str());
	}
	
	void Start()
	{
		LoadRawResources();
		LoadPipeline();
		LoadAssets();
		_future = std::async(&HairRender::RenderThread, this);
	}
	
	void RenderThread()
	{
		core::counter_fps<float, 3> fps;
		_rotate_time_last = core::datetime::system();
		SceneConstantBuffer cbuffer;

		RECT rcClient;
		GetClientRect(_hwnd, &rcClient);

		core::sizei windowSize(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);

		core::recti sccisorrect = { 0, 0, windowSize.cx, windowSize.cy };
		RHI::ViewPort viewport = { 0, 0, windowSize.cx, windowSize.cy, 0.0f, 1.0f};

		core::float4x4 matrView = core::float4x4_lookat_lh({ 0.0f, 0.0f, -14.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
		core::float4x4 matrProj = core::float4x4_perspective_lh(3.14f / 3.0f, 16.0f / 9.0f, 0.1f, 5000.0f);

		while (rendering)
		{
			GetClientRect(_hwnd, &rcClient);
			core::sizei windowSize2(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
			if (windowSize2 != windowSize)
			{
				windowSize = windowSize2;
				_rendertarget.reset();
				matrView = core::float4x4_lookat_lh({ 0.0f, 0.0f, -14.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
				matrProj = core::float4x4_perspective_lh(3.14f / 3.0f, 16.0f / 9.0f, 0.1f, 5000.0f);
				sccisorrect = { 0, 0, windowSize.cx, windowSize.cy };
				viewport = { 0, 0, (float)windowSize.cx, (float)windowSize.cy, 0.0f, 1.0f };
				RHI::RenderTargetArgs rtparams = {};
				rtparams.hwnd = _hwnd;
				_rendertarget = _device->CreateRenderTargetForHWND(rtparams);
			}

			if (_rotating)
			{
				auto elapse = core::datetime::system() - _rotate_time_last;
				_rotate_time_last += elapse;
				_rotate += elapse;
			}

			core::float4x4 matrWorld = core::float4x4_rotate({ 0.0f, _rotate * 3.14f * 0.5f, 0.0f });
			cbuffer.transform = matrWorld * matrView * matrProj;
			cbuffer.tessFactor = _tessFactor;
			std::memcpy(_constbuffer->Data(), &cbuffer, sizeof(cbuffer));

			_rendertarget->Begin();
			_cmdallocator->Reset();
			_cmdlist->Reset(_cmdallocator.get());

			_cmdlist->SetViewPort(viewport);
			_cmdlist->SetScissorRect(sccisorrect);
			_cmdlist->TransitionBarrier(_rendertarget.get(), RHI::ResourceState::RenderTarget);
			_cmdlist->SetRenderTarget(_rendertarget->CurrentRTV());
			_cmdlist->ClearRenderTarget(0xffcccccc);

			_cmdlist->SetResourcePacket(_resourcepacket.get());

			//_cmdlist->SetPipelineState(_pipelinestate.get());
			//_cmdlist->IASetIndexBuffer(_indexbuffer.get(), sizeof(uint16_t), sizeof(uint16_t) * _nindices);
			//_cmdlist->SetGraphicsResourceView(0, _constbuffer_view.get());
			//_cmdlist->IASetTopologyType(RHI::Topology::Point4PatchList);
			//_cmdlist->DrawIndexedInstanced(_nindices, 1, 0, 0, 0);
			
			_cmdlist->SetPipelineState(_pipelinestate_basic.get());
			_cmdlist->SetGraphicsResourceView(0, _constbuffer_view.get());
			_cmdlist->IASetVertexBuffer(_vetexbuffer.get(), sizeof(Vertex), sizeof(Vertex) * _nvertices);
			_cmdlist->IASetIndexBuffer(_indexbuffer_lines.get(), sizeof(uint16_t), sizeof(uint16_t) * _nindices_lines);
			_cmdlist->IASetTopologyType(RHI::Topology::LineList);
			_cmdlist->DrawIndexedInstanced(_nindices_lines, 1, 0, 0, 0);

			_cmdlist->TransitionBarrier(_rendertarget.get(), RHI::ResourceState::Present);
			_cmdlist->SetRenderTarget(nullptr);
			_cmdlist->Close();
			_rendertarget->Excute(_cmdlist.get());
			_rendertarget->Present(0);
			_rendertarget->End();
			fps.acc(1);

			core::logger_fps tt(__FILE__, __LINE__, 10);
			if (tt.ok())
				printf("\rfps = %.6f", fps.fps());
		}
	}
	
	void LoadPipeline()
	{
		RHI.Load();
		std::vector<RHI::RHIAdapterDesc> adapters = RHI.AdapterDescs();
		_device = RHI.CreateDevice(adapters[0].uri);
		_cmdqueue = _device->CreateCommandQueue(RHI::CommandType::Direct, RHI::CommandQueueFlag::None);
		RHI::RenderTargetArgs rtparams = {};
		rtparams.hwnd = _hwnd;
		_rendertarget = _device->CreateRenderTargetForHWND(rtparams);
		_cmdallocator = _device->CreateCommandAllocator(RHI::CommandType::Direct);
		_cmdlist = _device->CreateCommandList(RHI::CommandType::Direct);
	}

	void LoadAssets()
	{
		RHI::PipelineStateTableRange range0, range1;
		range0.type = RHI::DescripteorRangeType::ConstBuffer;
		range0.shaderRegister = 0;
		range1.type = RHI::DescripteorRangeType::ShaderResource;
		range1.shaderRegister = 0;

		RHI::PipelineStateTable table;
		table.shader = RHI::Shader::All;
		table.ranges.push_back(range0);
		//table.ranges.push_back(range1);

		RHI::PipelineStateArgs psargs = {};
		psargs.tables.push_back(table);
		psargs.samplers.push_back(RHI::SamplerArgs());
		psargs.rasterize.depthClip = false;

		psargs.topology = RHI::TopologyType::Line;
		std::u8string path_basic = u8"../data/shaders/basic.hlsl";
		psargs.VS = path_basic;
		psargs.VSMain = "VSMain";
		psargs.PS = path_basic;
		psargs.PSMain = "PSMain";
		_pipelinestate_basic = _device->CreatePipelineState(psargs);
		
		std::u8string path = u8"../data/shaders/bspline.hlsl";
		psargs.VS = path;
		psargs.VSMain = "VSMain";
		psargs.HS = path;
		psargs.HSMain = "HSMain";
		psargs.DS = path;
		psargs.DSMain = "DSMain";
		psargs.GS = path;
		psargs.GSMain = "GSMain";
		psargs.PS = path;
		psargs.PSMain = "PSMain";
		psargs.topology = RHI::TopologyType::Patch;
		_pipelinestate = _device->CreatePipelineState(psargs);

		RHI::ResourcePacketArgs packetArgs = {};
		packetArgs.type = RHI::ResourcePacketType::Resource;
		packetArgs.capacity = 2;
		packetArgs.flags = RHI::ResourcePacketFlag::ShaderVisible;
		_resourcepacket = _device->CreateResourcePacket(packetArgs);

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
		_vetexbuffer = _device->CreateResource(verticesParams);

		_cmdlist->CopyResource(_vetexbuffer.get(), vetexbuffer_UL.get());

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

		_cmdlist->CopyResource(_indexbuffer.get(), indexbuffer_UL.get());

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

		_cmdlist->CopyResource(_indexbuffer_lines.get(), indexLinesbuffer_UL.get());
		
		RHI::ResourceArgs constParams = {};
		constParams.heap.type = RHI::HeapType::Upload;
		constParams.size.cx = (sizeof(SceneConstantBuffer) + 0xff) & ~0xff;
		constParams.dimension = RHI::ResourceDimension::Raw;
		constParams.states = RHI::ResourceState::GenericRead;
		_constbuffer = _device->CreateResource(constParams);

		RHI::ResourceViewArgs viewArgs = {};
		viewArgs.type = RHI::ResourceType::ConstBuffer;
		_constbuffer_view = _resourcepacket->SetShaderResource(0, _constbuffer.get(), viewArgs);

		_cmdlist->TransitionBarrier(_vetexbuffer.get(), RHI::ResourceState::VertexBuffer);
		_cmdlist->TransitionBarrier(_indexbuffer.get(), RHI::ResourceState::IndexBuffer);
		_cmdlist->TransitionBarrier(_indexbuffer_lines.get(), RHI::ResourceState::IndexBuffer);
		_cmdlist->Close();
		_rendertarget->Excute(_cmdlist.get());
		_rendertarget->End();
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

				for (int ipoint = 0; ipoint < npoints; ipoint++)
				{
					std::string line3;
					std::getline(file, line3);
					std::istringstream ss_vertex(line3);
					ss_vertex >> vertex.position.x >> vertex.position.y >> vertex.position.z;
					vertex.position.z = -vertex.position.z;

					const int g_strandPointsMax = 64;
					vertices.push_back(vertex);
					_vertices.push_back(vertex);
					if (ipoint >= 3)
					{
						_indices.push_back(_vertices.size() - 4);
						_indices.push_back(_vertices.size() - 3);
						_indices.push_back(_vertices.size() - 2);
						_indices.push_back(_vertices.size() - 1);
					}

					if (ipoint >= 1)
					{
						_indices_lines.push_back(_vertices.size() - 2);
						_indices_lines.push_back(_vertices.size() - 1);
					}
				}
			}
			_controlPoints.push_back(vertices);
		}
		file.close();

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
	std::shared_ptr<RHI::RHIResourcePacket> _resourcepacket;
	
	std::shared_ptr<RHI::RHIResource> _vetexbuffer;
	std::shared_ptr<RHI::RHIResource> _indexbuffer;
	std::shared_ptr<RHI::RHIResource> _indexbuffer_lines;
	size_t _nvertices = 0;
	size_t _nindices = 0;
	size_t _nindices_lines = 0;
	std::shared_ptr<RHI::RHIResource> _constbuffer;
	std::shared_ptr<RHI::RHIResourceView> _constbuffer_view;

	std::future<void> _future;
	core::float2 _tessFactor = { 1.0f, 10.0f };
	
	std::vector<std::vector<Vertex>> _controlPoints;
	std::vector<Vertex> _vertices;
	std::vector<uint16_t> _indices;
	std::vector<uint16_t> _indices_lines;


	// transform

	bool _rotating = true;
	float _rotate = 0.0f;
	float64_t _rotate_time_last = 0.0;
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
