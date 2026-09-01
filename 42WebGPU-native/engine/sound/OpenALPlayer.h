#pragma once
#include <algorithm>
#include <AL/al.h>
#include <AL/alc.h>
#include <iostream>

#include "IAudioOutput.h"

class OpenALPlayer : public IAudioOutput {

    friend class AudioDecoder;

public:

    OpenALPlayer();
    ~OpenALPlayer();

    bool init() override;
    void enqueueData(const std::vector<uint8_t>& pcmData) override;
    void pause() override;
    void resume() override;

    void setVolume(float volume) override;
    float getVolume() override;

private:
    
    void flush() override;

    ALuint m_source = 0u;
    ALuint m_buffers[2] = { 0u, 0u };

    bool m_isPlaying = false;


    std::vector<uint8_t> m_audioAccumulator;
};