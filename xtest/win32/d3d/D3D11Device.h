#pragma once

#include "d3d.h"
#include "win32/com_ptr.h"
#include "DXGIFactory.h"

struct ID3D11Device;

namespace win32 {
    namespace d3d {

        struct D3D11DeviceDesc
        {
            uint32_t adapter = 0;
            deviceid_t deviceId = 0;
            u8string description;
            uint32_t vendorId = 0;
            uint32_t subSysId = 0;
            uint32_t revision = 0;
        };

        class D3D11Device : public player::IVideoDecoderHWAccelDevice, public std::enable_shared_from_this<D3D11Device>
        {
        public:
            D3D11Device(std::shared_ptr<DXGIFactory> dxgi, deviceid_t deviceId);
            ~D3D11Device();

            D3DDeviceDesc GetDesc() const;
            std::shared_ptr<DXGIFactory> GetDXGI() const { return _dxgi; }
            com_ptr<ID3D11Device> GetPointer() const;
        public:
            player::CodecDeviceDesc GetCodecDeviceDesc() const;
            std::map<AVCodecID, std::vector<player::CodecDesc>> GetCodecDescs() const;
            std::vector<player::CodecDesc> GetCodecDescs(AVCodecID codecId) const;
            std::vector<player::CodecDesc> GetCodecDescs(player::VideoCodecFormat codecFormat) const;
            std::shared_ptr<player::IVideoDecoderHWAccelerator> CreateAccelerator(player::VideoCodecFormat codecFormat);

        private:
            common::error_e initDevice();
            bool testCodec(const GUID & dxvaCodecGuid, player::VideoCodecFormat format) const;
            player::CodecLevel testCodecLevel(const GUID & dxvaCodecGuid) const;

        private:
            std::mutex _mtx;
            u8string _description;
            std::shared_ptr<DXGIFactory> _dxgi;
            deviceid_t _deviceId = 0;
            com_ptr<ID3D11Device> _device;
            win32::dll _dll;
        };
    }
}