#pragma once

struct IMMDeviceEnumerator;
struct IMMDevice;
struct IAudioClient;
struct IAudioRenderClient;

#include "player/media.h"
#include "win32/com_ptr.h"

namespace win32
{
    namespace mm
    {
        struct MMDeviceDesc
        {
            u8string devid;
            u8string friendlyName;
            player::AudioFormat format;
        };

        class MMAudioPlayer : public player::IAudioPlayer
        {
        public:
            MMAudioPlayer();
            ~MMAudioPlayer();

            std::vector<MMDeviceDesc> GetDeviceDescs() const;
            std::tuple<common::error_e, MMDeviceDesc> GetDeviceDesc(com_ptr<struct IMMDevice> device) const;
            std::tuple<common::error_e, MMDeviceDesc> GetDefaultDeviceDesc() const;
            common::error_e Start();
            common::error_e Pause();
            common::error_e Stop();
            common::error_e Clear();
            common::error_e Join();

            std::tuple<common::error_e, std::shared_ptr<player::Clock>> AddAudio(std::shared_ptr<player::IAudioBufferInputStream> stream);

            common::error_e SetVolume(float_t volume);

        private:
            std::tuple<common::error_e, com_ptr<IMMDeviceEnumerator>> initialEnumerator();
            common::error_e resetDevice();
            common::error_e initialDevice();
            void playThread();

        private:
            std::thread _thPlay;

            struct playItemT
            {
                int64_t packetIndex = 0;
                int64_t itemIndex = 0;
                std::shared_ptr<player::IAudioBufferInputStream> stream;
                std::shared_ptr<player::Clock> clock;

                player::AudioBuffer buffer;
                common::error_e state = common::error_ok;

                int64_t numSamplesRendered = 0;
                double_t ptsRendered = 0;
            };

            std::mutex _mtx;
            std::condition_variable _cond;

            std::atomic<float_t> _volume = 1.0;
            int64_t _itemIndex = 0;
            std::list<playItemT> _playItems;
            com_ptr<IMMDeviceEnumerator> _enumerator;
            com_ptr<IMMDevice> _audioDevice;
            com_ptr<IAudioClient> _audioClient;
            com_ptr<IAudioRenderClient> _audioRenderClient;

            int64_t _numBufferedSamples = 0;
            std::vector<uint8_t> _audioBuffers;
            player::AudioFormat _format;

            std::atomic<common::error_e> _state = common::error_ok;
            std::atomic<int32_t> _flags = common::flag_none;
            std::atomic<double_t> _pts = 0;

            std::shared_ptr<SwrContext> _swr;

            int64_t _pendingCountTotal = 0;
        };

    }
}
