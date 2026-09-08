#pragma once

#include <vector>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
}

#include <AL/al.h>
#include <AL/alext.h>

#include "ISoundEffect.h"
#include "Cache.h"

class OpenALEffect : public ISoundEffect {

    struct CacheEntry {      
        CacheEntry(const std::string& file);
        ~CacheEntry();

        CacheEntry(const CacheEntry&) = delete;
        CacheEntry& operator=(const CacheEntry&) = delete;

        CacheEntry(CacheEntry&& other) noexcept;
        CacheEntry& operator=(CacheEntry&& other) noexcept;

        ALuint buffer;
    };

public:

    OpenALEffect();
    ~OpenALEffect();

    void init() override;
    void play(const std::string& file) override;
    void setVolume(float volume);

private:

    std::vector<ALuint> m_sources;
    size_t m_next;

    static CacheLRU<std::string, OpenALEffect::CacheEntry> Cache;
};