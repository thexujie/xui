#include "stdafx.h"
#include "MMAudioPlayer.h"

#include "win32/windows.h"
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <functiondiscoverykeys.h>
#include <avrt.h>

// REFERENCE_TIME time units per second and per millisecond

namespace win32
{
    namespace mm
    {
        static void mm_audio_player_samples_buffer_free(void * /*opaque*/, uint8_t * data)
        {
            uint8_t ** ptr = reinterpret_cast<uint8_t **>(data);
            av_freep(ptr);
        }

        using namespace common;

        static const GUID SDL_KSDATAFORMAT_SUBTYPE_PCM = { 0x00000001, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };
        static const GUID SDL_KSDATAFORMAT_SUBTYPE_IEEE_FLOAT = { 0x00000003, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };

#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

        MMAudioPlayer::MMAudioPlayer()
        {
        }


        MMAudioPlayer::~MMAudioPlayer()
        {
            if (_thPlay.joinable())
            {
                _flags = _flags | common::flag_stop;
                _thPlay.join();
            }
        }

        std::vector<MMDeviceDesc> MMAudioPlayer::GetDeviceDescs() const
        {
            std::lock_guard<std::mutex> lock(const_cast<MMAudioPlayer *>(this)->_mtx);

            std::vector<MMDeviceDesc> descs;

            auto[err, enumerator] = const_cast<MMAudioPlayer *>(this)->initialEnumerator();
            if (err < 0)
                return { };

            com_ptr<IMMDeviceCollection> collection;
            HRESULT hr = enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &collection);
            if (FAILED(hr) || !collection)
                return descs;

            uint32_t total = 0;
            hr = collection->GetCount(&total);
            if (FAILED(hr) || !collection)
                return descs;

            MMDeviceDesc desc = {};
            for (uint32_t id = 0; id < total; ++id)
            {
                com_ptr<IMMDevice> device;
                hr = collection->Item(id, &device);
                if (FAILED(hr) || !device)
                    continue;

                auto [err, desc]= GetDeviceDesc(device);
                if (err)
                    continue;
                descs.push_back(desc);
            }

            return descs;
        }

        std::tuple<common::error_e, MMDeviceDesc> MMAudioPlayer::GetDeviceDesc(com_ptr<struct IMMDevice> device) const
        {
            if (!device)
                return {error_args, {}};

            HRESULT hr = S_OK;
            LPWSTR devid = NULL;
            hr = device->GetId(&devid);
            if (FAILED(hr))
                return { error_inner, {} };

            u8string strDevid = tools::string::ucs2_u8(devid, -1);
            CoTaskMemFree(devid);

            com_ptr<IPropertyStore> props;
            hr = device->OpenPropertyStore(STGM_READ, &props);
            if (FAILED(hr) || !props)
                return { error_inner, {} };

            PROPVARIANT var;
            PropVariantInit(&var);
            hr = props->GetValue(PKEY_Device_FriendlyName, &var);
            if (FAILED(hr))
            {
                PropVariantClear(&var);
                return { error_inner, {} };
            }

            u8string strFriendlyName = tools::string::ucs2_u8(var.pwszVal, -1);
            PropVariantClear(&var);

            MMDeviceDesc desc = {};
            desc.devid = strDevid;
            desc.friendlyName = strFriendlyName;

            //格式信息

            com_ptr<struct IAudioClient> audioClient;
            hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, reinterpret_cast<void **>(&audioClient));
            if (FAILED(hr) || !audioClient)
                return { error_inner, {} };

            WAVEFORMATEX * waveformat = nullptr;
            audioClient->GetMixFormat(&waveformat);
            if (FAILED(hr) || !waveformat)
                return { error_inner, {} };

            desc.format.chanels = waveformat->nChannels;
            desc.format.sampleRate = waveformat->nSamplesPerSec;
            if ((waveformat->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) && (waveformat->wBitsPerSample == 32))
                desc.format.sampleFormat = AV_SAMPLE_FMT_FLT;
            else if ((waveformat->wFormatTag == WAVE_FORMAT_PCM) && (waveformat->wBitsPerSample == 16))
                desc.format.sampleFormat = AV_SAMPLE_FMT_S16;
            else if ((waveformat->wFormatTag == WAVE_FORMAT_PCM) && (waveformat->wBitsPerSample == 32))
                desc.format.sampleFormat = AV_SAMPLE_FMT_S32;
            else if (waveformat->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
            {
                const WAVEFORMATEXTENSIBLE * ext = reinterpret_cast<const WAVEFORMATEXTENSIBLE *>(waveformat);
                if (ext->SubFormat == SDL_KSDATAFORMAT_SUBTYPE_IEEE_FLOAT && (waveformat->wBitsPerSample == 32))
                    desc.format.sampleFormat = AV_SAMPLE_FMT_FLT;
                else if (ext->SubFormat == SDL_KSDATAFORMAT_SUBTYPE_PCM && (waveformat->wBitsPerSample == 16))
                    desc.format.sampleFormat = AV_SAMPLE_FMT_S16;
                else if (ext->SubFormat == SDL_KSDATAFORMAT_SUBTYPE_PCM && (waveformat->wBitsPerSample == 32))
                    desc.format.sampleFormat = AV_SAMPLE_FMT_S32;
                else {}
            }
            else
                desc.format.sampleFormat = AV_SAMPLE_FMT_NONE;
            CoTaskMemFree(waveformat);

            return { error_ok, desc };
        }

        std::tuple<common::error_e, MMDeviceDesc> MMAudioPlayer::GetDefaultDeviceDesc() const
        {
            std::lock_guard<std::mutex> lock(const_cast<MMAudioPlayer *>(this)->_mtx);
            auto [err, enumerator] = const_cast<MMAudioPlayer *>(this)->initialEnumerator();
            if (err < 0)
                return { err, {} };
            com_ptr<IMMDevice> audioDevice;
            HRESULT hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &audioDevice);
            if (FAILED(hr) || !audioDevice)
                return { error_inner, {} };
            return GetDeviceDesc(audioDevice);
        }

        common::error_e MMAudioPlayer::Start()
        {
            _flags &= ~common::flag_pause;
            if (!_thPlay.joinable())
                _thPlay = std::thread(std::bind(&MMAudioPlayer::playThread, this));
            _cond.notify_all();
            return error_ok;
        }

        common::error_e MMAudioPlayer::Pause()
        {
            if (_thPlay.joinable())
                _flags = _flags | common::flag_pause;
            return error_ok;
        }

        common::error_e MMAudioPlayer::Clear()
        {
            std::lock_guard<std::mutex> lock(_mtx);
            _playItems.clear();
            return error_ok;
        }

        common::error_e MMAudioPlayer::Stop()
        {
            if (_thPlay.joinable())
                _flags = _flags | common::flag_stop;
            return error_ok;
        }

        common::error_e MMAudioPlayer::Join()
        {
            if (_thPlay.joinable())
            {
                _flags = _flags | common::flag_stop;
                _thPlay.join();
            }
            return error_ok;
        }

        std::tuple<common::error_e, std::shared_ptr<player::Clock>> MMAudioPlayer::AddAudio(std::shared_ptr<player::IAudioBufferInputStream> stream)
        {
            std::lock_guard<std::mutex> lock(_mtx);
            std::shared_ptr<player::Clock> clock = std::make_shared<player::Clock>();
            _playItems.push_back({0, _itemIndex++, stream, clock });
            _cond.notify_all();
            return { error_ok, clock };
        }

        common::error_e MMAudioPlayer::SetVolume(float_t volume)
        {
            _volume = volume;
            return error_ok;
        }

        std::tuple<common::error_e, com_ptr<IMMDeviceEnumerator>> MMAudioPlayer::initialEnumerator()
        {
            if (!_enumerator)
            {
                HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void **>(&_enumerator));
                if (FAILED(hr) || !_enumerator)
                    return { error_inner, nullptr };
            }
            return { error_ok, _enumerator };
        }

        common::error_e MMAudioPlayer::resetDevice()
        {
            logger::inf() << __FUNCTION__;
            _audioClient->Reset();
            _audioRenderClient.reset();
            _audioClient.reset();
            _audioDevice.reset();
            _state = initialDevice();
            if (_state < 0)
                return _state;
            return error_ok;
        }

        common::error_e MMAudioPlayer::initialDevice()
        {
            auto[err_enum, enumerator] = const_cast<MMAudioPlayer *>(this)->initialEnumerator();
            if (err_enum < 0)
                return err_enum;

            com_ptr<IMMDevice> audioDevice;
            HRESULT hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &audioDevice);
            if (FAILED(hr) || !audioDevice)
                return error_generic;

            com_ptr<struct IAudioClient> audioClient;
            hr = audioDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, reinterpret_cast<void **>(&audioClient));
            if (FAILED(hr) || !audioClient)
                return error_generic;

            WAVEFORMATEX * __waveformat = nullptr;
            audioClient->GetMixFormat(&__waveformat);
            if (FAILED(hr) || !__waveformat)
                return error_generic;
            std::unique_ptr<WAVEFORMATEX, decltype(CoTaskMemFree)*> waveformat(__waveformat, CoTaskMemFree);

            int64_t durationDefault = REFTIMES_PER_SEC / 20;
            int64_t durationMin = REFTIMES_PER_SEC / 20;

            hr = audioClient->GetDevicePeriod(&durationDefault, &durationMin);
            if (FAILED(hr) || durationMin <= 0)
                return error_generic;

            // 使用默认的 duration 可以获得最高的性价比
            // 使用 durationMin 可能会导致声音断断续续（因为 Sleep）。
            int64_t duration = durationDefault;
            uint32_t audioClientFlags = /*AUDCLNT_STREAMFLAGS_EVENTCALLBACK*/0;
            hr = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, audioClientFlags, duration, 0, waveformat.get(), NULL);
            if (FAILED(hr))
                return error_generic;

            uint32_t numBufferedSamples = 0;
            hr = audioClient->GetBufferSize(&numBufferedSamples);
            if (FAILED(hr) || numBufferedSamples <= 0)
                return error_generic;

            com_ptr<struct IAudioRenderClient> audioRenderClient;
            hr = audioClient->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void**>(&audioRenderClient));
            if (FAILED(hr) || !audioRenderClient)
                return error_generic;

            hr = audioClient->Start();
            if (FAILED(hr))
                return error_generic;

            AVSampleFormat sampleFormat = AV_SAMPLE_FMT_NONE;
            if ((waveformat->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) && (waveformat->wBitsPerSample == 32))
                sampleFormat = AV_SAMPLE_FMT_FLT;
            else if ((waveformat->wFormatTag == WAVE_FORMAT_PCM) && (waveformat->wBitsPerSample == 16))
                sampleFormat = AV_SAMPLE_FMT_S16;
            else if ((waveformat->wFormatTag == WAVE_FORMAT_PCM) && (waveformat->wBitsPerSample == 32))
                sampleFormat = AV_SAMPLE_FMT_S32;
            else if (waveformat->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
            {
                const WAVEFORMATEXTENSIBLE * ext = reinterpret_cast<const WAVEFORMATEXTENSIBLE *>(waveformat.get());
                if (ext->SubFormat == SDL_KSDATAFORMAT_SUBTYPE_IEEE_FLOAT && (waveformat->wBitsPerSample == 32))
                    sampleFormat = AV_SAMPLE_FMT_FLT;
                else if (ext->SubFormat == SDL_KSDATAFORMAT_SUBTYPE_PCM && (waveformat->wBitsPerSample == 16))
                    sampleFormat = AV_SAMPLE_FMT_S16;
                else if (ext->SubFormat == SDL_KSDATAFORMAT_SUBTYPE_PCM && (waveformat->wBitsPerSample == 32))
                    sampleFormat = AV_SAMPLE_FMT_S32;
                else {}
            }

            _numBufferedSamples = numBufferedSamples;
            _audioDevice = audioDevice;
            _audioClient = audioClient;
            _audioRenderClient = audioRenderClient;

            _format = { waveformat->nChannels, 0, (int32_t)waveformat->nSamplesPerSec, sampleFormat };

            auto [err_desc, desc] = GetDeviceDesc(audioDevice);
            logger::inf() << __FUNCTION__" " << desc.friendlyName << ", "
                //<< desc.devid << ", "
                << av_get_sample_fmt_name(_format.sampleFormat) << ", "
                << _format.chanels << " chanels, "
                << _format.sampleRate << "Hz, "
                << 1000.0f * numBufferedSamples / _format.sampleRate / _format.chanels << "ms buffer";
            return error_ok;
        }

        void MMAudioPlayer::playThread()
        {
            logger::act() << __FUNCTION__;
            common::thread_set_name(0, "MMAudioPlayer::playThread");

            _state = initialDevice();
            if (_state < 0)
                return;

            //废物，偶尔会一点作用都没有
            //DWORD taskIndex = 0;
            //HANDLE hTask = AvSetMmThreadCharacteristics(TEXT("Pro Audio"), &taskIndex);
            common::thread_set_priority(thread_priority_highest);

            HRESULT hr = S_OK;

            bool needReset = false;
            std::list<playItemT> playItems;
            while (_state >= 0 && !(_flags & common::flag_stop))
            {
                if (_flags & common::flag_pause)
                {
                    std::unique_lock<std::mutex> lock(_mtx);
                    if (_flags & common::flag_pause)
                    {
                        _cond.wait(lock, [this]() {return (_flags & common::flag_stop) || !(_flags & common::flag_pause); });
                        continue;
                    }
                }

                {
                    //获取列表
                    std::unique_lock<std::mutex> lock(_mtx, std::try_to_lock);
                    if (lock.owns_lock())
                    {
                        for (auto & item : _playItems)
                        {
                            _state = item.stream->SetOutputFormat(_format);
                            if (_state < 0)
                                break;
                        }
                        playItems.splice(playItems.end(), _playItems);

                        if (playItems.empty())
                        {
                            _cond.wait(lock, [this]() {return !_playItems.empty() || (_flags & common::flag_stop); });
                            continue;
                        }
                    }
                }

                auto iter = playItems.begin();
                while (iter != playItems.end())
                {
                    if (iter->state < 0)
                        iter = playItems.erase(iter);
                    else
                    {
                        if (iter->packetIndex <= 0)
                        {
                            _state = iter->stream->SetOutputFormat(_format);
                            if (_state < 0)
                            {
                                iter = playItems.erase(iter);
                                continue;
                            }
                        }
                        ++iter;
                    }
                }

                if (needReset)
                {
                    needReset = false;
                    player::AudioFormat oldFormat = _format;
                    resetDevice();

                    std::shared_ptr<SwrContext> _swr(swr_alloc(), [](SwrContext * ptr) { swr_free(&ptr); });
                    int64_t in_channel_layout = oldFormat.chanelLayout ? oldFormat.chanelLayout : av_get_default_channel_layout(oldFormat.chanels);
                    av_opt_set_int(_swr.get(), "in_channel_layout", in_channel_layout, 0);
                    av_opt_set_int(_swr.get(), "in_sample_rate", oldFormat.sampleRate, 0);
                    av_opt_set_sample_fmt(_swr.get(), "in_sample_fmt", oldFormat.sampleFormat, 0);

                    int64_t out_channel_layout = _format.chanelLayout ? _format.chanelLayout : av_get_default_channel_layout(_format.chanels);
                    av_opt_set_int(_swr.get(), "out_channel_layout", out_channel_layout, 0);
                    av_opt_set_int(_swr.get(), "out_sample_rate", _format.sampleRate, 0);
                    av_opt_set_sample_fmt(_swr.get(), "out_sample_fmt", _format.sampleFormat, 0);

                    int averr = swr_init(_swr.get());
                    if (averr < 0)
                    {
                        _state = error_state;
                        break;
                    }

                    uint32_t dstBytesPerSample = av_get_bytes_per_sample(_format.sampleFormat);
                    for (auto & item : _playItems)
                    {
                        //item.buffer = {};
                        //item.stream->SetOutputFormat(_format);
                        ////item.clock->souceIndex = 0;
                        ////item.clock->pts = std::nan(0);
                        ////item.state = error_ok;
                        ////item.numSamplesRendered = 0;
                        ////item.ptsRendered = 0;
                        //continue;
                        //item.stream->Clear();

                        item.ptsRendered += (double_t)item.numSamplesRendered / oldFormat.sampleRate;
                        //printf("item[%lld] discard %lld samples, %lf ms.\n", item.packetIndex, 
                        //    item.buffer.numSamples - item.buffer.numSamplesRead, 
                        //    1000.0 * (item.buffer.numSamples - item.buffer.numSamplesRead) / oldSampleRate);

                        int64_t numSamplesUnread = item.buffer.numSamples - item.numSamplesRendered;
                        int32_t numSamplesMax = swr_get_out_samples(_swr.get(), numSamplesUnread);

                        player::AudioBuffer newBuffer;
                        newBuffer.bufferIndex = item.buffer.bufferIndex;
                        std::shared_ptr<AVFrame> avframe2(av_frame_alloc(), [](AVFrame * ptr) { av_frame_free(&ptr); });
                        avframe2->channels = _format.chanels;
                        avframe2->channel_layout = _format.chanelLayout;
                        avframe2->sample_rate = _format.sampleRate;
                        avframe2->format = _format.sampleFormat;
                        avframe2->pts = item.buffer.avframe->pts;
                        avframe2->pkt_dts = item.buffer.avframe->pkt_dts;
                        av_samples_alloc(avframe2->data, avframe2->linesize, _format.chanels, numSamplesMax, _format.sampleFormat, 1);
                        avframe2->buf[0] = av_buffer_create((uint8_t *)(&(avframe2->data[0])), 0, mm_audio_player_samples_buffer_free, nullptr, AV_BUFFER_FLAG_READONLY);

                        int32_t numSamples = 0;
                        while (true)
                        {
                            uint8_t * dst[AV_NUM_DATA_POINTERS] = {};
                            for (int cnt = 0; cnt < AV_NUM_DATA_POINTERS; ++cnt)
                            {
                                if (avframe2->data[cnt])
                                    dst[cnt] = avframe2->data[cnt] + numSamples * dstBytesPerSample;
                            }

                            if (!numSamples)
                            {
                                uint32_t srcBytesPerSample = av_get_bytes_per_sample(oldFormat.sampleFormat);
                                uint8_t * src[AV_NUM_DATA_POINTERS] = {};
                                for (int cnt = 0; cnt < AV_NUM_DATA_POINTERS; ++cnt)
                                {
                                    if (item.buffer.avframe->data[cnt])
                                        src[cnt] = item.buffer.avframe->data[cnt] + item.numSamplesRendered * srcBytesPerSample;
                                }
                                averr = swr_convert(_swr.get(), dst, (int32_t)(numSamplesMax - numSamples), (const uint8_t **)src, numSamplesUnread);
                            }
                            else
                                averr = swr_convert(_swr.get(), dst, (int32_t)(numSamplesMax - numSamples), 0, 0);

                            if (averr < 0)
                                break;

                            if (averr == 0)
                                break;

                            numSamples += averr;
                        }

                        if (averr < 0)
                        {
                            _state = error_state;
                            break;
                        }

                        avframe2->nb_samples = numSamples;
                        newBuffer.avframe = avframe2;
                        newBuffer.numSamples = numSamples;

                        item.buffer = newBuffer;
                        item.numSamplesRendered = 0;
                        item.stream->SetOutputFormat(_format);
                    }
                }

                float_t volume = std::clamp(_volume.load(), 0.0f, 10.0f);
                int64_t bytesPerSample = av_get_bytes_per_sample(_format.sampleFormat);
                int64_t bytesPerSampleXChanels = bytesPerSample * _format.chanels;

                int64_t numSamplesTotal = _numBufferedSamples / 2;
                uint8_t * mmBuffer = nullptr;
                hr = _audioRenderClient->GetBuffer((uint32_t)numSamplesTotal, &mmBuffer);
                if (hr == AUDCLNT_E_DEVICE_INVALIDATED)
                {
                    needReset = true;
                    continue;
                }

                if (FAILED(hr))
                    continue;

                // 超过 1 个音频源就需要进行混合，先清空
                // 但是，还是为了防止 pendding 时无数据造成噪音，清空算了
                //if (playItems.size() > 1)
                memset(mmBuffer, 0, numSamplesTotal * bytesPerSampleXChanels);

                uint32_t numSamplesPaddingCurr = 0;
                _audioClient->GetCurrentPadding(&numSamplesPaddingCurr);
                for (auto & item : playItems)
                {
                    int64_t numSamplesRenderd = 0;
                    while (numSamplesRenderd < numSamplesTotal && item.state >= 0)
                    {
                        int64_t numSamplesNeeded = numSamplesTotal - numSamplesRenderd;
                        int64_t numSamplesHave = item.buffer.numSamples - item.numSamplesRendered;
                        if (numSamplesHave <= 0)
                        {
                            item.buffer = {};
                            item.numSamplesRendered = 0;
                            item.ptsRendered = 0;
                            item.state = item.stream->PeekBuffer(item.buffer);
                            //common::error_e state = _decoder->NextBuffer(buffer, std::numeric_limits<uint32_t>::max());
                            if (item.state == error_pendding)
                            {
                                //logger::inf() << __FUNCTION__" auduio frame pendding " << _pendingCountTotal++ << "...";
                                break;
                            }

                            if (item.state < 0)
                                break;

                            if(!item.buffer.avframe)
                            {
                                item.clock->pts = 0;
                                item.numSamplesRendered = 0;
                                item.ptsRendered = 0;
                                continue;
                            }

                            numSamplesHave = item.buffer.numSamples;
                            ++item.packetIndex;
                        }

                        int64_t numSamples = numSamplesHave < numSamplesNeeded ? numSamplesHave : numSamplesNeeded;
                        if (numSamples > 0)
                        {
                            uint8_t * dst = mmBuffer + (numSamplesRenderd * bytesPerSampleXChanels);
                            uint8_t * src = (uint8_t *)item.buffer.avframe->data[0] + (item.numSamplesRendered * bytesPerSampleXChanels);
                            if (playItems.size() < 2)
                            {
                                //if(std::round(volume * 1000) == 1000)
                                //    memcpy(dst, src, numSamples * bytesPerSampleXChanels);
                                //else
                                {
                                    for (uint32_t isam = 0; isam < numSamples * _format.chanels; ++isam)
                                    {
                                        float_t * dstF = (float_t *)(dst + isam * bytesPerSample);
                                        float_t * srcF = (float_t *)(src + isam * bytesPerSample);
                                        *dstF = *srcF * volume;
                                    }
                                }
                            }
                            else
                            {
                                // 混合
                                switch (_format.sampleFormat)
                                {
                                case AV_SAMPLE_FMT_FLT:
                                    for (int64_t isam = 0; isam < numSamples * _format.chanels; ++isam)
                                    {
                                        float_t * dstF = (float_t *)(dst + isam * bytesPerSample);
                                        float_t * srcF = (float_t *)(src + isam * bytesPerSample);
                                        *dstF += *srcF * volume;
                                    }

                                    //for (uint32_t isam = 0; isam < numSamplesTotal * _format.chanels; ++isam)
                                    //{
                                    //    float_t * dstF = (float_t *)(mmBuffer + isam * bytesPerSample);
                                    //    *dstF /= playItems.size();
                                    //}
                                    break;
                                default:
                                    break;
                                }
                            }
                            numSamplesRenderd += numSamples;
                            item.numSamplesRendered += numSamples;
                        }
                    }

                    // Rendered 仅仅是加入了缓存，并没有被送入 EndPoint，即没有被播放，numSamplesPaddingCurr 记录了当前等待被播放的样本数
                    // 考虑减去这些样本数，以获得更加精准的 pts
                    item.clock->souceIndex = item.buffer.souceIndex;
                    item.clock->timelineIndex = item.buffer.timelineIndex;
                    item.clock->pts = item.buffer.pts + (double_t)(item.numSamplesRendered - numSamplesPaddingCurr) / _format.sampleRate;
                    //item.clock->pts = item.buffer.pts + (double_t)item.numSamplesRendered / _format.sampleRate;
                }

                needReset = false;

                if (!mmBuffer)
                    continue;

                hr = _audioRenderClient->ReleaseBuffer((uint32_t)numSamplesTotal, 0);
                if (hr == AUDCLNT_E_DEVICE_INVALIDATED)
                {
                    needReset = true;
                    continue;
                }

                if (FAILED(hr))
                {
                    logger::err() << __FUNCTION__" _audioRenderClient->ReleaseBuffer " << std::hex << hr;
                    break;
                }

                uint32_t numSamplesPadding = 0;
                while (_state >= 0 && !(_flags & common::flag_stop))
                {
                    hr = _audioClient->GetCurrentPadding(&numSamplesPadding);
                    if (hr == AUDCLNT_E_DEVICE_INVALIDATED)
                    {
                        needReset = true;
                        break;
                    }
                    if (FAILED(hr))
                    {
                        _state = error_generic;
                        break;
                    }

                    if (numSamplesPadding <= numSamplesTotal)
                        break;

                    int64_t nmsSleep = ((numSamplesPadding - numSamplesTotal) * 1000) / _format.sampleRate;
                    Sleep((uint32_t)nmsSleep);
                }
            }

            logger::act() << __FUNCTION__" end with " << _state;
        }
    }
}
