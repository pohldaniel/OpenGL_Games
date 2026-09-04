#pragma once

#include <vector>
#include <cstdint>

class IAudioOutput {

public:
    virtual ~IAudioOutput() = default;
    virtual bool init() = 0;
    virtual void enqueueData(const std::vector<uint8_t>& pcmData) = 0;
    virtual void flush() = 0;
    virtual void setVolume(float volume) = 0;
    virtual float getVolume() = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;
};