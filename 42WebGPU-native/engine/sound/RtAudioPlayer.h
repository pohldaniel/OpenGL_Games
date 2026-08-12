#pragma once
#include <RtAudio.h>
#include "AudioDecoder.h"
#include "AudioRingBuffer.h" // Der exakt gleiche Ringbuffer aus dem Oboe-Beispiel
#include <vector>
#include "SoftwareMixer.h"

class RtAudioPlayer {
public:
    RtAudioPlayer();
    ~RtAudioPlayer();

    bool init();
    void play(const std::string& filename);
    void update();
    SoftwareMixer& getMixer();
    AudioRingBuffer& getRingBuffer();
    void startHardwareStream();

    int audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status);

private:
    RtAudio dac;
    AudioDecoder decoder;
    AudioRingBuffer ringBuffer;
    bool isPlaying = false;

    SoftwareMixer m_softwareMixer;
};