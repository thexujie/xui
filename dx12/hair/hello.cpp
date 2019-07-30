#include "stdafx.h"
#include "hello.h"

static const UINT FrameCount = 2;
struct Vertex
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT4 color;
};

win32::comptr<ID3D12Device> m_device;
win32::comptr<ID3D12CommandQueue> m_commandQueue;
win32::comptr<ID3D12CommandAllocator> m_commandAllocator;
win32::comptr<ID3D12CommandAllocator> m_bundleAllocator;
win32::comptr<ID3D12RootSignature> m_rootSignature;
win32::comptr<ID3D12DescriptorHeap> m_rtvHeap;
win32::comptr<ID3D12PipelineState> m_pipelineState;
win32::comptr<ID3D12GraphicsCommandList> m_commandList;
win32::comptr<ID3D12GraphicsCommandList> m_bundle;
UINT m_rtvDescriptorSize;

win32::comptr<IDXGISwapChain3> m_swapChain;
D3D12_VIEWPORT m_viewport;
D3D12_RECT m_scissorRect;

UINT m_frameIndex = 0;
HANDLE m_fenceEvent;
win32::comptr<ID3D12Fence> m_fence;
UINT64 m_fenceValue;
float m_aspectRatio;

win32::comptr<ID3D12Resource> m_renderTargets[FrameCount];

win32::comptr<ID3D12Resource> m_vertexBuffer;
D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;


void WaitForPreviousFrame()
{
    // WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
    // This is code implemented as such for simplicity. The D3D12HelloFrameBuffering
    // sample illustrates how to use fences for efficient resource usage and to
    // maximize GPU utilization.

    // Signal and increment the fence value.
    const UINT64 fence = m_fenceValue;
    win32::throw_if_failed(m_commandQueue->Signal(m_fence.get(), fence));
    m_fenceValue++;

    // Wait until the previous frame is finished.
    if (m_fence->GetCompletedValue() < fence)
    {
        win32::throw_if_failed(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }

    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

void LoadPipeLine(HWND hwnd, core::sizei size)
{
    m_frameIndex = 0;
    m_viewport = { 0, 0, (float)size.cx, (float)size.cy };
    m_scissorRect = { 0, 0, (int)size.cx, (int)size.cy };
    m_rtvDescriptorSize = 0;

    m_aspectRatio = static_cast<float>(size.cx) / static_cast<float>(size.cy);

    core::bitflag<uint32_t> dxgiFactoryFlags;
#ifdef _DEBUG
    win32::comptr<ID3D12Debug> debugInterface;
    auto hr = D3D12GetDebugInterface(IID_PPV_ARGS(debugInterface.getpp()));
    if (SUCCEEDED(hr))
    {
        debugInterface->EnableDebugLayer();
        dxgiFactoryFlags.set(DXGI_CREATE_FACTORY_DEBUG, true);
    }
#endif

    win32::comptr<IDXGIFactory3> dxgiFactory;
    win32::throw_if_failed(CreateDXGIFactory(IID_PPV_ARGS(dxgiFactory.getpp())));

    win32::comptr<IDXGIAdapter1> adapter;
    for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != dxgiFactory->EnumAdapters1(adapterIndex, adapter.getpp()); ++adapterIndex)
    {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            continue;

        if (SUCCEEDED(D3D12CreateDevice(adapter.get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            break;

        adapter.reset();
    }

    win32::throw_if_failed(D3D12CreateDevice(adapter.get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(m_device.getpp())));

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    win32::throw_if_failed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_commandQueue.getpp())));


    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = FrameCount;
    swapChainDesc.Width = size.cx;
    swapChainDesc.Height = size.cy;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    win32::comptr<IDXGISwapChain1> swapChain;
    win32::throw_if_failed(dxgiFactory->CreateSwapChainForHwnd(
        m_commandQueue.get(),        // Swap chain needs the queue so that it can force a flush on it.
        hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        swapChain.getpp()
    ));

    win32::throw_if_failed(dxgiFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));

    m_swapChain = swapChain.as<IDXGISwapChain3>();
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    // Create descriptor heaps.
    {
        // Describe and create a render target view (RTV) descriptor heap.
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = FrameCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        win32::throw_if_failed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(m_rtvHeap.getpp())));

        m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }


    // Create frame resources.
    {
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

        // Create a RTV for each frame.
        for (UINT n = 0; n < FrameCount; n++)
        {
            win32::throw_if_failed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(m_renderTargets[n].getpp())));
            m_device->CreateRenderTargetView(m_renderTargets[n].get(), nullptr, rtvHandle);
            rtvHandle.ptr += m_rtvDescriptorSize;
        }
    }

    win32::throw_if_failed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_commandAllocator.getpp())));
    win32::throw_if_failed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE, IID_PPV_ARGS(m_bundleAllocator.getpp())));
}

void LoadAssets()
{
    // Create an empty root signature.
    {
        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.NumParameters = 0;
        rootSignatureDesc.pParameters = nullptr;
        rootSignatureDesc.NumStaticSamplers = 0;
        rootSignatureDesc.pStaticSamplers = nullptr;
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        win32::comptr<ID3DBlob> signature;
        win32::comptr<ID3DBlob> error;
        win32::throw_if_failed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, signature.getpp(), error.getpp()));
        win32::throw_if_failed(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(m_rootSignature.getpp())));
    }

    // Create the pipeline state, which includes compiling and loading shaders.
    {
        win32::comptr<ID3DBlob> vertexShader;
        win32::comptr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
        // Enable better shader debugging with the graphics debugging tools.
        UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        UINT compileFlags = 0;
#endif

        win32::throw_if_failed(D3DCompileFromFile(core::current_path().append(L"shaders.hlsl").c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, vertexShader.getpp(), nullptr));
        win32::throw_if_failed(D3DCompileFromFile(core::current_path().append(L"shaders.hlsl").c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, pixelShader.getpp(), nullptr));

        // Define the vertex input layout.
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        D3D12_RASTERIZER_DESC descRasterizer = {};
        descRasterizer.FillMode = D3D12_FILL_MODE_SOLID;
        descRasterizer.CullMode = D3D12_CULL_MODE_BACK;
        descRasterizer.FrontCounterClockwise = FALSE;
        descRasterizer.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
        descRasterizer.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
        descRasterizer.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
        descRasterizer.DepthClipEnable = TRUE;
        descRasterizer.MultisampleEnable = FALSE;
        descRasterizer.AntialiasedLineEnable = FALSE;
        descRasterizer.ForcedSampleCount = 0;
        descRasterizer.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

        D3D12_BLEND_DESC descBlend = {};
        descBlend.AlphaToCoverageEnable = FALSE;
        descBlend.IndependentBlendEnable = FALSE;
        const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
        {
            FALSE, FALSE,
            D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
            D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
            D3D12_LOGIC_OP_NOOP,
            D3D12_COLOR_WRITE_ENABLE_ALL,
        };
        for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
            descBlend.RenderTarget[i] = defaultRenderTargetBlendDesc;

        // Describe and create the graphics pipeline state object (PSO).
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = m_rootSignature.get();
        psoDesc.VS = { vertexShader->GetBufferPointer(), vertexShader->GetBufferSize() };
        psoDesc.PS = { pixelShader->GetBufferPointer(), pixelShader->GetBufferSize() };
        psoDesc.RasterizerState = descRasterizer;
        psoDesc.BlendState = descBlend;
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;
        win32::throw_if_failed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(m_pipelineState.getpp())));
    }

    // Create the command list.
    win32::throw_if_failed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.get(), /*m_pipelineState.get()*/m_pipelineState.get(), IID_PPV_ARGS(m_commandList.getpp())));

    // Command lists are created in the recording state, but there is nothing
    // to record yet. The main loop expects it to be closed, so close it now.
    win32::throw_if_failed(m_commandList->Close());

    // Create the vertex buffer.
    {
        // Define the geometry for a triangle.
        Vertex triangleVertices[] =
        {
            { { 0.0f, 0.25f * m_aspectRatio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
            { { 0.25f, -0.25f * m_aspectRatio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
            { { -0.25f, -0.25f * m_aspectRatio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
        };

        const UINT vertexBufferSize = sizeof(triangleVertices);
        D3D12_HEAP_PROPERTIES heapProperties = {};
        heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
        heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        heapProperties.CreationNodeMask = 1;
        heapProperties.VisibleNodeMask = 1;

        D3D12_RESOURCE_DESC descResource = {};
        descResource.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        descResource.Alignment = 0;
        descResource.Width = vertexBufferSize;
        descResource.Height = 1;
        descResource.DepthOrArraySize = 1;
        descResource.MipLevels = 1;
        descResource.Format = DXGI_FORMAT_UNKNOWN;
        descResource.SampleDesc.Count = 1;
        descResource.SampleDesc.Quality = 0;
        descResource.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        descResource.Flags = D3D12_RESOURCE_FLAG_NONE;


        // Note: using upload heaps to transfer static data like vert buffers is not 
        // recommended. Every time the GPU needs it, the upload heap will be marshalled 
        // over. Please read up on Default Heap usage. An upload heap is used here for 
        // code simplicity and because there are very few verts to actually transfer.
        win32::throw_if_failed(m_device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &descResource,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(m_vertexBuffer.getpp())));

        // Copy the triangle data to the vertex buffer.
        UINT8 * pVertexDataBegin;
        D3D12_RANGE readRange = { 0, 0 };        // We do not intend to read from this resource on the CPU.
        win32::throw_if_failed(m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void **>(&pVertexDataBegin)));
        memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
        m_vertexBuffer->Unmap(0, nullptr);

        // Initialize the vertex buffer view.
        m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
        m_vertexBufferView.StrideInBytes = sizeof(Vertex);
        m_vertexBufferView.SizeInBytes = vertexBufferSize;
    }

    // Create and record the bundle.
    {
        win32::throw_if_failed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_BUNDLE, m_bundleAllocator.get(), /*m_pipelineState.get()*/NULL, IID_PPV_ARGS(m_bundle.getpp())));
        //m_bundle->SetGraphicsRootSignature(m_rootSignature.get());
        //m_bundle->SetPipelineState(m_pipelineState.get());
        //m_bundle->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_bundle->IASetVertexBuffers(0, 1, &m_vertexBufferView);
        //m_bundle->DrawInstanced(3, 1, 0, 0);
        win32::throw_if_failed(m_bundle->Close());
    }

    // Create synchronization objects and wait until assets have been uploaded to the GPU.
    {
        win32::throw_if_failed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.getpp())));
        m_fenceValue = 1;

        // Create an event handle to use for frame synchronization.
        m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (m_fenceEvent == nullptr)
        {
            win32::throw_if_failed(HRESULT_FROM_WIN32(GetLastError()));
        }

        // Wait for the command list to execute; we are reusing the same command 
        // list in our main loop but for now, we just want to wait for setup to 
        // complete before continuing.
        WaitForPreviousFrame();
    }
    
}



void PopulateCommandList()
{
    // Command list allocators can only be reset when the associated 
    // command lists have finished execution on the GPU; apps should use 
    // fences to determine GPU execution progress.
    win32::throw_if_failed(m_commandAllocator->Reset());

    // However, when ExecuteCommandList() is called on a particular command 
    // list, that command list can then be reset at any time and must be before 
    // re-recording.
    win32::throw_if_failed(m_commandList->Reset(m_commandAllocator.get(), m_pipelineState.get()));

    // Set necessary state.
    //m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
    m_commandList->RSSetViewports(1, &m_viewport);
    m_commandList->RSSetScissorRects(1, &m_scissorRect);

    // Indicate that the back buffer will be used as a render target.
    D3D12_RESOURCE_BARRIER barrier;
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = m_renderTargets[m_frameIndex].get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    m_commandList->ResourceBarrier(1, &barrier);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
    rtvHandle.ptr += m_frameIndex * m_rtvDescriptorSize;
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    // Record commands.
    const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    m_commandList->SetGraphicsRootSignature(m_rootSignature.get());
    // Execute the commands stored in the bundle.
    m_commandList->ExecuteBundle(m_bundle.get());
    m_commandList->SetPipelineState(m_pipelineState.get());
    m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    m_commandList->DrawInstanced(3, 1, 0, 0);
    // Indicate that the back buffer will now be used to present.
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    m_commandList->ResourceBarrier(1, &barrier);

    win32::throw_if_failed(m_commandList->Close());
}

void OnRender()
{
    // Record all the commands we need to render the scene into the command list.
    PopulateCommandList();

    // Execute the command list.
    ID3D12CommandList * ppCommandLists[] = { m_commandList.get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // Present the frame.
    win32::throw_if_failed(m_swapChain->Present(1, 0));

    WaitForPreviousFrame();
}