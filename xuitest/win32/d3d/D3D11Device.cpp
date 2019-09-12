#include "stdafx.h"
#include "win32/windows.h"
#include "win32/dll.h"
#include "D3D11Device.h"
#include <d3dcommon.h>
#include <d3d11.h>
#include <libavutil/hwcontext_d3d11va.h>
#include "player/av.h"

static void d3d11va_default_lock(void *ctx)
{
    WaitForSingleObjectEx(ctx, INFINITE, FALSE);
}

static void d3d11va_default_unlock(void *ctx)
{
    ReleaseMutex(ctx);
}


#define D3DALIGN(x, a) (((x)+(a)-1)&~((a)-1))
namespace win32 {
    namespace d3d {
        using namespace common;
        using namespace player;

        constexpr GUID DXVA2_NoEncrypt = { 0x1b81beD0, 0xa0c7, 0x11d3, 0xb9, 0x84, 0x00, 0xc0, 0x4f, 0x2e, 0x73, 0xc5 };
        static const DXGI_FORMAT DECODER_CHECK_FORMATS[] = { DXGI_FORMAT_NV12 , DXGI_FORMAT_420_OPAQUE, DXGI_FORMAT_P010, DXGI_FORMAT_R8G8B8A8_UNORM };

        static const DXGI_FORMAT DECODER_STAGING_FORMATS[] = { DXGI_FORMAT_NV12, DXGI_FORMAT_420_OPAQUE, DXGI_FORMAT_P010, DXGI_FORMAT_R8G8B8A8_UNORM };

        struct D3D11VideoDecoderHWAccelerator : public IVideoDecoderHWAccelerator, public std::enable_shared_from_this<D3D11VideoDecoderHWAccelerator>
        {
        public:
            struct AVFrameDX11VAContext
            {
            public:
                int32_t _index = 0;
                std::shared_ptr<D3D11VideoDecoderHWAccelerator> _accelerator;
            };

            D3D11VideoDecoderHWAccelerator(std::shared_ptr<D3D11Device> device) : _device(device) {}
            ~D3D11VideoDecoderHWAccelerator()
            {
            }

            error_e SetCodecFormat(VideoCodecFormat codecFormat)
            {
                HRESULT hr = S_OK;
                if (!_device)
                    return error_state;

                com_ptr<ID3D11Device> dxdevice = _device->GetPointer();
                com_ptr<ID3D11DeviceContext> deviceContext;
                dxdevice->GetImmediateContext(&deviceContext);
                if (!deviceContext)
                    return error_inner;

                com_ptr<ID3D11VideoDevice> videoDevice = dxdevice;
                if (!dxdevice || !videoDevice)
                    return error_inner;

                GUID decoderProfileGuid = {};
                D3D11_VIDEO_DECODER_CONFIG decoderConfig = {};
                uint32_t profileCount = videoDevice->GetVideoDecoderProfileCount();
                uint32_t scoreMax = 0;
                for (uint32_t iprofile = 0; iprofile < profileCount; ++iprofile)
                {
                    GUID profileGuid = {};
                    hr = videoDevice->GetVideoDecoderProfile(iprofile, &profileGuid);
                    if (FAILED(hr))
                        continue;

                    AVCodecID dxvaCodecId = MapDXVA2CodecID(profileGuid);
                    if (dxvaCodecId != codecFormat.codecId)
                        continue;

                    D3D11_VIDEO_DECODER_DESC desc = {};
                    desc.Guid = profileGuid;
                    desc.SampleWidth = codecFormat.format.width;
                    desc.SampleHeight = codecFormat.format.height;
                    desc.OutputFormat = DXGI_FORMAT_NV12;

                    // 找一个合适的配置
                    uint32_t numConfigs = 0;
                    hr = videoDevice->GetVideoDecoderConfigCount(&desc, &numConfigs);
                    if (FAILED(hr) || numConfigs < 1)
                        continue;

                    for (uint32_t icfg = 0; icfg < numConfigs; ++icfg)
                    {
                        D3D11_VIDEO_DECODER_CONFIG config = {};
                        hr = videoDevice->GetVideoDecoderConfig(&desc, icfg, &config);
                        if (FAILED(hr))
                            continue;

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
                            decoderProfileGuid = profileGuid;
                            decoderConfig = config;
                            scoreMax = score;
                        }
                    }
                }

                if (decoderProfileGuid == GUID())
                    return error_not_supported;

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
                numSurfaces = 4 + 4;

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


                D3D11_TEXTURE2D_DESC texturesDesc = { 0 };

                texturesDesc.Width = D3DALIGN(codecFormat.format.width, surfaceAlignment);
                texturesDesc.Height = D3DALIGN(codecFormat.format.height, surfaceAlignment);
                texturesDesc.MipLevels = 1;
                texturesDesc.ArraySize = numSurfaces;
                texturesDesc.Format = DXGI_FORMAT_NV12;
                texturesDesc.SampleDesc.Count = 1;
                texturesDesc.Usage = D3D11_USAGE_DEFAULT;
                texturesDesc.MiscFlags = 0;
                texturesDesc.BindFlags = D3D11_BIND_DECODER | D3D11_BIND_SHADER_RESOURCE;
                texturesDesc.CPUAccessFlags = 0;

                com_ptr<ID3D11Texture2D> texture;
                hr = dxdevice->CreateTexture2D(&texturesDesc, NULL, &texture);
                if (FAILED(hr))
                    return error_inner;

                //D3D11_VIDEO_DECODER_OUTPUT_VIEW_DESC decoderOutputViewDesc = {};
                //decoderOutputViewDesc.DecodeProfile = decoderProfileGuid;
                //decoderOutputViewDesc.ViewDimension = D3D11_VDOV_DIMENSION_TEXTURE2D;

                D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourcesViewDescY = {};
                D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourcesViewDescUV = {};
                shaderResourcesViewDescY.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
                shaderResourcesViewDescY.Format = DXGI_FORMAT_R8_UNORM;
                shaderResourcesViewDescY.Texture2DArray.MipLevels = 1;
                shaderResourcesViewDescY.Texture2DArray.MostDetailedMip = 0;
                shaderResourcesViewDescY.Texture2DArray.FirstArraySlice = 0;
                shaderResourcesViewDescY.Texture2DArray.ArraySize = 1;
                shaderResourcesViewDescUV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
                shaderResourcesViewDescUV.Format = DXGI_FORMAT_R8G8_UNORM;
                shaderResourcesViewDescUV.Texture2DArray.MipLevels = 1;
                shaderResourcesViewDescUV.Texture2DArray.MostDetailedMip = 0;
                shaderResourcesViewDescUV.Texture2DArray.FirstArraySlice = 0;
                shaderResourcesViewDescUV.Texture2DArray.ArraySize = 1;

                std::vector<com_ptr<ID3D11VideoDecoderOutputView>> decoderViews;
                std::vector<com_ptr<ID3D11ShaderResourceView>> resourceViewsY;
                std::vector<com_ptr<ID3D11ShaderResourceView>> resourceViewsUV;
                for(int iview = 0; iview < numSurfaces; ++iview)
                {
                    //decoderOutputViewDesc.Texture2D.ArraySlice = iview;
                    //com_ptr<ID3D11VideoDecoderOutputView> decoderView;
                    //hr = videoDevice->CreateVideoDecoderOutputView(texture.get(), &decoderOutputViewDesc, &decoderView);
                    //if (FAILED(hr))
                    //    return error_inner;
                    //decoderViews.emplace_back(decoderView);

                    shaderResourcesViewDescY.Texture2DArray.FirstArraySlice = iview;
                    shaderResourcesViewDescY.Texture2DArray.ArraySize = 1;
                    shaderResourcesViewDescUV.Texture2DArray.FirstArraySlice = iview;
                    shaderResourcesViewDescUV.Texture2DArray.ArraySize = 1;
                    com_ptr<ID3D11ShaderResourceView> resourceViewY;
                    com_ptr<ID3D11ShaderResourceView> resourceViewUV;
                    hr = dxdevice->CreateShaderResourceView(texture.get(), &shaderResourcesViewDescY, &resourceViewY);
                    if (FAILED(hr))
                        return error_inner;
                    hr = dxdevice->CreateShaderResourceView(texture.get(), &shaderResourcesViewDescUV, &resourceViewUV);
                    if (FAILED(hr))
                        return error_inner;
                    resourceViewsY.emplace_back(resourceViewY);
                    resourceViewsUV.emplace_back(resourceViewUV);

                    DxSetDebugName(resourceViewY.get(), "resourceViewY");
                    DxSetDebugName(resourceViewUV.get(), "resourceViewUV");
                }

                //D3D11_VIDEO_DECODER_DESC decoderDesc = {};
                //decoderDesc.Guid = decoderProfileGuid;
                //decoderDesc.SampleWidth = codecFormat.format.width;
                //decoderDesc.SampleHeight = codecFormat.format.height;
                //decoderDesc.OutputFormat = DXGI_FORMAT_NV12;

                //com_ptr<ID3D11VideoDecoder> decoder;
                //hr = videoDevice->CreateVideoDecoder(&decoderDesc, &decoderConfig, &decoder);
                //if (FAILED(hr))
                //    return error_inner;

                com_ptr<ID3D11VideoContext> videoContext = deviceContext;
                if(!videoContext)
                    return error_inner;

                // device context
                std::shared_ptr<AVBufferRef> avdeviceContext(av_hwdevice_ctx_alloc(AV_HWDEVICE_TYPE_D3D11VA), [](AVBufferRef * ptr) {if (ptr) av_buffer_unref(&ptr); });
                AVHWDeviceContext * avdeviceContext_ptr = reinterpret_cast<AVHWDeviceContext *>(avdeviceContext->data);
                avdeviceContext_ptr->free = 0;
                avdeviceContext_ptr->user_opaque = 0;
                AVD3D11VADeviceContext * decoderDeviceContext = (AVD3D11VADeviceContext *)(avdeviceContext_ptr->hwctx);
                decoderDeviceContext->device = dxdevice.ref();
                decoderDeviceContext->device_context = deviceContext.ref();
                decoderDeviceContext->video_device = videoDevice.ref();
                decoderDeviceContext->video_context = videoContext.ref();
                decoderDeviceContext->lock_ctx = CreateMutex(NULL, 0, NULL);
                decoderDeviceContext->lock = d3d11va_default_lock;
                decoderDeviceContext->unlock = d3d11va_default_unlock;

                // frames context
                std::shared_ptr<AVBufferRef> avframeContext(av_hwframe_ctx_alloc(avdeviceContext.get()), [](AVBufferRef * ptr) {if(ptr) av_buffer_unref(&ptr); });
                AVHWFramesContext *  avframesContext_ptr = reinterpret_cast<AVHWFramesContext *>(avframeContext->data);
                avframesContext_ptr->free = 0;
                avframesContext_ptr->user_opaque = 0;
                avframesContext_ptr->format = AV_PIX_FMT_D3D11;
                avframesContext_ptr->sw_format = AV_PIX_FMT_NV12;
                avframesContext_ptr->width = texturesDesc.Width;
                avframesContext_ptr->height = texturesDesc.Height;
                avframesContext_ptr->initial_pool_size = numSurfaces;

                AVD3D11VAFramesContext * decoderFramesContext = (AVD3D11VAFramesContext *)(avframesContext_ptr->hwctx);
                decoderFramesContext->texture = texture.ref();

                int err = av_hwframe_ctx_init(avframeContext.get());
                assert(err >= 0);
                if (err)
                {
                    logger::err() << __FUNCTION__" av_hwframe_ctx_init failed " << averr_str(err);
                    return error_inner;
                }

                _dxdevice = dxdevice;
                _dxeviceContext = deviceContext;
                //_decoder = decoder;
                _videoDevice = videoDevice;
                _videoContxt = videoContext;
                _inputCodecFormat = codecFormat;
                _decoderGuid = decoderProfileGuid;
                _decoderConfig = decoderConfig;
                _texture = texture;
                _decoderViews.swap(decoderViews);
                _resourceViewsY.swap(resourceViewsY);
                _resourceViewsUV.swap(resourceViewsUV);
                _used.resize(numSurfaces, false);

                _avdeviceContext = avdeviceContext;
                _avframeContext = avframeContext;
                _surfaceFormat = { (int32_t)texturesDesc.Width, (int32_t)texturesDesc.Height, AV_PIX_FMT_D3D11 };

                return error_ok;
            }

            error_e NeedReset() const
            {
                return error_state;
            }

            error_e Reset()
            {
                common::logger::act() << __FUNCTION__" Reset d3d11va decoder";
                std::lock_guard<std::mutex> lock(_mtx);
                return SetCodecFormat(_inputCodecFormat);
            }

            virtual void * GetFFmpegHWAccelContext() { return nullptr; }

            virtual std::shared_ptr<AVBufferRef> GetFFmmpegHWAccelDeviceContext() { return _avdeviceContext; }
            virtual std::shared_ptr<AVBufferRef> GetFFmmpegHWAccelFramesContext() { return _avframeContext; }

            AVPixelFormat GetOutputPixelFormat() const { return AV_PIX_FMT_D3D11; }

            error_e ReleaseBuffer(AVFrameDX11VAContext * ctx)
            {
                std::lock_guard<std::mutex> lock(_mtx);

                assert(0 <= ctx->_index && ctx->_index < _used.size());
                _used[ctx->_index] = false;
                return error_ok;
            }

            static void release_frame_buffer(void * opaque, uint8_t * data)
            {
                AVFrameDX11VAContext * ctx = reinterpret_cast<AVFrameDX11VAContext *>(data);
                ctx->_accelerator->ReleaseBuffer(ctx);
                delete ctx;
            }

            error_e GetBuffer(std::shared_ptr<AVFrame> frame, int32_t flags)
            {
                std::lock_guard<std::mutex> lock(_mtx);

                int32_t index = -1;
                for (uint32_t cnt = 0; cnt < _used.size(); cnt++)
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

                frame->data[0] = (uint8_t *)_texture.get();
                frame->data[1] = (uint8_t *)index;
                //frame->data[3] = (uint8_t *)_decoderViews[index].get();
                frame->data[4] = (uint8_t *)_resourceViewsY[index].get();
                frame->data[5] = (uint8_t *)_resourceViewsUV[index].get();


                frame->buf[0] = av_buffer_allocz(sizeof(AVD3D11FrameDescriptor));
                AVD3D11FrameDescriptor * fd = (AVD3D11FrameDescriptor *)(frame->buf[0]->data);
                // 不能 ref
                fd->texture = _texture.get();
                fd->index = index;

                AVFrameDX11VAContext * ctx = new AVFrameDX11VAContext();
                ctx->_index = index;
                ctx->_accelerator = shared_from_this();
                frame->buf[1] = av_buffer_create((uint8_t*)ctx, 0, release_frame_buffer, (void *)this, AV_BUFFER_FLAG_READONLY);

                frame->format = _surfaceFormat.pixelFormat;
                frame->width = _surfaceFormat.width;
                frame->height= _surfaceFormat.height;
                _used[index] = true;

                return error_ok;
            }

        private:
            std::mutex _mtx;
            std::shared_ptr<D3D11Device> _device;
            com_ptr<ID3D11Device> _dxdevice;
            com_ptr<ID3D11DeviceContext> _dxeviceContext;

            std::shared_ptr<AVBufferRef> _avdeviceContext;
            std::shared_ptr<AVBufferRef> _avframeContext;

            com_ptr<ID3D11VideoDecoder> _decoder;
            com_ptr<ID3D11VideoDevice> _videoDevice;
            com_ptr<ID3D11VideoContext> _videoContxt;
            GUID _decoderGuid = {};
            D3D11_VIDEO_DECODER_CONFIG _decoderConfig = {};

            VideoCodecFormat _inputCodecFormat;
            VideoFormat _surfaceFormat;
            com_ptr<ID3D11Texture2D> _texture;
            std::vector<bool> _used;
            std::vector<com_ptr<ID3D11VideoDecoderOutputView>> _decoderViews;
            std::vector<com_ptr<ID3D11ShaderResourceView>> _resourceViewsY;
            std::vector<com_ptr<ID3D11ShaderResourceView>> _resourceViewsUV;
        };

        D3D11Device::D3D11Device(std::shared_ptr<DXGIFactory> dxgi, deviceid_t deviceId) : _dxgi(dxgi), _deviceId(deviceId)
        {

        }

        D3D11Device::~D3D11Device()
        {

        }

        D3DDeviceDesc D3D11Device::GetDesc() const
        {
            com_ptr<IDXGIDevice> dxgiDevice = GetPointer();
            if (!dxgiDevice)
                return {};

            com_ptr<IDXGIAdapter> adapter;
            HRESULT hr = dxgiDevice->GetAdapter(&adapter);
            if (FAILED(hr))
                return {};

            if (!adapter)
                return {};

            DXGI_ADAPTER_DESC dxgiDesc = {};
            hr = adapter->GetDesc(&dxgiDesc);
            if (FAILED(hr))
                return {};

            D3DDeviceDesc desc = {};

            desc.deviceId = dxgiDesc.DeviceId;
            desc.application = "Direct3D 11";
            desc.description = tools::string::ucs2_u8(dxgiDesc.Description);
            desc.vendorId = dxgiDesc.VendorId;
            desc.subSysId = dxgiDesc.SubSysId;
            desc.revision = dxgiDesc.Revision;

            return desc;
        }

        player::CodecDeviceDesc D3D11Device::GetCodecDeviceDesc() const
        {
            D3DDeviceDesc d3dDesc = GetDesc();
            player::CodecDeviceDesc desc;
            desc.application = d3dDesc.application;
            desc.description = d3dDesc.description;
            desc.type = AV_HWDEVICE_TYPE_D3D11VA;
            desc.deviceId = d3dDesc.deviceId;
            desc.vendorId = d3dDesc.vendorId;
            desc.subSysId = d3dDesc.subSysId;
            return desc;
        }

        std::map<AVCodecID, std::vector<player::CodecDesc>> D3D11Device::GetCodecDescs() const
        {
            com_ptr<ID3D11VideoDevice> videoDevice = GetPointer();
            if (!videoDevice)
                return {};

            HRESULT hr = S_OK;
            std::map<AVCodecID, std::vector<CodecDesc>> descs;
            uint32_t profileCount = videoDevice->GetVideoDecoderProfileCount();
            for (uint32_t iprofile = 0; iprofile < profileCount; ++iprofile)
            {
                GUID guidProfile = {};
                hr = videoDevice->GetVideoDecoderProfile(iprofile, &guidProfile);
                if (FAILED(hr))
                    continue;

                AVCodecID dxvaCodecId = MapDXVA2CodecID(guidProfile);

                std::vector<AVPixelFormat> outputFormats;
                for( auto format : DECODER_CHECK_FORMATS)
                {
                    BOOL bSupported = FALSE;
                    hr = videoDevice->CheckVideoDecoderFormat(&guidProfile, format, &bSupported);
                    if (FAILED(hr))
                        continue;

                    if (bSupported)
                        outputFormats.push_back(DXGIFormat2AvPixelFormat(format));
                }

                CodecLevel codecLevel = testCodecLevel(guidProfile);
                descs[dxvaCodecId].push_back({ GUID2String(guidProfile), codecLevel, outputFormats });
            }
            return descs;
        }

        std::vector<player::CodecDesc> D3D11Device::GetCodecDescs(AVCodecID codecId) const
        {
            com_ptr<ID3D11VideoDevice> videoDevice = GetPointer();
            if (!videoDevice)
                return {};

            HRESULT hr = S_OK;
            std::vector<CodecDesc> descs;
            uint32_t profileCount = videoDevice->GetVideoDecoderProfileCount();
            for (uint32_t iprofile = 0; iprofile < profileCount; ++iprofile)
            {
                GUID guidProfile = {};
                hr = videoDevice->GetVideoDecoderProfile(iprofile, &guidProfile);
                if (FAILED(hr))
                    continue;

                AVCodecID dxvaCodecId = MapDXVA2CodecID(guidProfile);
                if (dxvaCodecId != codecId)
                    continue;

                std::vector<AVPixelFormat> outputFormats;
                for (auto format : DECODER_CHECK_FORMATS)
                {
                    BOOL bSupported = FALSE;
                    hr = videoDevice->CheckVideoDecoderFormat(&guidProfile, format, &bSupported);
                    if (FAILED(hr))
                        continue;

                    if (bSupported)
                        outputFormats.push_back(DXGIFormat2AvPixelFormat(format));
                }

                CodecLevel codecLevel = testCodecLevel(guidProfile);
                descs.push_back({ GUID2String(guidProfile), codecLevel, outputFormats });
            }
            return descs;
        }

        std::vector<player::CodecDesc> D3D11Device::GetCodecDescs(player::VideoCodecFormat codecFormat) const
        {
            CodecLevel codecLevel = CodecLevelFromSize({ codecFormat.format.width, codecFormat.format.height });
            if (codecLevel == CodecLevelNone)
                return {};

            com_ptr<ID3D11VideoDevice> videoDevice = GetPointer();
            if (!videoDevice)
                return {};

            HRESULT hr = S_OK;
            std::vector<CodecDesc> descs;
            uint32_t profileCount = videoDevice->GetVideoDecoderProfileCount();
            for (uint32_t iprofile = 0; iprofile < profileCount; ++iprofile)
            {
                GUID guidProfile = {};
                hr = videoDevice->GetVideoDecoderProfile(iprofile, &guidProfile);
                if (FAILED(hr))
                    continue;

                AVCodecID dxvaCodecId = MapDXVA2CodecID(guidProfile);
                if (dxvaCodecId != codecFormat.codecId)
                    continue;

                std::vector<AVPixelFormat> outputFormats;
                for (auto format : DECODER_CHECK_FORMATS)
                {
                    BOOL bSupported = FALSE;
                    hr = videoDevice->CheckVideoDecoderFormat(&guidProfile, format, &bSupported);
                    if (FAILED(hr))
                        continue;

                    if (bSupported)
                        outputFormats.push_back(DXGIFormat2AvPixelFormat(format));
                }

                if(testCodec(guidProfile, codecFormat))
                    descs.push_back({ GUID2String(guidProfile), codecLevel, outputFormats });
            }
            return descs;
        }

        std::shared_ptr<player::IVideoDecoderHWAccelerator> D3D11Device::CreateAccelerator(player::VideoCodecFormat codecFormat)
        {
            com_ptr<ID3D11VideoDevice> videoDevice = GetPointer();
            if (!videoDevice)
                return nullptr;

            std::shared_ptr<D3D11VideoDecoderHWAccelerator> accelerator = std::make_shared<D3D11VideoDecoderHWAccelerator>(shared_from_this());
            error_e state = accelerator->SetCodecFormat(codecFormat);
            if (state < 0)
                return nullptr;
            return accelerator;
        }

        com_ptr<ID3D11Device> D3D11Device::GetPointer() const
        {
            std::lock_guard<std::mutex> lock(const_cast<D3D11Device*>(this)->_mtx);
            if (!_dxgi)
                return nullptr;
            
            if(!_device)
            {
                error_e err = const_cast<D3D11Device *>(this)->initDevice();
                if (err)
                    return nullptr;
            }
            return _device;
        }

        common::error_e D3D11Device::initDevice()
        {
            if (!_dll && _dll.load("d3d11.dll") < 0)
                return error_not_supported;

            if (!_dxgi)
                return error_state;

            if (_device)
                return error_ok;

            static D3D_FEATURE_LEVEL FeatureLevels[] = {
                D3D_FEATURE_LEVEL_11_1,
                D3D_FEATURE_LEVEL_11_0,
            };

            com_ptr<IDXGIAdapter> adapter = _dxgi->GetAdapter(_deviceId);
            if (!adapter)
                return error_not_found;

            auto pfnD3D11CreateDevice = _dll.get<decltype(D3D11CreateDevice) *>("D3D11CreateDevice");
            if (!pfnD3D11CreateDevice)
            {
                logger::err() << __FUNCTION__" canot find D3D11CreateDevice in d3dll.dll " << winerr_str(GetLastError());
                return error_inner;
            }

            D3D_FEATURE_LEVEL pFeatureLevelsOut;
            DWORD dwFlags = 0;
#ifdef _DEBUG
            dwFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
            com_ptr<ID3D11Device> device;
            HRESULT hr = pfnD3D11CreateDevice(adapter.get(),
                D3D_DRIVER_TYPE_UNKNOWN,
                NULL,
                dwFlags,
                FeatureLevels,
                (uint32_t)std::size(FeatureLevels),
                D3D11_SDK_VERSION,
                &device,
                &pFeatureLevelsOut,
                NULL);

            if (FAILED(hr))
                return error_inner;

            com_ptr<ID3D10Multithread> d3d11Multithread = device;
            if (!d3d11Multithread)
                return error_inner;

            d3d11Multithread->SetMultithreadProtected(true);
            BOOL bSucc = d3d11Multithread->GetMultithreadProtected();
            assert(bSucc);
            if(!bSucc)
            {
                logger::err() << __FUNCTION__" d3d11Multithread->SetMultithreadProtected failed";
                return error_inner;
            }
            _device = device;
            DxSetDebugName(_device.get(), "ID3D11Device");

            DXGI_ADAPTER_DESC dxgiDesc = {};
            hr = adapter->GetDesc(&dxgiDesc);
            if (SUCCEEDED(hr))
            {
                D3DDeviceDesc desc = {};
                _deviceId = dxgiDesc.DeviceId;
                _description = tools::string::ucs2_u8(dxgiDesc.Description);
            }

//#ifdef _PLAYER_DEBUG
//            HRESULT hrCheck = S_OK;
//            for(int cnt = 0; cnt < DXGI_FORMAT_V408; ++cnt)
//            {
//                DXGI_FORMAT format = (DXGI_FORMAT)cnt;
//                uint32_t bSupport = false;
//                hrCheck = device->CheckFormatSupport(format, &bSupport);
//                logger::dbg() << __FUNCTION__" " << DXGIFormatName(format) << ((FAILED(hrCheck) || !bSupport) ? " -" : " OK");
//            }
//#endif
            logger::act() << __FUNCTION__" ok";
            return error_ok;
        }

        bool D3D11Device::testCodec(const GUID & guidCodec, player::VideoCodecFormat format) const
        {
            com_ptr<ID3D11VideoDevice> videoDevice = _device;
            if (!videoDevice)
                return false;

            HRESULT hr = S_OK;
            D3D11_VIDEO_DECODER_DESC desc = { 0 };
            desc.Guid = guidCodec;
            desc.OutputFormat = AvPixelFormat2DXGIFormat(AV_PIX_FMT_NV12);
            desc.SampleWidth = format.format.width;
            desc.SampleHeight = format.format.height;

            uint32_t numDxvaConfigs = 0;
            hr = videoDevice->GetVideoDecoderConfigCount(&desc, &numDxvaConfigs);
            if (FAILED(hr) || numDxvaConfigs < 1)
                return false;

            uint32_t scoreMax = 0;
            for (uint32_t icfg = 0; icfg < numDxvaConfigs; ++icfg)
            {
                D3D11_VIDEO_DECODER_CONFIG config = {};
                hr = videoDevice->GetVideoDecoderConfig(&desc, icfg, &config);
                if (FAILED(hr))
                    return false;

                uint32_t score = 0;
                if (config.ConfigBitstreamRaw == 1)
                    score = 1;
                else if (format.codecId == AV_CODEC_ID_H264 && config.ConfigBitstreamRaw == 2)
                    score = 2;
                else
                    continue;

                if (IsEqualGUID(config.guidConfigBitstreamEncryption, DXVA2_NoEncrypt))
                    score += 16;

                if (score > scoreMax)
                    return true;
            }
            return false;
        }

        player::CodecLevel D3D11Device::testCodecLevel(const GUID & guidCodec) const
        {
            com_ptr<ID3D11VideoDevice> videoDevice = _device;
            if (!videoDevice)
                return {};

            HRESULT hr = S_OK;
            D3D11_VIDEO_DECODER_DESC desc = { 0 };
            desc.Guid = guidCodec;
            desc.OutputFormat = AvPixelFormat2DXGIFormat(AV_PIX_FMT_NV12);

            CodecLevel codecLevel = CodecLevelNone;
            for (int level = CodecLevelSD; level <= CodecLevel8K; ++level)
            {
                CodecSize size = CodecLevelToSize((CodecLevel)level);
                desc.SampleWidth = size.width;
                desc.SampleHeight = size.height;

                uint32_t numDxvaConfigs = 0;
                hr = videoDevice->GetVideoDecoderConfigCount(&desc, &numDxvaConfigs);
                if (FAILED(hr))
                    break;

                if (numDxvaConfigs < 1)
                    break;

                codecLevel = (CodecLevel)level;
            }
            return codecLevel;
        }
    }
}
