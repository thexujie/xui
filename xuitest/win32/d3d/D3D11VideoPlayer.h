#pragma once

#include "player/media.h"
#include "player/VideoPlayer.h"
#include "D3D11Device.h"

struct IDXGISwapChain;
struct ID3D11RenderTargetView;

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11SamplerState;
struct ID3D11Buffer;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;

namespace win32 { namespace d3d
{
    class D3D11VideoPlayer : public player::VideoPlayer
    {
    public:
        D3D11VideoPlayer(std::shared_ptr<D3D11Device> device, std::shared_ptr<player::IVideoRenderWindow> window);
        D3D11VideoPlayer(std::shared_ptr<DXGIFactory> dxgi, deviceid_t deviceId, std::shared_ptr<player::IVideoRenderWindow> window);
        ~D3D11VideoPlayer();

        std::shared_ptr<D3D11Device> GetDevice() const { return _device; }
        player::VideoFormat GetDeviceVideoFormat() const;
    private:
        void videoThread();
        common::error_e render(player::VideoBuffer & buffer);
        common::error_e present();
        common::error_e session(int session);
        virtual common::error_e pausing();

    private:
        common::error_e resetSwapChain(int32_t width, int32_t height);
        common::error_e initRender();

    private:
        std::shared_ptr<player::IVideoRenderWindow> _window;
        std::shared_ptr<D3D11Device> _device;
        com_ptr<IDXGISwapChain> _swapChain;
        com_ptr<ID3D11RenderTargetView> _renderTargetView;
        int32_t _hr = 0;
        double_t _tsLastReset = std::nan(0);

        com_ptr<ID3D11VertexShader> _vertexShader;
        com_ptr<ID3D11PixelShader> _pixelShader;
        com_ptr<ID3D11InputLayout> _inputLayout;
        com_ptr<ID3D11SamplerState> _samplerState;

        com_ptr<ID3D11Buffer> _renderParams;
        com_ptr<ID3D11Buffer> _vertexBuffer;

        com_ptr<ID3D11Texture2D> _videoBuffer;
        com_ptr<ID3D11ShaderResourceView> _videoBufferViewY;
        com_ptr<ID3D11ShaderResourceView> _videoBufferViewUV;

        std::shared_ptr<SwsContext> _sws;
    };
}}
