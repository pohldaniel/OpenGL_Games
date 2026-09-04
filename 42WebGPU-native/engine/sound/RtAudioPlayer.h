#pragma once

#include <vector>
#include <RtAudio.h>
#include "AudioDecoder.h"
#include "AudioRingBuffer.h"
#include "SoftwareMixer.h"
#include "IAudioOutput.h"

class RtAudioPlayer : public IAudioOutput {

    friend int rtaudio_callback_wrapper(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void* userData);

public:

    RtAudioPlayer();
    ~RtAudioPlayer();

    bool init() override;
    void enqueueData(const std::vector<uint8_t>& pcmData) override;
    void pause() override;
    void resume() override;
    void setVolume(float volume) override;
    float getVolume() override;

    SoftwareMixer& getMixer();
   
private:

    void flush() override;
    int audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status);
  
    RtAudio dac;
    AudioDecoder decoder;
    AudioRingBuffer m_ringBuffer;
    bool isPlaying = false;

    SoftwareMixer m_softwareMixer;
    std::vector<uint8_t> m_audioAccumulator;
};