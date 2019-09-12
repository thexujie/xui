#include "stdafx.h"
#include "D3D11VideoPlayer.h"

#include "win32/windows.h"
#include <d3d11.h>
#include <DirectXMath.h>

//#define DXGI_ERROR_DEVICE_REMOVED 0x887A0005

namespace win32 { namespace d3d
{
    using namespace common;
    using namespace player;

#pragma pack(push,1)
    struct RenderParams
    {
        std::array<float, 4 * 4> matrMVP;
        std::array<float, 4 * 4> matrYUV2RGB;
        std::array<float, 4> textureOffset;
    };

    struct VertexT
    {
        std::array<float, 3> pos;
        std::array<float, 2> tex;
    };
#pragma pack(pop)

    D3D11VideoPlayer::D3D11VideoPlayer(std::shared_ptr<D3D11Device> device, std::shared_ptr<IVideoRenderWindow> window):_device(device), _window(window)
    {
        
    }

    D3D11VideoPlayer::D3D11VideoPlayer(std::shared_ptr<DXGIFactory> dxgi, deviceid_t deviceId, std::shared_ptr<IVideoRenderWindow> window): _window(window)
    {
        _device = std::make_shared<D3D11Device>(dxgi, deviceId);
    }

    D3D11VideoPlayer::~D3D11VideoPlayer()
    {
        if (_thPlay.joinable())
        {
            _flags = _flags | common::flag_stop;
            _cond.notify_all();
            _thPlay.join();
        }
    }

    VideoFormat D3D11VideoPlayer::GetDeviceVideoFormat() const
    {
        if (!_swapChain && const_cast<D3D11VideoPlayer *>(this)->initRender() < 0)
            return {};

        DXGI_SWAP_CHAIN_DESC desc = {};
        _swapChain->GetDesc(&desc);
        return { (int32_t)desc.BufferDesc.Width, (int32_t)desc.BufferDesc.Height, DXGIFormat2AvPixelFormat(desc.BufferDesc.Format)};
    }

    void D3D11VideoPlayer::videoThread()
    {
        VideoPlayer::videoThread();
    }

    error_e D3D11VideoPlayer::render(VideoBuffer & buffer)
    {
        if (!_device)
            return error_state;

        //std::lock_guard<std::mutex> lock(_device->GetMutex());

        com_ptr<ID3D11Device> dxdevice = _device->GetPointer();
        if (!dxdevice)
            return error_state;

        if (!_swapChain && initRender() < 0)
            return error_inner;

        HWND hWnd = (HWND)_window->GetHandle();
        RECT rcClient;
        GetClientRect(hWnd, &rcClient);
        int32_t windowW = std::max(rcClient.right - rcClient.left, 16L);
        int32_t windowH = std::max(rcClient.bottom - rcClient.top, 16L);

        // 渲染
        DXGI_SWAP_CHAIN_DESC backBufferDesc = {};
        _swapChain->GetDesc(&backBufferDesc);

        double_t tsCurr = get_time_hr();
        if (!IsIconic(hWnd) && (windowW != backBufferDesc.BufferDesc.Width || windowH != backBufferDesc.BufferDesc.Height || _hr == DXGI_ERROR_DEVICE_REMOVED))
        {
            if (std::isnan(_tsLastReset))
                _tsLastReset = tsCurr;

            if (tsCurr - _tsLastReset > 0.5f)
            {
                error_e error = resetSwapChain(windowW, windowH);
                if (error < 0)
                    return error_inner;

                _swapChain->GetDesc(&backBufferDesc);
                _tsLastReset = std::nan(0);
            }
        }

        com_ptr<ID3D11DeviceContext> deviceContext;
        dxdevice->GetImmediateContext(&deviceContext);
        if (!deviceContext)
            return error_inner;
        
        if (!buffer.avframe)
        {
            ID3D11RenderTargetView * rtvs[] = { _renderTargetView.get() };
            deviceContext->OMSetRenderTargets(1, rtvs, 0);

            float ClearColor[4] = { 0, 0, 0, 0 };
            deviceContext->ClearRenderTargetView(_renderTargetView.get(), ClearColor);

            //_hr = renderDevice->BeginScene();
            //assert(SUCCEEDED(_hr) || _hr == DXGI_ERROR_DEVICE_REMOVED);
            //if (FAILED(_hr))
            //    return _hr == DXGI_ERROR_DEVICE_REMOVED ? error_ok : error_inner;

            //_hr = renderDevice->EndScene();
            //assert(SUCCEEDED(_hr) || _hr == DXGI_ERROR_DEVICE_REMOVED);
            //if (FAILED(_hr))
            //    return _hr == DXGI_ERROR_DEVICE_REMOVED ? error_ok : error_inner;
            return error_ok;
        }

        VideoFormat videoFormat = buffer.format;
        ID3D11Texture2D * videoTeture = nullptr;
        int32_t videoIndex = 0;
        ID3D11ShaderResourceView * videoViewY = nullptr;
        ID3D11ShaderResourceView * videoViewUV = nullptr;
        DXGI_FORMAT stagingFormat = DXGI_FORMAT_UNKNOWN;
        switch (buffer.format.pixelFormat)
        {
        case AV_PIX_FMT_BGR0:
            stagingFormat = DXGI_FORMAT_B8G8R8X8_UNORM;
            break;
        case AV_PIX_FMT_NV12:
            stagingFormat = DXGI_FORMAT_NV12;
            break;
        case AV_PIX_FMT_YUV420P:
            stagingFormat = DXGI_FORMAT_NV12;
            break;
        case AV_PIX_FMT_D3D11:
        {
            videoTeture = reinterpret_cast<ID3D11Texture2D *>(buffer.avframe->data[0]);
            videoIndex = (int)buffer.avframe->data[1];
            videoViewY = reinterpret_cast<ID3D11ShaderResourceView *>(buffer.avframe->data[4]);
            videoViewUV = reinterpret_cast<ID3D11ShaderResourceView *>(buffer.avframe->data[5]);
            stagingFormat = DXGI_FORMAT_UNKNOWN;

            D3D11_TEXTURE2D_DESC desc = {};
            videoTeture->GetDesc(&desc);
            videoFormat.width = (int32_t)desc.Width;
            videoFormat.height = (int32_t)desc.Height;
            break;
        }
        default:
            stagingFormat = DXGI_FORMAT_UNKNOWN;
            break;
        }

        //创建 Surface
        if (_videoBuffer)
        {
            D3D11_TEXTURE2D_DESC surfaceDesc = {};
            _videoBuffer->GetDesc(&surfaceDesc);
            if (surfaceDesc.Width != buffer.format.width || surfaceDesc.Height != buffer.format.height ||
                stagingFormat != surfaceDesc.Format)
                _videoBuffer.reset();
        }

        if (!_videoBuffer && stagingFormat != DXGI_FORMAT_UNKNOWN)
        {
            D3D11_TEXTURE2D_DESC texturesDesc = { 0 };
            texturesDesc.Width = buffer.format.width;
            texturesDesc.Height = buffer.format.height;
            texturesDesc.MipLevels = 1;
            texturesDesc.ArraySize = 1;
            texturesDesc.Format = stagingFormat;
            texturesDesc.SampleDesc.Count = 1;
            texturesDesc.Usage = D3D11_USAGE_DEFAULT;
            texturesDesc.MiscFlags = 0;
            texturesDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            texturesDesc.CPUAccessFlags = 0;
            com_ptr<ID3D11Texture2D> videoBuffer;
            _hr = dxdevice->CreateTexture2D(&texturesDesc, NULL, &videoBuffer);
            if (FAILED(_hr))
                return error_inner;

            if(stagingFormat == DXGI_FORMAT_NV12)
            {
                D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourcesViewDescY = {};
                D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourcesViewDescUV = {};
                shaderResourcesViewDescY.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                shaderResourcesViewDescY.Format = DXGI_FORMAT_R8_UNORM;
                shaderResourcesViewDescY.Texture2D.MipLevels = 1;
                shaderResourcesViewDescY.Texture2D.MostDetailedMip = 0;
                shaderResourcesViewDescUV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                shaderResourcesViewDescUV.Format = DXGI_FORMAT_R8G8_UNORM;
                shaderResourcesViewDescUV.Texture2D.MipLevels = 1;
                shaderResourcesViewDescUV.Texture2D.MostDetailedMip = 0;

                com_ptr<ID3D11ShaderResourceView> resourceViewsY;
                com_ptr<ID3D11ShaderResourceView> resourceViewsUV;
                _hr = dxdevice->CreateShaderResourceView(videoBuffer.get(), &shaderResourcesViewDescY, &resourceViewsY);
                if (FAILED(_hr))
                    return error_inner;

                _hr = dxdevice->CreateShaderResourceView(videoBuffer.get(), &shaderResourcesViewDescUV, &resourceViewsUV);
                if (FAILED(_hr))
                    return error_inner;

                _videoBuffer = videoBuffer;
                _videoBufferViewY = resourceViewsY;
                _videoBufferViewUV = resourceViewsUV;
            }
            else
            {
                logger::err() << __FUNCTION__" not supported format " << stagingFormat;
                return error_not_supported;
            }
        }

        //-----------------------------

        if (stagingFormat != DXGI_FORMAT_UNKNOWN)
        {
            assert(_videoBuffer && _videoBufferViewY);
            if (!_videoBuffer || !_videoBufferViewY)
            {
                logger::err() << __FUNCTION__" null video buffer or video buffer Y SRV";
                return error_inner;
            }

            const AVPixFmtDescriptor * pixelFormatDesc = av_pix_fmt_desc_get(buffer.format.pixelFormat);
            if (!pixelFormatDesc)
                return error_inner;

            if (pixelFormatDesc->flags & AV_PIX_FMT_FLAG_HWACCEL)
            {
                logger::err() << __FUNCTION__" sws_getContext format not supported";
                _state = error_not_supported;
                return error_not_supported;
            }

            D3D11_TEXTURE2D_DESC dstDesc = { 0 };
            dstDesc.Width = buffer.format.width;
            dstDesc.Height = buffer.format.height;
            dstDesc.MipLevels = 1;
            dstDesc.ArraySize = 1;
            dstDesc.Format = DXGI_FORMAT_NV12;
            dstDesc.SampleDesc.Count = 1;
            dstDesc.Usage = D3D11_USAGE_STAGING;
            dstDesc.MiscFlags = 0;
            dstDesc.BindFlags = 0;
            dstDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            com_ptr<ID3D11Texture2D> videoBufferStaging;
            _hr = dxdevice->CreateTexture2D(&dstDesc, NULL, &videoBufferStaging);
            if (FAILED(_hr))
                return error_inner;

            D3D11_MAPPED_SUBRESOURCE dstResource = {};
            _hr = deviceContext->Map(videoBufferStaging.get(), 0, D3D11_MAP_WRITE, 0, &dstResource);
            if (FAILED(_hr))
                return error_inner;

            assert(_videoBufferViewUV);
            if (!_videoBufferViewUV)
            {
                logger::err() << __FUNCTION__" null video buffer or video buffer UV SRV";
                return error_inner;
            }

            if (!_sws)
            {
                SwsContext * ctx = sws_getContext(buffer.format.width, buffer.format.height, buffer.format.pixelFormat,
                    buffer.format.width, buffer.format.height, DXGIFormat2AvPixelFormat(stagingFormat), SWS_POINT, NULL, NULL, NULL);
                if (!ctx)
                {
                    logger::err() << __FUNCTION__" sws_getContext format not supported";
                    _state = error_not_supported;
                    return error_not_supported;
                }
                _sws.reset(ctx, [](SwsContext * ptr) { sws_freeContext(ptr); });
            }

            uint8_t * bitsY = (uint8_t *)dstResource.pData;
            uint8_t * bitsUV = (uint8_t *)dstResource.pData + dstResource.RowPitch * dstDesc.Height;
            uint8_t * dst[VideoBuffer::MAX_PLANE] = { bitsY, bitsUV };
            int32_t dst_linesize[VideoBuffer::MAX_PLANE] = { dstResource.RowPitch, dstResource.RowPitch };

            int height = sws_scale(_sws.get(), buffer.avframe->data, buffer.avframe->linesize, 0, buffer.format.height, dst, dst_linesize);
            assert(height == buffer.format.height);

            deviceContext->Unmap(videoBufferStaging.get(), 0);
            deviceContext->CopyResource(_videoBuffer.get(), videoBufferStaging.get());
        }
        else
        {
            if (buffer.avframe->format == AV_PIX_FMT_D3D11VA_VLD)
            {
            }
        }

        //----------------------------

        ID3D11RenderTargetView * rtvs[] = { _renderTargetView .get()};
        deviceContext->OMSetRenderTargets(1, rtvs, 0);

        float ClearColor[4] = {0, 0, 0, 0};
        deviceContext->ClearRenderTargetView(_renderTargetView.get(), ClearColor);

        D3D11_VIEWPORT viewport;
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width = backBufferDesc.BufferDesc.Width;
        viewport.Height = backBufferDesc.BufferDesc.Height;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        deviceContext->RSSetViewports(1, &viewport);

        D3D11_MAPPED_SUBRESOURCE MappedResource = {};
        deviceContext->Map(_renderParams.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
        RenderParams & renderParams = *(RenderParams *)MappedResource.pData;

        RECT rcDst = { 0, 0, windowW, windowH };
        double_t ratio = static_cast<double_t>(buffer.format.width) / buffer.format.height;
        double_t nWidth = windowH * ratio;
        double_t nHeight = windowW / ratio;
        if (nWidth > windowW)
        {
            rcDst.top = (windowH - nHeight) / 2;
            rcDst.bottom = rcDst.top + nHeight;
        }
        else
        {
            rcDst.left = (windowW - nWidth) / 2;
            rcDst.right = rcDst.left + nWidth;
        }

        DirectX::XMMATRIX xmMVP = DirectX::XMMatrixOrthographicOffCenterLH(
            1.0f / (rcDst.left / (float)windowW * 2.0f - 1.0f),
            1.0f / (rcDst.right / (float)windowW * 2.0f - 1.0f),
            1.0f / (-rcDst.bottom / (float)windowH * 2.0f + 1.0f),
            1.0f / (-rcDst.top / (float)windowH * 2.0f + 1.0f),
            0.01f, 1.0f);
        //xmMVP = DirectX::XMMatrixIdentity();
        memcpy(renderParams.matrMVP.data(), &xmMVP, sizeof(xmMVP));
        //buffer.format.colorSpace = AVCOL_SPC_BT709;
        if (buffer.format.colorSpace == AVCOL_SPC_RGB)
        {
            renderParams.matrYUV2RGB = { 1.164384f, 1.164384f, 1.164384f, 0.000000f,
                0.000000f, -0.391762f, 2.017232f, 0.000000f,
                1.596027f, -0.812968f, 0.000000f, 0.000000f,
                -0.874202f, 0.531668f, -1.085631f, 1.000000f };
        }
        else if(buffer.format.colorSpace == AVCOL_SPC_BT470BG || buffer.format.colorSpace == AVCOL_SPC_SMPTE170M)
        {
            renderParams.matrYUV2RGB = { 1.000000f, 1.000000f, 1.000000f, 0.000000f,
                0.000000f, -0.345491f, 1.778976f, 0.000000f,
                1.407520f, -0.716948f, 0.000000f, 0.000000f,
                -0.706520f, 0.533303f, -0.892976f, 1.000000f };
        }
        else if (buffer.format.colorSpace == AVCOL_SPC_BT709)
        {
            renderParams.matrYUV2RGB = { 1.164384f, 1.164384f, 1.164384f, 0.000000f,
                0.000000f, -0.213249f, 2.112402f, 0.000000f,
                1.792741f, -0.532909f, 0.000000f, 0.000000f,
                -0.972945f, 0.301483f, -1.133402f, 1.000000f };
        }
        else
        {
            renderParams.matrYUV2RGB = { 1.164384f, 1.164384f, 1.164384f, 0.000000f,
                0.000000f, -0.391762f, 2.017232f, 0.000000f,
                1.596027f, -0.812968f, 0.000000f, 0.000000f,
                -0.874202f, 0.531668f, -1.085631f, 1.000000f };
        }
        if (buffer.avframe)
            renderParams.textureOffset = { (float)buffer.format.width / videoFormat.width, (float)buffer.format.height / videoFormat.height };
        else
            renderParams.textureOffset = { 1, 1 };

        deviceContext->Unmap(_renderParams.get(), 0);

        ID3D11Buffer * buffers[] = { _renderParams.get() };
        deviceContext->VSSetConstantBuffers(0, 1, buffers);
        deviceContext->PSSetConstantBuffers(0, 1, buffers);

        deviceContext->IASetInputLayout(_inputLayout.get());
        deviceContext->VSSetShader(_vertexShader.get(), NULL, 0);
        deviceContext->PSSetShader(_pixelShader.get(), NULL, 0);

        ID3D11SamplerState * sss[] = { _samplerState.get() };
        deviceContext->PSSetSamplers(0, 1, sss);

        if (stagingFormat == DXGI_FORMAT_UNKNOWN)
        {
            com_ptr<ID3D11Device> videoDevcie;
            videoViewY->GetDevice(&videoDevcie);
            assert(videoDevcie == dxdevice);
            ID3D11ShaderResourceView * pssrs[] = { videoViewY, videoViewUV };
            deviceContext->PSSetShaderResources(0, 2, pssrs);
        }
        else if (_videoBufferViewY)
        {
            ID3D11ShaderResourceView * pssrs[] = { _videoBufferViewY.get(), _videoBufferViewUV.get() };
            deviceContext->PSSetShaderResources(0, 2, pssrs);
        }

        UINT stride = sizeof(VertexT);
        UINT offset = 0;
        //设置数据信息格式控制信息
        deviceContext->IASetInputLayout(_inputLayout.get());
        //设置要绘制的几何体信息
        deviceContext->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
        //指明如何绘制三角形
        deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        deviceContext->Draw(6, 0);

        assert(SUCCEEDED(_hr) || _hr == DXGI_ERROR_DEVICE_REMOVED);
        if (FAILED(_hr))
            return _hr == DXGI_ERROR_DEVICE_REMOVED ? error_ok : error_inner;

        return error_ok;
    }

    error_e D3D11VideoPlayer::present()
    {
        //std::lock_guard<std::mutex> lock(_device->GetMutex());
        if(_swapChain)
        {
            _hr = _swapChain->Present(0, 0);
            if (FAILED(_hr))
                return _hr == DXGI_ERROR_DEVICE_REMOVED ? error_ok : error_inner;
        }
        return error_ok;
    }

    common::error_e D3D11VideoPlayer::session(int session)
    {
        _sws.reset();
        return error_ok;
    }

    common::error_e D3D11VideoPlayer::pausing()
    {
        return present();
    }

    error_e D3D11VideoPlayer::resetSwapChain(int32_t width, int32_t height)
    {
        if (!_window)
            return error_state;

        com_ptr<ID3D11Device> device = _device->GetPointer();
        if (!device)
            return error_inner;

        DXGI_SWAP_CHAIN_DESC scd = {};
        scd.Windowed = TRUE;
        scd.BufferDesc.Width = width;
        scd.BufferDesc.Height = height;
        scd.OutputWindow = (HWND)_window->GetHandle();
        scd.SampleDesc.Count = 1;
        scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scd.BufferCount = 1;
        com_ptr<IDXGISwapChain> swapChain;
        HRESULT hr = _device->GetDXGI()->GetPointer()->CreateSwapChain(_device->GetPointer().get(), &scd, &swapChain);
        assert(SUCCEEDED(hr));
        if (FAILED(hr))
            return error_inner;

        com_ptr<ID3D11Texture2D> buffer;
        hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&buffer);

        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.Format = scd.BufferDesc.Format;
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        rtvDesc.Texture2D.MipSlice = 0;
        com_ptr<ID3D11RenderTargetView> renderTargetView;
        hr = device->CreateRenderTargetView(buffer.get(), &rtvDesc, &renderTargetView);
        if (FAILED(hr))
            return error_inner;

        _swapChain = swapChain;
        _renderTargetView = renderTargetView;
        return error_ok;
    }

    error_e D3D11VideoPlayer::initRender()
    {
        if (!_window)
            return error_state;

        com_ptr<ID3D11Device> device = _device->GetPointer();
        if (!device)
            return error_inner;

        HWND hWnd = (HWND)_window->GetHandle();
        RECT rcClient;
        GetClientRect(hWnd, &rcClient);
        int32_t windowW = std::max(rcClient.right - rcClient.left, 16L);
        int32_t windowH = std::max(rcClient.bottom - rcClient.top, 16L);

        error_e error = resetSwapChain(windowW, windowH);
        if (error < 0)
            return error;


        com_ptr<ID3D11VertexShader> vertexShader;
        com_ptr<ID3D11PixelShader> pixelShader;

        auto [vsBuffer, nbvsBuffer] = load_res(IDR_SHADER_VS, res_file);
        auto [psBuffer, nbpsBuffer] = load_res(IDR_SHADER_PS, res_file);

        _hr = device->CreateVertexShader(vsBuffer.get(), nbvsBuffer, NULL, &vertexShader);
        if(FAILED(_hr))
        {
            logger::err() << __FUNCTION__" device->CreateVertexShader " << winerr_str(_hr);
            return error_inner;
        }
        _hr = device->CreatePixelShader(psBuffer.get(), nbpsBuffer, NULL, &pixelShader);
        if (FAILED(_hr))
        {
            logger::err() << __FUNCTION__" device->CreatePixelShader " << winerr_str(_hr);
            return error_inner;
        }

        com_ptr<ID3D11InputLayout> inputLayout;
        const D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        _hr = device->CreateInputLayout(layout, (uint32_t)std::size(layout), vsBuffer.get(), nbvsBuffer, &inputLayout);
        if (FAILED(_hr))
        {
            logger::err() << __FUNCTION__" device->CreateInputLayout " << winerr_str(_hr);
            return error_inner;
        }

        com_ptr<ID3D11SamplerState> samplerState;
        D3D11_SAMPLER_DESC samDesc = {};
        samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samDesc.AddressU = samDesc.AddressV = samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samDesc.MaxAnisotropy = 1;
        samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        samDesc.MaxLOD = D3D11_FLOAT32_MAX;
        _hr = device->CreateSamplerState(&samDesc, &samplerState);
        if (FAILED(_hr))
        {
            logger::err() << __FUNCTION__" device->CreateSamplerState " << winerr_str(_hr);
            return error_inner;
        }

        com_ptr<ID3D11Buffer> renderParams;
        D3D11_BUFFER_DESC cbDesc = {};
        cbDesc.Usage = D3D11_USAGE_DYNAMIC;
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        cbDesc.ByteWidth = sizeof(RenderParams);
        _hr = device->CreateBuffer(&cbDesc, NULL, &renderParams);
        if (FAILED(_hr))
        {
            logger::err() << __FUNCTION__" device->CreateBuffer for render params" << winerr_str(_hr);
            return error_inner;
        }

        com_ptr<ID3D11Buffer> vertexBuffer;
        VertexT vertices[] =
        {
            {{-1, 1, 1}, {0.0f, 0.0f}},
            {{1, 1, 1}, {1.0f, 0.0f}},
            {{1, -1, 1}, {1.0f, 1.0f}},
            {{1, -1, 1}, {1.0f, 1.0f}},
            {{-1, -1, 1}, {0.0f, 1.0f}},
            {{-1, 1, 1}, {0.0f, 0.0f}},
        };

        D3D11_BUFFER_DESC vertexDesc;
        ZeroMemory(&vertexDesc, sizeof(vertexDesc));
        vertexDesc.Usage = D3D11_USAGE_DEFAULT;
        vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexDesc.ByteWidth = sizeof(VertexT) * std::size(vertices);

        D3D11_SUBRESOURCE_DATA resourceData;
        ZeroMemory(&resourceData, sizeof(resourceData));
        resourceData.pSysMem = vertices;
        _hr = device->CreateBuffer(&vertexDesc, &resourceData, &vertexBuffer);
        if (FAILED(_hr))
        {
            logger::err() << __FUNCTION__" device->CreateBuffer for vertices" << winerr_str(_hr);
            return error_inner;
        }

        _vertexShader = vertexShader;
        _pixelShader = pixelShader;
        _inputLayout = inputLayout;
        _samplerState = samplerState;
        _renderParams = renderParams;
        _vertexBuffer = vertexBuffer;

        logger::act() << __FUNCTION__" ok";
        return error_ok;
    }
}}
