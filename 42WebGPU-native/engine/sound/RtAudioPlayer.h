#pragma once

#include <vector>
#include <RtAudio.h>
#include "AudioDecoder.h"
#include "AudioRingBuffer.h"
#include "SoftwareMixer.h"
#include "IAudioOutput.h"

class RtAudioPlayer : public IAudioOutput {
public:
    RtAudioPlayer();
    ~RtAudioPlayer();

    bool init() override;
    //void play(const std::string& filename);
    //void update();
    SoftwareMixer& getMixer();

    //void startHardwareStream();

    int audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status);

    void enqueueData(const std::vector<uint8_t>& pcmData) override;

    void setVolume(float volume) override {
        getMixer().setVolume(volume);
    }

    float getVolume() override {
        return getMixer().getVolume();
    }

    void pause() override {
        if(dac.isStreamRunning())
            dac.stopStream();
    }

    void resume() override {
        if(!dac.isStreamRunning()) 
            dac.startStream();
    }
    void flush() override {}
private:
    RtAudio dac;
    AudioDecoder decoder;
    AudioRingBuffer m_ringBuffer;
    bool isPlaying = false;

    SoftwareMixer m_softwareMixer;
};