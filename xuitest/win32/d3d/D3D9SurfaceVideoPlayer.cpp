#include "stdafx.h"
#include "D3D9SurfaceVideoPlayer.h"

#include "win32/windows.h"
#include "d3d.h"
#include "win32/windows.h"
#include <d3d9.h>
#include <dxva2api.h>

namespace win32 { namespace d3d
{
    D3D9SurfaceVideoPlayer::D3D9SurfaceVideoPlayer(std::shared_ptr<D3D9Device> d3d9Device) : _device(d3d9Device) { }

    D3D9SurfaceVideoPlayer::~D3D9SurfaceVideoPlayer()
    {
        if (_thPlay.joinable())
        {
            _flags = _flags | common::flag_stop;
            _cond.notify_all();
            _thPlay.join();
        }
    }

    void D3D9SurfaceVideoPlayer::videoThread()
    {
        if (!_device)
        {
            _state = error_state;
            return;
        }

        VideoPlayer::videoThread();
    }

    error_e D3D9SurfaceVideoPlayer::render(player::VideoBuffer & buffer)
    {
        if (!_device)
            return error_state;

        HWND hWnd = (HWND)_device->GetWindowHandle();
        RECT rcClient;
        GetClientRect(hWnd, &rcClient);
        int32_t width = std::max(rcClient.right - rcClient.left, 16L);
        int32_t height = std::max(rcClient.bottom - rcClient.top, 16L);

        player::VideoFormat deviceFormat = _device->GetDeviceVideoFormat();
        double_t tsCurr = get_time_hr();
        if ((_hr != D3DERR_DEVICELOST) && (width != deviceFormat.width || height != deviceFormat.height))
        {
            if (std::isnan(_tsLastReset))
                _tsLastReset = tsCurr;

            if (tsCurr - _tsLastReset > 1)
            {
                _hr = D3DERR_DEVICELOST;
            }
        }

        if (_hr == D3DERR_DEVICELOST || _hr == D3DERR_DEVICEREMOVED)
        {
            _d3d9Surface.reset();
            error_e err_reset = _device->ResetDevice();
            if (err_reset == error_ok)
            {
                _hr = S_OK;
                _tsLastReset = std::nan(0);
            }
            else
                return error_ok;
        }

        com_ptr<IDirect3DDevice9> renderDevice = _device->GetPointer();
        if (!renderDevice)
            return error_ok;

        if(!buffer.avframe)
        {
            // 渲染
            _hr = renderDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
            assert(SUCCEEDED(_hr) || _hr == D3DERR_DEVICELOST);
            if (FAILED(_hr))
                return _hr == D3DERR_DEVICELOST ? error_ok : error_inner;

            _hr = renderDevice->BeginScene();
            assert(SUCCEEDED(_hr) || _hr == D3DERR_DEVICELOST);
            if (FAILED(_hr))
                return _hr == D3DERR_DEVICELOST ? error_ok : error_inner;

            _hr = renderDevice->EndScene();
            assert(SUCCEEDED(_hr) || _hr == D3DERR_DEVICELOST);
            if (FAILED(_hr))
                return _hr == D3DERR_DEVICELOST ? error_ok : error_inner;
            return error_ok;
        }

        // 传输内容
        com_ptr<struct IDirect3DSurface9> dataSurface;
        D3DFORMAT d3dfmtOfBuffer = D3DFMT_UNKNOWN;
        switch(buffer.avframe->format)
        {
        case AV_PIX_FMT_RGB0:
            d3dfmtOfBuffer = D3DFMT_X8B8G8R8;
            break;
        case AV_PIX_FMT_BGR0:
            d3dfmtOfBuffer = D3DFMT_X8R8G8B8;
            break;
        case AV_PIX_FMT_NV12:
            d3dfmtOfBuffer = (D3DFORMAT)D3DFormatExNV12;
            break;
        case AV_PIX_FMT_YUV420P:
            d3dfmtOfBuffer = (D3DFORMAT)D3DFormatExNV12;
            //d3dfmtOfBuffer = (D3DFORMAT)D3DFormatExIYUV;
            break;
        case AV_PIX_FMT_DXVA2_VLD:
            {
                com_ptr<IDirect3DSurface9> surface(reinterpret_cast<IDirect3DSurface9 *>(buffer.avframe->data[3]));
                if (surface)
                    surface->AddRef();

                com_ptr<IDirect3DDevice9> dataSurfaceDevice;
                _hr = surface->GetDevice(&dataSurfaceDevice);
                assert(SUCCEEDED(_hr) || _hr == D3DERR_DEVICELOST);
                if (FAILED(_hr))
                    return _hr == D3DERR_DEVICELOST ? error_ok : error_inner;

                if (dataSurfaceDevice == renderDevice)
                {
                    d3dfmtOfBuffer = D3DFMT_UNKNOWN;
                    dataSurface = surface;
                }
                //不做这个处理了，效率低，效果也不怎么好，不如黑屏， return error_ok; 更好
                else if (!(_flags & flag_fallback_on_different_device))
                    d3dfmtOfBuffer = (D3DFORMAT)D3DFormatExNV12;
                else
                    return error_ok;
            }
            break;
        default:
            d3dfmtOfBuffer = D3DFMT_UNKNOWN;
            break;
        }

        //创建 Surface
        if (_d3d9Surface)
        {
            D3DSURFACE_DESC surfaceDesc = {};
            _d3d9Surface->GetDesc(&surfaceDesc);
            if (surfaceDesc.Width != buffer.format.width || surfaceDesc.Height != buffer.format.height ||
                d3dfmtOfBuffer != surfaceDesc.Format)
                _d3d9Surface.reset();
        }

        if (!_d3d9Surface && d3dfmtOfBuffer != D3DFMT_UNKNOWN)
        {
            _hr = renderDevice->CreateOffscreenPlainSurface(
                buffer.format.width, buffer.format.height,
                d3dfmtOfBuffer,
                D3DPOOL_DEFAULT,
                &_d3d9Surface,
                NULL);
            if (FAILED(_hr))
                return _hr == D3DERR_DEVICELOST ? error_ok : error_inner;
        }

        assert(_d3d9Surface || dataSurface);
        if (!_d3d9Surface && !dataSurface)
            return error_ok;

        if(dataSurface)
        {
            // 使用 StretchRect
        }
        else
        {
            const AVPixFmtDescriptor * pixelFormatDesc = av_pix_fmt_desc_get(buffer.format.pixelFormat);
            if (!pixelFormatDesc)
                return error_inner;

            if (pixelFormatDesc->flags & AV_PIX_FMT_FLAG_HWACCEL)
            {
                //logger::err() << __FUNCTION__" sws_getContext format not supported";
                //_state = error_not_supported;
                //return error_not_supported;
                return error_ok;
            }

            D3DSURFACE_DESC dstDesc = {};
            if (_d3d9Surface)
                _d3d9Surface->GetDesc(&dstDesc);

            D3DLOCKED_RECT dstResource = {};
            _hr = _d3d9Surface->LockRect(&dstResource, NULL, D3DLOCK_DISCARD);
            assert(SUCCEEDED(_hr) || _hr == D3DERR_DEVICELOST);
            if (FAILED(_hr))
                return _hr == D3DERR_DEVICELOST ? error_ok : error_inner;

            if (!_sws)
            {
                SwsContext * ctx = sws_getContext(buffer.format.width, buffer.format.height, buffer.format.pixelFormat,
                    buffer.format.width, buffer.format.height, D3DFormat2AvPixelFormat(d3dfmtOfBuffer), SWS_POINT, NULL, NULL, NULL);
                if (!ctx)
                {
                    logger::err() << __FUNCTION__" sws_getContext format not supported";
                    _state = error_not_supported;
                    return error_not_supported;
                }
                _sws.reset(ctx, [](SwsContext * ptr) { sws_freeContext(ptr); });
            }

            uint8_t * dst[player::VideoBuffer::MAX_PLANE] = { };
            int32_t dst_linesize[player::VideoBuffer::MAX_PLANE] = { dstResource.Pitch, dstResource.Pitch };

            if(d3dfmtOfBuffer == D3DFormatExNV12)
            {
                uint8_t * bitsY = (uint8_t *)dstResource.pBits;
                uint8_t * bitsUV = (uint8_t *)dstResource.pBits + dstResource.Pitch * dstDesc.Height;
                dst[0] = bitsY;
                dst[1] = bitsUV;
            }
            else
            {
                assert(false);
                return error_inner;
            }
            int height = sws_scale(_sws.get(), buffer.avframe->data, buffer.avframe->linesize, 0, buffer.format.height, dst, dst_linesize);
            assert(height == buffer.format.height);

            _hr = _d3d9Surface->UnlockRect();
            assert(SUCCEEDED(_hr) || _hr == D3DERR_DEVICELOST);
            if (FAILED(_hr))
                return _hr == D3DERR_DEVICELOST ? error_ok : error_inner;
        }

        // 渲染
        _hr = renderDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
        assert(SUCCEEDED(_hr) || _hr == D3DERR_DEVICELOST);
        if (FAILED(_hr))
            return _hr == D3DERR_DEVICELOST ? error_ok : error_inner;

        _hr = renderDevice->BeginScene();
        assert(SUCCEEDED(_hr) || _hr == D3DERR_DEVICELOST);
        if (FAILED(_hr))
            return _hr == D3DERR_DEVICELOST ? error_ok : error_inner;

        com_ptr<IDirect3DSurface9> backBuffer;
        _hr = renderDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);
        assert(SUCCEEDED(_hr) || _hr == D3DERR_DEVICELOST);
        if (FAILED(_hr))
            return _hr == D3DERR_DEVICELOST ? error_ok : error_inner;

        D3DSURFACE_DESC backBufferDesc = {};
        backBuffer->GetDesc(&backBufferDesc);

        RECT rcSrc = { 0, 0, (int32_t)buffer.format.width, (int32_t)buffer.format.height };
        RECT rcDst = { 0, 0, (int32_t)backBufferDesc.Width, (int32_t)backBufferDesc.Height };
        double_t ratio = static_cast<double_t>(buffer.format.width) / buffer.format.height;
        double_t nWidth = backBufferDesc.Height * ratio;
        double_t nHeight = backBufferDesc.Width / ratio;
        if(nWidth > backBufferDesc.Width)
        {
            rcDst.top = (int32_t)((backBufferDesc.Height - nHeight) / 2);
            rcDst.bottom = (int32_t)(rcDst.top + nHeight);
        }
        else
        {
            rcDst.left = (int32_t)((backBufferDesc.Width - nWidth) / 2);
            rcDst.right = (int32_t)(rcDst.left + nWidth);
        }

#ifdef _DEBUG
        com_ptr<IDirect3DDevice9> _device;
        _hr = (dataSurface ? dataSurface.get() : _d3d9Surface.get())->GetDevice(&_device);
        assert(SUCCEEDED(_hr) || _hr == D3DERR_DEVICELOST);
        if (FAILED(_hr))
            return _hr == D3DERR_DEVICELOST ? error_ok : error_inner;

        assert(_device == renderDevice);
#endif
        _hr = renderDevice->StretchRect(dataSurface ? dataSurface.get() : _d3d9Surface.get(), &rcSrc, backBuffer.get(), &rcDst, D3DTEXF_LINEAR);
        assert(SUCCEEDED(_hr) || _hr == D3DERR_DEVICELOST);
        if (FAILED(_hr))
            return _hr == D3DERR_DEVICELOST ? error_ok : error_inner;


        _hr = renderDevice->EndScene();
        assert(SUCCEEDED(_hr) || _hr == D3DERR_DEVICELOST);
        if (FAILED(_hr))
            return _hr == D3DERR_DEVICELOST ? error_ok : error_inner;

        return error_ok;
    }

    error_e D3D9SurfaceVideoPlayer::present()
    {
        com_ptr<IDirect3DDevice9> renderDevice = _device->GetPointer();
        if (renderDevice && SUCCEEDED(_hr))
        {
            _hr = renderDevice->Present(NULL, NULL, NULL, NULL);
            if(_hr == D3DERR_INVALIDCALL)
            {
                logger::inf() << __FUNCTION__" invalid call happened!";
                _hr = D3DERR_DEVICELOST;
                return error_ok;
            }

            assert(SUCCEEDED(_hr) || _hr == D3DERR_DEVICELOST || _hr == D3DERR_DEVICEREMOVED);
            if (FAILED(_hr))
                return (_hr == D3DERR_DEVICELOST || _hr == D3DERR_DEVICEREMOVED) ? error_ok : error_inner;
        }
        return error_ok;
    }

    common::error_e D3D9SurfaceVideoPlayer::session(int session)
    {
        _sws.reset();
        return error_ok;
    }

    common::error_e D3D9SurfaceVideoPlayer::pendding()
    {
        return error_ok;
    }

    common::error_e D3D9SurfaceVideoPlayer::pausing()
    {
        present();
        return error_ok;
    }
}}
