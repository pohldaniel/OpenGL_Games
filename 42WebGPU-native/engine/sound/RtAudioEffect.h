#pragma once

#include <vector>
#include <RtAudio.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
}

#include "AudioRingBuffer.h"
#include "SoftwareMixer.h"
#include "ISoundEffect.h"
#include "Cache.h"

class RtAudioEffect : public ISoundEffect {

    struct CacheEntry {      
        CacheEntry(const std::string& file);
        ~CacheEntry();

        CacheEntry(const CacheEntry&) = delete;
        CacheEntry& operator=(const CacheEntry&) = delete;

        CacheEntry(CacheEntry&& other) noexcept;
        CacheEntry& operator=(CacheEntry&& other) noexcept;

        std::vector<int16_t> m_samples;
        uint32_t m_totalSamples;
    };

public:

    RtAudioEffect();
    ~RtAudioEffect();

    void init() override;
    void play(const std::string& file) override;
    void resume();

    SoftwareMixer& getMixer();

private:

    int audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status);

    RtAudio m_dac;
    AudioRingBuffer m_ringBuffer;
    SoftwareMixer m_softwareMixer;

    static int RtAudioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void* userData);

    static CacheLRU<std::string, RtAudioEffect::CacheEntry> Cache;
};