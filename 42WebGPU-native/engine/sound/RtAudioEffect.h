#pragma once

#include <vector>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
}


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


    };

public:

    RtAudioEffect();
    ~RtAudioEffect();

    void init() override;
    void play(const std::string& file) override;

private:


    size_t m_next;

    static CacheLRU<std::string, RtAudioEffect::CacheEntry> Cache;
};