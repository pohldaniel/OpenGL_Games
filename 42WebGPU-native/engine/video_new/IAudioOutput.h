#pragma once
#include <vector>
#include <cstdint>

#include "../sound/AudioRingBuffer.h"

class IAudioOutput {

public:
    virtual ~IAudioOutput() = default;
    virtual bool init() = 0;
    virtual AudioRingBuffer& getRingBuffer() = 0;
    virtual void enqueueData(const std::vector<uint8_t>& pcmData) = 0;
    virtual void setVolume(float volume) = 0;
    virtual float getVolume() = 0;
};