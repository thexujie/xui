#include "stdafx.h"
#include "D3D9Device.h"
#include "d3d.h"
#include "win32/windows.h"
#include <d3d9.h>
#include <dxva2api.h>
#include <libavutil/hwcontext_dxva2.h>

namespace win32
{
    namespace d3d
    {
        using namespace common;
        using namespace player;

        D3D9Device::D3D9Device(std::shared_ptr<D3D9Factory> d3d9,
            std::shared_ptr<IVideoRenderWindow> window) : _d3d9(d3d9), _window(window) { }

        D3D9Device::~D3D9Device() { }

        D3D9DeviceDesc D3D9Device::GetDesc() const
        {
            if (!_d3d9)
                return {};

            com_ptr<IDirect3DDevice9> d3d9Device = GetPointer();
            if (!d3d9Device)
                return {};

            HRESULT hr = S_OK;
            com_ptr<struct IDirect3D9> d3d9 = _d3d9->GetD3D9();

            D3DDEVICE_CREATION_PARAMETERS createParam = {};
            hr = d3d9Device->GetCreationParameters(&createParam);
            if (FAILED(hr))
                return {};

            D3DDISPLAYMODE displayMode = {};
            hr = d3d9Device->GetDisplayMode(0, &displayMode);
            if (FAILED(hr))
                return {};

            D3DADAPTER_IDENTIFIER9 adapterIdentifier = {};
            hr = d3d9->GetAdapterIdentifier(createParam.AdapterOrdinal, 0, &adapterIdentifier);
            if (FAILED(hr))
                return {};

            com_ptr<IDirect3DSurface9> backBuffer0;
            hr = d3d9Device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer0);
            if (FAILED(hr))
                return {};

            D3DCAPS9 caps = {};
            hr = d3d9->GetDeviceCaps(createParam.AdapterOrdinal, D3DDEVTYPE_HAL, &caps);
            if (FAILED(hr))
                return {};

            D3DSURFACE_DESC backBufferDesc = {};
            backBuffer0->GetDesc(&backBufferDesc);

            D3D9DeviceDesc desc = {};

            desc.adapter = createParam.AdapterOrdinal;
            desc.deviceId = adapterIdentifier.DeviceId;
            std::memcpy(&desc.deviceIdentifier, &adapterIdentifier.DeviceIdentifier,
                std::min(std::size(desc.deviceIdentifier), sizeof(GUID)));

            desc.driver = adapterIdentifier.Driver;
            desc.application = "Direct3D 9";
            desc.description = adapterIdentifier.Description;
            desc.displayDeviceName = adapterIdentifier.DeviceName;
            desc.driverVersion = adapterIdentifier.DriverVersion.QuadPart;
            desc.vendorId = adapterIdentifier.VendorId;
            desc.subSysId = adapterIdentifier.SubSysId;
            desc.revision = adapterIdentifier.Revision;
            desc.WHQLLevel = adapterIdentifier.WHQLLevel;

            desc.displayWidth = displayMode.Width;
            desc.displayHeight = displayMode.Height;

            desc.backBufferWidth = backBufferDesc.Width;
            desc.backBufferHeight = backBufferDesc.Height;

            return desc;
        }

        com_ptr<IDirect3DDevice9> D3D9Device::GetPointer() const
        {
            std::lock_guard<std::mutex> lock(const_cast<D3D9Device *>(this)->_mtx);
            if (_d3d9Device)
                return _d3d9Device;

            const_cast<D3D9Device *>(this)->initDevice();
            return _d3d9Device;
        }

        com_ptr<IDirectXVideoDecoderService> D3D9Device::GetDecoderServicePointer() const
        {
            com_ptr<IDirect3DDevice9> d3d9Device = GetPointer();
            if (!d3d9Device)
                return nullptr;

            std::lock_guard<std::mutex> lock(const_cast<D3D9Device *>(this)->_mtx);
            if (_decoderService)
                return _decoderService;

            const_cast<D3D9Device *>(this)->initDevice();
            return _decoderService;
        }

        CodecDeviceDesc D3D9Device::GetCodecDeviceDesc() const
        {
            D3D9DeviceDesc d3d9Desc = GetDesc();
            CodecDeviceDesc desc = {};
            desc.application = d3d9Desc.application;
            desc.description = d3d9Desc.description;
            desc.type = AV_HWDEVICE_TYPE_DXVA2;
            desc.deviceId = d3d9Desc.deviceId;
            desc.vendorId = d3d9Desc.vendorId;
            desc.subSysId = d3d9Desc.subSysId;
            return desc;
        }

        VideoFormat D3D9Device::GetDeviceVideoFormat() const
        {
            com_ptr<IDirect3DDevice9> d3d9Device = GetPointer();
            if (!d3d9Device)
                return {};
            return _deviceFormat;
        }

        void * D3D9Device::GetWindowHandle() const
        {
            return _window ? _window->GetHandle() : nullptr;
        }

        error_e D3D9Device::ResetDevice()
        {
            HRESULT hr = S_OK;
            if (!_d3d9 || !_window)
                return error_state;

            std::lock_guard<std::mutex> lock(_mtx);
            _d3d9Device = nullptr;
            error_e state = initDevice();
            if (state < 0)
                return state;

            if (_d3d9DeviceManager)
            {
                hr = _d3d9DeviceManager->ResetDevice(_d3d9Device.get(), _resetToken);
                if (FAILED(hr))
                    return error_inner;
            }

            return error_ok;
        }

        std::map<AVCodecID, std::vector<CodecDesc>> D3D9Device::GetCodecDescs() const
        {
            com_ptr<IDirectXVideoDecoderService> decoderService = GetDecoderServicePointer();
            if (!decoderService)
                return {};

            HRESULT hr = S_OK;
            uint32_t numDecoderGuids = 0;
            GUID * decoderGuids = nullptr;
            hr = decoderService->GetDecoderDeviceGuids(&numDecoderGuids, &decoderGuids);
            if (FAILED(hr))
                return {};
            std::shared_ptr<GUID>(decoderGuids, [](GUID * guids) { CoTaskMemFree(guids); });

            std::map<AVCodecID, std::vector<CodecDesc>> descs;
            for (uint32_t idecoder = 0; idecoder < numDecoderGuids; ++idecoder)
            {
                const GUID & dxvaCodecGuid = decoderGuids[idecoder];
                AVCodecID dxvaCodecId = MapDXVA2CodecID(dxvaCodecGuid);

                uint32_t numDxvaOutputFormats = 0;
                D3DFORMAT * dxvaOutputFormats = nullptr;
                hr = decoderService->GetDecoderRenderTargets(dxvaCodecGuid, &numDxvaOutputFormats, &dxvaOutputFormats);
                if (FAILED(hr))
                    continue;
                std::shared_ptr<D3DFORMAT> __dxvaOutputFormats(dxvaOutputFormats, [](D3DFORMAT * ptr) { CoTaskMemFree(ptr); });

                if (numDxvaOutputFormats < 1)
                    continue;

                std::vector<AVPixelFormat> outputFormats;
                for (uint32_t iformat = 0; iformat < numDxvaOutputFormats; ++iformat)
                    outputFormats.emplace_back(D3DFormat2AvPixelFormat(dxvaOutputFormats[iformat]));

                CodecLevel codecLevel = testCodecLevel(dxvaCodecGuid);
                descs[dxvaCodecId].push_back({ GUID2String(dxvaCodecGuid), codecLevel, outputFormats });
            }
            return descs;
        }

        std::vector<CodecDesc> D3D9Device::GetCodecDescs(AVCodecID codecId) const
        {
            com_ptr<IDirectXVideoDecoderService> decoderService = GetDecoderServicePointer();
            if (!decoderService)
                return {};

            HRESULT hr = S_OK;
            uint32_t numDecoderGuids = 0;
            GUID * decoderGuids = nullptr;
            hr = decoderService->GetDecoderDeviceGuids(&numDecoderGuids, &decoderGuids);
            if (FAILED(hr))
                return {};
            std::shared_ptr<GUID>(decoderGuids, [](GUID * guids) { CoTaskMemFree(guids); });

            std::vector<CodecDesc> descs;
            for (uint32_t idecoder = 0; idecoder < numDecoderGuids; ++idecoder)
            {
                const GUID & dxvaCodecGuid = decoderGuids[idecoder];
                AVCodecID dxvaCodecId = MapDXVA2CodecID(dxvaCodecGuid);
                if (dxvaCodecId != codecId)
                    continue;

                uint32_t numDxvaOutputFormats = 0;
                D3DFORMAT * dxvaOutputFormats = nullptr;
                hr = decoderService->GetDecoderRenderTargets(dxvaCodecGuid, &numDxvaOutputFormats, &dxvaOutputFormats);
                if (FAILED(hr))
                    continue;
                std::shared_ptr<D3DFORMAT>(dxvaOutputFormats, [](D3DFORMAT * ptr) { CoTaskMemFree(ptr); });

                if (numDxvaOutputFormats < 1)
                    continue;

                std::vector<AVPixelFormat> outputFormats;
                for (uint32_t iformat = 0; iformat < numDxvaOutputFormats; ++iformat)
                    outputFormats.emplace_back(D3DFormat2AvPixelFormat(dxvaOutputFormats[iformat]));

                CodecLevel codecLevel = testCodecLevel(dxvaCodecGuid);
                descs.push_back({ GUID2String(dxvaCodecGuid), codecLevel, outputFormats });

                break;
            }
            return descs;
        }

        std::vector<CodecDesc> D3D9Device::GetCodecDescs(VideoCodecFormat codecFormat) const
        {
            com_ptr<IDirectXVideoDecoderService> decoderService = GetDecoderServicePointer();
            if (!decoderService)
                return {};

            CodecLevel codecLevel = CodecLevelFromSize({ codecFormat.format.width, codecFormat.format.height });
            if (codecLevel == CodecLevelNone)
                return {};

            HRESULT hr = S_OK;
            uint32_t numDecoderGuids = 0;
            GUID * decoderGuids = nullptr;
            hr = decoderService->GetDecoderDeviceGuids(&numDecoderGuids, &decoderGuids);
            if (FAILED(hr))
                return {};
            std::shared_ptr<GUID>(decoderGuids, [](GUID * guids) { CoTaskMemFree(guids); });

            std::vector<CodecDesc> descs;
            for (uint32_t idecoder = 0; idecoder < numDecoderGuids; ++idecoder)
            {
                const GUID & dxvaCodecGuid = decoderGuids[idecoder];
                AVCodecID dxvaCodecId = MapDXVA2CodecID(dxvaCodecGuid);
                if (dxvaCodecId != codecFormat.codecId)
                    continue;

                uint32_t numDxvaOutputFormats = 0;
                D3DFORMAT * dxvaOutputFormats = nullptr;
                hr = decoderService->GetDecoderRenderTargets(dxvaCodecGuid, &numDxvaOutputFormats, &dxvaOutputFormats);
                if (FAILED(hr))
                    continue;
                std::shared_ptr<D3DFORMAT>(dxvaOutputFormats, [](D3DFORMAT * ptr) { CoTaskMemFree(ptr); });

                if (numDxvaOutputFormats < 1)
                    continue;

                std::vector<AVPixelFormat> outputFormats;
                for (uint32_t iformat = 0; iformat < numDxvaOutputFormats; ++iformat)
                    outputFormats.emplace_back(D3DFormat2AvPixelFormat(dxvaOutputFormats[iformat]));

                bool codecTest = testCodec(dxvaCodecGuid, codecFormat);
                if (!codecTest)
                    continue;

                descs.push_back({ GUID2String(dxvaCodecGuid), codecLevel, outputFormats });

                break;
            }
            return descs;
        }

        struct D3D9SurfaceVideoPlayerDefaultDeviceHWAccelerator : public IVideoDecoderHWAccelerator, public std::enable_shared_from_this<D3D9SurfaceVideoPlayerDefaultDeviceHWAccelerator>
        {
        private:
            struct AVFrameDXVAContext
            {
            public:
                int32_t _index = 0;
                std::shared_ptr<D3D9SurfaceVideoPlayerDefaultDeviceHWAccelerator> _accelerator;
            };
        public:
            D3D9SurfaceVideoPlayerDefaultDeviceHWAccelerator(com_ptr<IDirect3DDeviceManager9> d3d9DeviceManager) :
                _d3d9DeviceManager(d3d9DeviceManager) {}

            ~D3D9SurfaceVideoPlayerDefaultDeviceHWAccelerator()
            {
            }

            error_e SetCodecFormat(VideoCodecFormat codecFormat)
            {
                HRESULT hr = S_OK;
                if (!_decoderService)
                {
                    HANDLE hDevice = INVALID_HANDLE_VALUE;
                    hr = _d3d9DeviceManager->OpenDeviceHandle(&hDevice);
                    if (FAILED(hr))
                        return error_inner;

                    com_ptr<IDirectXVideoDecoderService> decoderService;
                    hr = _d3d9DeviceManager->GetVideoService(hDevice, __uuidof(IDirectXVideoDecoderService),
                        (void **)&decoderService);
                    if (FAILED(hr))
                    {
                        _d3d9DeviceManager->CloseDeviceHandle(hDevice);
                        return error_inner;
                    }

                    _hDevice.reset((void *)hDevice, [this](void * ptr)
                    {
                        if (ptr)
                            _d3d9DeviceManager->CloseDeviceHandle((HANDLE)ptr);
                    });
                    _decoderService = decoderService;
                }

                DXVA2_VideoDesc desc = { 0 };
                desc.SampleWidth = codecFormat.format.width;
                desc.SampleHeight = codecFormat.format.height;
                desc.Format = (D3DFORMAT)D3DFormatExNV12;

                GUID decoderGuid = {};
                DXVA2_ConfigPictureDecode dxvaConfig = {};

                uint32_t numDecoderGuids = 0;
                GUID * decoderGuids = nullptr;
                hr = _decoderService->GetDecoderDeviceGuids(&numDecoderGuids, &decoderGuids);
                if (FAILED(hr))
                    return error_generic;

                std::shared_ptr<GUID>(decoderGuids, [](GUID * guids) { CoTaskMemFree(guids); });

                uint32_t scoreMax = 0;
                uint32_t scoreMaxConfigIndex = 0;
                for (uint32_t idecoder = 0; idecoder < numDecoderGuids; ++idecoder)
                {
                    const GUID & dxvaCodecGuid = decoderGuids[idecoder];
                    AVCodecID guidCodecId = MapDXVA2CodecID(dxvaCodecGuid);
                    if (guidCodecId != codecFormat.codecId)
                        continue;

                    // 测试是否支持指定的解码器
                    uint32_t numConfigs = 0;
                    DXVA2_ConfigPictureDecode * configs = nullptr;
                    hr = _decoderService->GetDecoderConfigurations(dxvaCodecGuid, &desc, NULL, &numConfigs, &configs);
                    if (FAILED(hr))
                        continue;

                    std::shared_ptr<DXVA2_ConfigPictureDecode>(configs, [](DXVA2_ConfigPictureDecode * configs)
                    {
                        CoTaskMemFree(configs);
                    });

                    for (uint32_t icfg = 0; icfg < numConfigs; ++icfg)
                    {
                        const DXVA2_ConfigPictureDecode & config = configs[icfg];

                        uint32_t score = 0;
                        if (config.ConfigBitstreamRaw == 1)
                            score = 1;
                        else if ((codecFormat.codecId == AV_CODEC_ID_H264 || codecFormat.codecId == AV_CODEC_ID_H265) && config.ConfigBitstreamRaw == 2)
                            score = 2;
                        else
                            continue;

                        if (IsEqualGUID(config.guidConfigBitstreamEncryption, DXVA2_NoEncrypt))
                            score += 16;

                        if (score > scoreMax)
                        {
                            scoreMax = score;
                            scoreMaxConfigIndex = icfg;
                        }
                    }

                    if (scoreMax)
                    {
                        uint32_t numOutputFormats = 0;
                        D3DFORMAT * outputFormats = nullptr;
                        hr = _decoderService->GetDecoderRenderTargets(dxvaCodecGuid, &numOutputFormats, &outputFormats);
                        if (FAILED(hr))
                            continue;
                        std::shared_ptr<D3DFORMAT>(outputFormats, [](D3DFORMAT * guids) { CoTaskMemFree(guids); });

                        if (numOutputFormats < 1)
                            continue;

                        desc.Format = outputFormats[0];
                        decoderGuid = dxvaCodecGuid;
                        dxvaConfig = configs[scoreMaxConfigIndex];
                        break;
                    }
                }

                if (desc.Format == D3DFMT_UNKNOWN)
                    return error_generic;

                int numSurfaces = 0;
                int surfaceAlignment = 0;
                /* decoding MPEG-2 requires additional alignment on some Intel GPUs,
                but it causes issues for H.264 on certain AMD GPUs..... */
                if (codecFormat.codecId == AV_CODEC_ID_MPEG2VIDEO)
                    surfaceAlignment = 32;
                /* the HEVC DXVA2 spec asks for 128 pixel aligned surfaces to ensure
                all coding features have enough room to work with */
                else if (codecFormat.codecId == AV_CODEC_ID_HEVC)
                    surfaceAlignment = 128;
                else
                    surfaceAlignment = 16;

                /* 4 base work surfaces */
                numSurfaces = 4;

                /* add surfaces based on number of possible refs */
                if (codecFormat.codecId == AV_CODEC_ID_H264 || codecFormat.codecId == AV_CODEC_ID_HEVC)
                    numSurfaces += 16;
                else if (codecFormat.codecId == AV_CODEC_ID_VP9)
                    numSurfaces += 8;
                else
                    numSurfaces += 2;

                /* add extra surfaces for frame threading */
                ////if (codecCtx->active_thread_type & FF_THREAD_FRAME)
                ////    accelerator->num_surfaces += codecCtx->thread_count;

                D3DFORMAT format = (D3DFORMAT)D3DFormatExNV12;;
                IDirect3DSurface9 ** surfaces = (IDirect3DSurface9 **)av_mallocz(numSurfaces * sizeof(IDirect3DSurface9 *));

                int32_t surfaceW = FFALIGN(codecFormat.format.width, surfaceAlignment);
                int32_t surfaceH = FFALIGN(codecFormat.format.height, surfaceAlignment);
                hr = _decoderService->CreateSurface(
                    surfaceW,
                    surfaceH,
                    numSurfaces - 1,
                    format, D3DPOOL_DEFAULT, 0,
                    DXVA2_VideoDecoderRenderTarget,
                    surfaces, NULL);

                if (FAILED(hr))
                    return error_inner;

                //desc.SampleWidth = codecFormat.format.width;
                //desc.SampleHeight = codecFormat.format.height;
                //desc.Format = AvPixelFormat2D3DFormat(codecFormat.format.pixelFormat);

                com_ptr<IDirectXVideoDecoder> decoder;
                hr = _decoderService->CreateVideoDecoder(decoderGuid,
                    &desc, &dxvaConfig, surfaces,
                    numSurfaces, &decoder);
                if (FAILED(hr))
                    return error_inner;

                _inputCodecFormat = codecFormat;
                _decoderGuid = decoderGuid;
                _dxvaConfig = dxvaConfig;
                _decoder = decoder;

                _numSurfaces = numSurfaces;
                _surfaces.reset(surfaces, [numSurfaces](IDirect3DSurface9 ** surfaces)
                {
                    if (!surfaces)
                        return;

                    for (int32_t cnt = 0; cnt < numSurfaces; ++cnt)
                    {
                        if (surfaces[cnt])
                            surfaces[cnt]->Release();
                    }
                    av_free(surfaces);
                });
                _used.resize(numSurfaces, false);
                _used.assign(numSurfaces, false);

                // device context
                std::shared_ptr<AVBufferRef> avdeviceContext(av_hwdevice_ctx_alloc(AV_HWDEVICE_TYPE_DXVA2), [](AVBufferRef * ptr) {if (ptr) av_buffer_unref(&ptr); });
                AVHWDeviceContext * avdeviceContext_ptr = reinterpret_cast<AVHWDeviceContext *>(avdeviceContext->data);
                avdeviceContext_ptr->free = 0;
                avdeviceContext_ptr->user_opaque = 0;
                AVDXVA2DeviceContext * decoderDeviceContext = (AVDXVA2DeviceContext *)(avdeviceContext_ptr->hwctx);
                decoderDeviceContext->devmgr = _d3d9DeviceManager.ref();

                // frames context
                std::shared_ptr<AVBufferRef> avframeContext(av_hwframe_ctx_alloc(avdeviceContext.get()), [](AVBufferRef * ptr) {if (ptr) av_buffer_unref(&ptr); });
                AVHWFramesContext *  avframesContext_ptr = reinterpret_cast<AVHWFramesContext *>(avframeContext->data);
                avframesContext_ptr->free = 0;
                avframesContext_ptr->user_opaque = 0;
                avframesContext_ptr->format = AV_PIX_FMT_DXVA2_VLD;
                avframesContext_ptr->sw_format = AV_PIX_FMT_NV12;
                avframesContext_ptr->width = surfaceW;
                avframesContext_ptr->height = surfaceH;
                avframesContext_ptr->initial_pool_size = numSurfaces;

                AVDXVA2FramesContext * decoderFramesContext = (AVDXVA2FramesContext *)(avframesContext_ptr->hwctx);
                decoderFramesContext->surfaces = _surfaces.get();
                decoderFramesContext->nb_surfaces = _numSurfaces;
                decoderFramesContext->decoder_to_release = _decoder.ref();
                decoderFramesContext->surface_type = DXVA2_VideoDecoderRenderTarget;

                _avdeviceContext = avdeviceContext;
                _avframeContext = avframeContext;

                return error_ok;
            }

            error_e NeedReset() const
            {
                if (!_hDevice)
                    return error_state;

                HRESULT hr = _d3d9DeviceManager->TestDevice((HANDLE)_hDevice.get());
                if (hr == DXVA2_E_NEW_VIDEO_DEVICE)
                    return error_ok;
                else
                    return error_state;
            }

            error_e Reset()
            {
                common::logger::act() << __FUNCTION__" Reset dxva2 decoder";
                std::lock_guard<std::mutex> lock(mtx);
                _surfaces.reset();
                _numSurfaces = 0;
                _decoder.reset();
                _decoderService.reset();
                _hDevice.reset();
                //return error_ok;
                _avdeviceContext.reset();
                _avframeContext.reset();
                return SetCodecFormat(_inputCodecFormat);
            }

            virtual std::shared_ptr<AVBufferRef> GetFFmmpegHWAccelDeviceContext() { return _avdeviceContext; }
            virtual std::shared_ptr<AVBufferRef> GetFFmmpegHWAccelFramesContext() { return _avframeContext; }

            AVPixelFormat GetOutputPixelFormat() const { return AV_PIX_FMT_DXVA2_VLD; }

            error_e ReleaseBuffer(AVFrameDXVAContext * ctx)
            {
                if (!ctx)
                    return error_nullptr;

                std::lock_guard<std::mutex> lock(mtx);
                assert(0 <= ctx->_index && ctx->_index < _used.size());
                if (ctx->_index < _numSurfaces)
                {
                    _used[ctx->_index] = false;
                }
                return error_ok;
            }

            static void dxva2_framebuffer_free(void * opaque, uint8_t * data)
            {
                if (!opaque)
                    return;
                AVFrameDXVAContext * ctx = reinterpret_cast<AVFrameDXVAContext *>(data);
                ctx->_accelerator->ReleaseBuffer(ctx);
                delete ctx;
            }

            error_e GetBuffer(std::shared_ptr<AVFrame> frame, int32_t flags)
            {
                std::lock_guard<std::mutex> lock(mtx);

                int32_t index = -1;
                for (int32_t cnt = 0; cnt < _numSurfaces; cnt++)
                {
                    if (!_used[cnt])
                    {
                        index = cnt;
                        break;
                    }
                }
                if (index == -1)
                {
                    assert(false);
                    return error_bad_data;
                }

                frame->data[3] = (uint8_t *)_surfaces.get()[index];
                _used[index] = true;

                AVFrameDXVAContext * ctx = new AVFrameDXVAContext();
                ctx->_index = index;
                ctx->_accelerator = shared_from_this();

                frame->buf[0] = av_buffer_create((uint8_t*)ctx, 0, dxva2_framebuffer_free, (void *)this, AV_BUFFER_FLAG_READONLY);

                frame->format = AV_PIX_FMT_DXVA2_VLD;
                return error_ok;
            }

            std::mutex mtx;
            com_ptr<IDirect3DDeviceManager9> _d3d9DeviceManager;

            std::shared_ptr<void> _hDevice;
            com_ptr<IDirectXVideoDecoderService> _decoderService;

            GUID _decoderGuid = {};
            DXVA2_ConfigPictureDecode _dxvaConfig = {};
            com_ptr<IDirectXVideoDecoder> _decoder;

            int32_t _numSurfaces = 0;
            std::shared_ptr<IDirect3DSurface9 *> _surfaces;

            std::vector<bool> _used;

            VideoCodecFormat _inputCodecFormat;
            VideoFormat _outputCodecFormat;

            std::shared_ptr<AVBufferRef> _avdeviceContext;
            std::shared_ptr<AVBufferRef> _avframeContext;
        };

        std::shared_ptr<IVideoDecoderHWAccelerator> D3D9Device::CreateAccelerator(VideoCodecFormat codecFormat)
        {
            std::lock_guard<std::mutex> lock(_mtx);

            HRESULT hr = S_OK;
            // dxva2
            error_e state = initDecoderService();
            if (state < 0 || !_decoderService)
                return nullptr;


            std::shared_ptr<D3D9SurfaceVideoPlayerDefaultDeviceHWAccelerator> accelerator = std::make_shared<D3D9SurfaceVideoPlayerDefaultDeviceHWAccelerator>(_d3d9DeviceManager);
            state = accelerator->SetCodecFormat(codecFormat);
            if (state < 0)
                return nullptr;
            return accelerator;
        }

        error_e D3D9Device::initDevice()
        {
            if (_d3d9Device)
                return error_ok;

            HRESULT hr = S_OK;
            if (!_d3d9 || !_window)
                return error_state;

            HWND hWnd = (HWND)_window->GetHandle();
            RECT rcClient;
            GetClientRect(hWnd, &rcClient);
            int32_t width = std::max(rcClient.right - rcClient.left, 16L);
            int32_t height = std::max(rcClient.bottom - rcClient.top, 16L);

            uint32_t adapterId = D3DADAPTER_DEFAULT;

            com_ptr<struct IDirect3DDevice9> d3d9Device;
            D3DPRESENT_PARAMETERS d3dparam = {};
            d3dparam.AutoDepthStencilFormat = D3DFMT_D24S8;
            d3dparam.BackBufferCount = 2;
            d3dparam.BackBufferFormat = D3DFMT_A8R8G8B8;
            d3dparam.BackBufferWidth = width;
            d3dparam.BackBufferHeight = height;
            d3dparam.EnableAutoDepthStencil = true;
            d3dparam.Flags = /*D3DPRESENTFLAG_*/0;
            d3dparam.FullScreen_RefreshRateInHz = 0;
            d3dparam.hDeviceWindow = hWnd;
            d3dparam.MultiSampleQuality = 0;
            d3dparam.MultiSampleType = D3DMULTISAMPLE_NONE;
            d3dparam.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
            d3dparam.SwapEffect = D3DSWAPEFFECT_FLIP;
            d3dparam.Windowed = TRUE;

            hr = _d3d9->GetD3D9()->CreateDevice(adapterId, D3DDEVTYPE_HAL, hWnd,
                D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,
                &d3dparam, &d3d9Device);
            if (FAILED(hr))
                return error_inner;

            _d3d9Device = d3d9Device;
            _deviceFormat = { width, height, D3DFormat2AvPixelFormat(d3dparam.BackBufferFormat) };

            return error_ok;
        }

        error_e D3D9Device::initDecoderService()
        {
            if (_decoderService)
                return error_ok;

            if (!_d3d9Device)
                return error_state;

            HRESULT hr = S_OK;
            uint32_t resetToken = 0;
            com_ptr<IDirect3DDeviceManager9> d3d9DeviceManager;
            hr = DXVA2CreateDirect3DDeviceManager9(&resetToken, &d3d9DeviceManager);
            if (FAILED(hr))
                return error_inner;

            hr = d3d9DeviceManager->ResetDevice(_d3d9Device.get(), resetToken);
            if (FAILED(hr))
                return error_inner;

            HANDLE handle = INVALID_HANDLE_VALUE;
            hr = d3d9DeviceManager->OpenDeviceHandle(&handle);
            if (FAILED(hr))
                return error_inner;

            com_ptr<IDirectXVideoDecoderService> decoderService;
            hr = d3d9DeviceManager->GetVideoService(handle, __uuidof(IDirectXVideoDecoderService),
                (void **)&decoderService);
            if (FAILED(hr))
                return error_inner;

            _hDevice = (void *)handle;
            _resetToken = resetToken;
            _d3d9DeviceManager = d3d9DeviceManager;
            _decoderService = decoderService;

            return error_ok;
        }

        bool D3D9Device::testCodec(const GUID & dxvaCodecGuid, VideoCodecFormat format) const
        {
            com_ptr<IDirectXVideoDecoderService> decoderService = GetDecoderServicePointer();
            if (!decoderService)
                return false;

            HRESULT hr = S_OK;
            DXVA2_VideoDesc desc = { 0 };
            desc.SampleWidth = format.format.width;
            desc.SampleHeight = format.format.height;
            desc.Format = AvPixelFormat2D3DFormat(format.format.pixelFormat);

            uint32_t numDxvaConfigs = 0;
            DXVA2_ConfigPictureDecode * dxvaConfigs = nullptr;
            hr = decoderService->GetDecoderConfigurations(dxvaCodecGuid, &desc, NULL, &numDxvaConfigs, &dxvaConfigs);
            if (FAILED(hr))
                return false;

            std::shared_ptr<DXVA2_ConfigPictureDecode>(dxvaConfigs, [](DXVA2_ConfigPictureDecode * ptr) { CoTaskMemFree(ptr); });
            if (numDxvaConfigs < 1)
                return false;

            uint32_t scoreMax = 0;
            for (uint32_t icfg = 0; icfg < numDxvaConfigs; ++icfg)
            {
                const DXVA2_ConfigPictureDecode & cfg = dxvaConfigs[icfg];

                uint32_t score = 0;
                if (cfg.ConfigBitstreamRaw == 1)
                    score = 1;
                else if ((format.codecId == AV_CODEC_ID_H264 || format.codecId == AV_CODEC_ID_H265) && cfg.ConfigBitstreamRaw == 2)
                    score = 2;
                else
                    continue;

                if (IsEqualGUID(cfg.guidConfigBitstreamEncryption, DXVA2_NoEncrypt))
                    score += 16;

                if (score > scoreMax)
                    return true;
            }
            return false;
        }

        CodecLevel D3D9Device::testCodecLevel(const GUID & dxvaCodecGuid) const
        {
            com_ptr<IDirectXVideoDecoderService> decoderService = GetDecoderServicePointer();
            if (!decoderService)
                return CodecLevelNone;

            HRESULT hr = S_OK;
            uint32_t numDecoderGuids = 0;
            GUID * decoderGuids = nullptr;
            hr = decoderService->GetDecoderDeviceGuids(&numDecoderGuids, &decoderGuids);
            if (FAILED(hr))
                return CodecLevelNone;

            DXVA2_VideoDesc desc = { 0 };
            desc.Format = AvPixelFormat2D3DFormat(AV_PIX_FMT_NV12);

            CodecLevel codecLevel = CodecLevelNone;
            for (int level = CodecLevelSD; level <= CodecLevel8K; ++level)
            {
                uint32_t numDxvaOutputFormats = 0;
                D3DFORMAT * dxvaOutputFormats = nullptr;
                hr = decoderService->GetDecoderRenderTargets(dxvaCodecGuid, &numDxvaOutputFormats, &dxvaOutputFormats);
                if (FAILED(hr))
                    continue;
                std::shared_ptr<D3DFORMAT>(dxvaOutputFormats, [](D3DFORMAT * ptr) { CoTaskMemFree(ptr); });
                if (numDxvaOutputFormats < 1)
                    continue;

                CodecSize size = CodecLevelToSize((CodecLevel)level);
                desc.SampleWidth = size.width;
                desc.SampleHeight = size.height;

                uint32_t numDxvaConfigs = 0;
                DXVA2_ConfigPictureDecode * dxvaConfigs = nullptr;
                hr = decoderService->GetDecoderConfigurations(dxvaCodecGuid, &desc, NULL, &numDxvaConfigs, &dxvaConfigs);
                if (FAILED(hr))
                    continue;
                std::shared_ptr<DXVA2_ConfigPictureDecode>(dxvaConfigs, [](DXVA2_ConfigPictureDecode * ptr) { CoTaskMemFree(ptr); });
                if (numDxvaConfigs < 1)
                    break;

                codecLevel = (CodecLevel)level;
            }
            return codecLevel;
        }
    }
}
