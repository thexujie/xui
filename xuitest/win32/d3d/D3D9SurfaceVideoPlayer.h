#pragma once

#include "player/media.h"
#include "player/VideoPlayer.h"
#include "D3D9Factory.h"
#include "D3D9Device.h"

namespace win32 { namespace d3d
{
    class D3D9SurfaceVideoPlayer : public player::VideoPlayer
    {
    public:
        enum flag_e
        {
            flag_fallback_on_different_device = 0x00010000,
        };

    public:
        D3D9SurfaceVideoPlayer(std::shared_ptr<D3D9Device> d3d9Device);
        ~D3D9SurfaceVideoPlayer();
    private:
        void videoThread();
        error_e render(player::VideoBuffer & buffer);
        common::error_e present();
        common::error_e session(int session);
        common::error_e pendding();
        common::error_e pausing();

    private:
        std::shared_ptr<D3D9Device> _device;
        com_ptr<struct IDirect3DSurface9> _d3d9Surface;
        int32_t _hr = 0;
        double_t _tsLastReset = std::nan(0);
        std::shared_ptr<SwsContext> _sws;
        com_ptr<struct IDirect3DSurface9> _pauseSurface;
    };
}
}
