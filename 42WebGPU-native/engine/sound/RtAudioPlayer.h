#pragma once
#include <RtAudio.h>
#include "AudioDecoder.h"
#include "AudioRingBuffer.h" // Der exakt gleiche Ringbuffer aus dem Oboe-Beispiel
#include <vector>
#include "SoftwareMixer.h"
#include "../video_new/IAudioOutput.h"

class RtAudioPlayer : public IAudioOutput {
public:
    RtAudioPlayer();
    ~RtAudioPlayer();

    bool init() override;
    void play(const std::string& filename);
    void update();
    SoftwareMixer& getMixer();
    AudioRingBuffer& getRingBuffer() override;
    void startHardwareStream();

    int audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status);

    void enqueueData(const std::vector<uint8_t>& pcmData) override;

    void setVolume(float volume) override {
        getMixer().setVolume(volume);
    }

    float getVolume() override {
        return getMixer().getVolume();
    }

   

private:
    RtAudio dac;
    AudioDecoder decoder;
    AudioRingBuffer m_ringBuffer;
    bool isPlaying = false;

    SoftwareMixer m_softwareMixer;
};