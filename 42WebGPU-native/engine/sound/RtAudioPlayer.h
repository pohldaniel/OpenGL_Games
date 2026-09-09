#pragma once

#include <vector>
#include <RtAudio.h>

#include "AudioRingBuffer.h"
#include "SoftwareMixer.h"
#include "IAudioOutput.h"

class RtAudioPlayer : public IAudioOutput {

public:

    RtAudioPlayer();
    ~RtAudioPlayer();

    bool init() override;
    void enqueueData(const std::vector<float>& pcmData) override;
    void pause() override;
    void resume() override;
    void setVolume(float volume) override;
    float getVolume() override;

    SoftwareMixer& getMixer();

private:

    void flush() override;
    int audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status);
  
    RtAudio m_dac;
    AudioRingBuffer m_ringBuffer;

    SoftwareMixer m_softwareMixer;
    std::vector<float> m_accumulator;
    
    static int RtAudioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void* userData);
};