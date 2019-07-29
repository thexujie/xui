#pragma once

#include "d3d.h"
#include "win32/com_ptr.h"
#include "win32/dll.h"

struct IDXGIFactory;
struct IDXGIAdapter;

namespace win32 {
    namespace d3d {

        class DXGIFactory
        {
        public:
            DXGIFactory();
            ~DXGIFactory();

            com_ptr<IDXGIFactory> GetPointer() const;
            std::vector<D3DDeviceDesc> GetDeviceDescs() const;

            com_ptr<IDXGIAdapter> GetAdapter(deviceid_t deviceId);
        private:
            common::error_e load();
        private:
            com_ptr<IDXGIFactory> _dxgi;
            win32::dll _dll;
        };
    }
}