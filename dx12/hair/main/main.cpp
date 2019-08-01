#include "stdafx.h"

//#include <ShellScalingAPI.h>
//#pragma comment(lib, "Shcore.lib")

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
//#pragma comment(lib, "E:/github/google/skia/out/x64d/skia.dll.lib")
#pragma comment(lib, "../../externals/skia/bin/x64/skia.dll.lib")
#include "main.h"

#include "hello.h"
#include "RHID3D12/RHID3D12.h"
#include "RHID3D12/RHID3D12Factory.h"


LRESULT CALLBACK DefaultWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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

	else if (message == WM_CLOSE)
	{
		win32::endLoop(0);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

std::atomic<bool> rendering = true;

std::shared_ptr<RHI::RHIResourcePacket> resourcePacket;
std::shared_ptr<RHI::RHIResource> vetexbuffer;
std::shared_ptr<RHI::RHIResource> indexbuffer;

std::shared_ptr<RHI::RHIResource> constbuffer;
std::shared_ptr<RHI::RHIResourceView> constbuffer_view;

std::shared_ptr<RHI::RHIResource> shaderresource;
std::shared_ptr<RHI::RHIResource> shaderresource_temp;
std::shared_ptr<RHI::RHIResourceView> shaderresource_view;

uint32_t nvertices = 0;
struct Vertex
{
	core::float3 position;
	core::float2 uv;
};

struct SceneConstantBuffer
{
	core::float4 offset;
};

void InitAssets(RHI::RHIDevice * device)
{
	Vertex vertices[] =
	{
		//{ { -1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
		//{ { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
		//{ { 1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
		//{ { -1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
		//
		{ { 1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
		{ { 1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
		//
		//{ { -25.f, +25.f, 0.0f }, { 0.0f, 0.0f } },
		//{ { +25.f, +25.f, 0.0f }, { 1.0f, 0.0f } },
		//{ { +25.f, -25.f, 0.0f }, { 1.0f, 1.0f } },
		//{ { -25.f, -25.f, 0.0f }, { 1.0f, 1.0f } },
	};
	nvertices = std::size(vertices);

	RHI::ResourcePacketArgs packetArgs =
	{
		.type = RHI::ResourcePacketType::Resource,
		.capacity = 2,
		.flags = RHI::ResourcePacketFlag::ShaderVisible
	};
	resourcePacket = device->CreateResourcePacket(packetArgs);

	uint16_t indices[] =
	{
		0, 1, 2, 1, 2, 3
	};

	RHI::ResourceArgs verticesParams = {};
	verticesParams.heap.type = RHI::HeapType::Upload;
	verticesParams.size.cx = sizeof(vertices);
	verticesParams.dimension = RHI::ResourceDimension::Raw;
	verticesParams.states = RHI::ResourceState::GenericRead;
	vetexbuffer = device->CreateResource(verticesParams);
	std::memcpy(vetexbuffer->Data(), vertices, sizeof(vertices));
	
	RHI::ResourceArgs indicesParams = {};
	indicesParams.heap.type = RHI::HeapType::Upload;
	indicesParams.size.cx = sizeof(vertices);
	indicesParams.dimension = RHI::ResourceDimension::Raw;
	indicesParams.states = RHI::ResourceState::GenericRead;
	indexbuffer = device->CreateResource(indicesParams);
	std::memcpy(indexbuffer->Data(), indices, sizeof(indices));

	RHI::ResourceArgs constParams = {};
	constParams.heap.type = RHI::HeapType::Upload;
	constParams.size.cx = (sizeof(SceneConstantBuffer) + 0xff) & ~0xff;
	constParams.dimension = RHI::ResourceDimension::Raw;
	constParams.states = RHI::ResourceState::GenericRead;
	constbuffer = device->CreateResource(constParams);

	RHI::ResourceViewArgs viewArgs =
	{
		.type = RHI::ResourceType::ConstBuffer,
	};
	constbuffer_view = resourcePacket->SetShaderResource(0, constbuffer.get(), viewArgs);

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
	
	drawing::image::image_data_t image;
	auto err = drawing::image::image_load(icc, u8"test.jpg", image);
	if (!err)
	{
		RHI::ResourceArgs imageTempParams = {};
		imageTempParams.heap.type = RHI::HeapType::Upload;
		imageTempParams.size.cx = image.format.width * image.format.height * 4;
		imageTempParams.dimension = RHI::ResourceDimension::Raw;
		imageTempParams.states = RHI::ResourceState::GenericRead;
		shaderresource_temp = device->CreateResource(imageTempParams);
		std::memcpy(shaderresource_temp->Data(), image.data, image.pitch * image.format.height);

		
		RHI::ResourceArgs imageParams = {};
		imageParams.heap.type = RHI::HeapType::Default;
		imageParams.size.cx = image.format.width;
		imageParams.size.cy = image.format.height;
		imageParams.dimension = RHI::ResourceDimension::Texture2D;
		imageParams.states = RHI::ResourceState::CopyDest;
		imageParams.format = core::pixelformat::bgra;
		shaderresource = device->CreateResource(imageParams);
		
		
		RHI::ResourceViewArgs imageArgs
		{
			.type = RHI::ResourceType::ShaderResource,
			.shaderresource =
			{
				.format = core::pixelformat::bgra,
				.dimension = RHI::ResourceViewDimension::Texture2D
			},
		};
		shaderresource_view = resourcePacket->SetShaderResource(1, shaderresource.get(), imageArgs);
	}
}

void RHIThread()
{
	RHI::RHID3D12::RHID3D12Factory RHI;
	RHI.Load();
	auto adapters = RHI.AdapterDescs();
	auto device = RHI.CreateDevice(adapters[0].uri);

	InitAssets(device.get());
	
	auto queue = device->CreateCommandQueue(RHI::CommandType::Direct, RHI::CommandQueueFlag::None);

	RHI::RenderTargetArgs rtparams =
	{
		.hwnd = g_hwnd,
	};
	auto rt = device->CreateRenderTargetForHWND(rtparams);
	auto cmdallocator = device->CreateCommandAllocator(RHI::CommandType::Direct);
	auto cmdlist = device->CreateCommandList(RHI::CommandType::Direct);

	RHI::PipelineStateArgs psargs =
	{
		.tables =
		{
			RHI::PipelineStateTables
			{
				.shader = RHI::Shader::All,
				.ranges =
				{
					{
						.type = RHI::DescripteorRangeType::ConstBuffer,
						.shaderRegister = 0,
					},
				},
			},
			
			RHI::PipelineStateTables
			{
				.shader = RHI::Shader::All,
				.ranges =
				{
					{
						.type = RHI::DescripteorRangeType::ShaderResource,
						.shaderRegister = 0,
					}
				},
			},
		}
	};

	RHI::PipelineStateTables table0 =
	{
		.shader = RHI::Shader::All,
		.ranges =
		{
			{
				.type = RHI::DescripteorRangeType::ConstBuffer,
				.shaderRegister = 0,
			},
			{
				.type = RHI::DescripteorRangeType::ShaderResource,
				.shaderRegister = 0,
			}
		},
	};

	RHI::SamplerArgs sampler0 =
	{
	};

	//psargs.tables.push_back(table0);
	psargs.samplers.push_back(sampler0);
	psargs.VS = u8"shaders.hlsl";
	psargs.VSMain = "VSMain";
	psargs.PS = u8"shaders.hlsl";
	psargs.PSMain = "PSMain";

	auto pso = device->CreatePipelineState(psargs);

	RECT rcClient;
	GetClientRect(g_hwnd, &rcClient);

	core::recti sccisorrect = { 0, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top };
	RHI::ViewPort viewport = { 0, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top };

	core::counter_fps<float, 3> fps;
	auto time_last = core::datetime::system();
	SceneConstantBuffer cbuffer;
	while (rendering)
	{
		auto elapse = core::datetime::system() - time_last;
		time_last += elapse;
		
		float speed = 0.5f;
		cbuffer.offset.x += speed * elapse;
		if (cbuffer.offset.x > 1.0f)
			cbuffer.offset.x = -1.5f;
		std::memcpy(constbuffer->Data(), &cbuffer, sizeof(cbuffer));
		
		rt->Begin();
		cmdallocator->Reset();
		cmdlist->Reset(cmdallocator.get());
		if (shaderresource_temp)
		{
			cmdlist->CopyResource(shaderresource.get(), shaderresource_temp.get());
			cmdlist->TransitionBarrier(shaderresource.get(), RHI::ResourceState::PixelShaderRerources);
		}
		
		cmdlist->SetViewPort(viewport);
		cmdlist->SetScissorRect(sccisorrect);
		cmdlist->TransitionBarrier(rt.get(), RHI::ResourceState::RenderTarget);
		cmdlist->SetRenderTarget(rt->CurrentRTV());
		cmdlist->ClearRenderTarget(0xff555555);
		cmdlist->SetPipelineState(pso.get());

		cmdlist->SetResourcePacket(resourcePacket.get());
		cmdlist->SetGraphicsResourceView(0, constbuffer_view.get());
		cmdlist->SetGraphicsResourceView(1, shaderresource_view.get());

		cmdlist->IASetIndexBuffer(indexbuffer.get(), 2, 12);
		cmdlist->IASetVertexBuffer(vetexbuffer.get(), sizeof(Vertex), sizeof(Vertex) * nvertices);
		cmdlist->IASetTopologyType(RHI::Topology::TriangleStrip);
		cmdlist->DrawInstanced(nvertices, 1, 0, 0);
		
		cmdlist->TransitionBarrier(rt.get(), RHI::ResourceState::Present);
		cmdlist->Close();
		rt->Excute(cmdlist.get());
		rt->End();
		rt->Present(0);
		fps.acc(1);

		if (shaderresource_temp)
		{
			shaderresource_temp.reset();
		}
		
		core::logger_fps tt(__FILE__, __LINE__, 10);
		if (tt.ok())
			printf("\rfps = %.6f", fps.fps());
	}

	vetexbuffer.reset();
	indexbuffer.reset();
	shaderresource.reset();
	shaderresource_temp.reset();
	resourcePacket.reset();
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

	g_hwnd = CreateWindowExW(
		0, win32::WINDOW_CLASS_NAME, L"Title", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080,
		NULL, NULL, hInstance, NULL);

	::ShowWindow(g_hwnd, SW_NORMAL);
	::UpdateWindow(g_hwnd);

	auto f_RHI = std::async(RHIThread);

    //LoadPipeLine((HWND)form->handle(), form->rect().round<int32_t>().size);
    //LoadAssets();

    //window->message += [](uint32_t msg, uintx_t wparam, intx_t lparam)
    //{
    //    if (msg == WM_PAINT)
    //        OnRender();
    //};

    //form->closed += []() {core::app().quit(0);};
    //form->show();

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
