#pragma once
#include "player/media.h"
#include "D3D9Factory.h"

struct IDirect3DDeviceManager9;
struct IDirectXVideoDecoderService;
struct IDirectXVideoDecoder;

namespace win32
{
    namespace d3d
    {
        class D3D9Device : public player::IVideoDecoderHWAccelDevice
        {
        public:
            D3D9Device(std::shared_ptr<D3D9Factory> d3d9, std::shared_ptr<player::IVideoRenderWindow> window);
            ~D3D9Device();

            D3D9DeviceDesc GetDesc() const;
            com_ptr<IDirect3DDevice9> GetPointer() const;
            com_ptr<IDirectXVideoDecoderService> GetDecoderServicePointer() const;

            player::VideoFormat GetDeviceVideoFormat() const;
            void * GetWindowHandle() const;

            error_e ResetDevice();

            player::CodecDeviceDesc GetCodecDeviceDesc() const;
            std::map<AVCodecID, std::vector<player::CodecDesc>> GetCodecDescs() const;
            std::vector<player::CodecDesc> GetCodecDescs(AVCodecID codecId) const;
            std::vector<player::CodecDesc> GetCodecDescs(player::VideoCodecFormat codecFormat) const;
            std::shared_ptr<player::IVideoDecoderHWAccelerator> CreateAccelerator(player::VideoCodecFormat codecFormat);

        private:
            error_e initDevice();
            error_e initDecoderService();
            bool testCodec(const GUID & dxvaCodecGuid, player::VideoCodecFormat format) const;
            player::CodecLevel testCodecLevel(const GUID & dxvaCodecGuid) const;

        private:
            std::shared_ptr<D3D9Factory> _d3d9;
            std::shared_ptr<player::IVideoRenderWindow> _window;
            com_ptr<IDirect3DDevice9> _d3d9Device;
            player::VideoFormat _deviceFormat;

            std::mutex _mtx;
            //decoder
            uint32_t _resetToken = 0;
            com_ptr<IDirect3DDeviceManager9> _d3d9DeviceManager;
            com_ptr<IDirectXVideoDecoderService> _decoderService;
            void * _hDevice = NULL;
        };

    }
}
