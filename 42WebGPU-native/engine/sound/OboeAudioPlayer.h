#pragma once
#include <oboe/Oboe.h>
#include "AudioDecoder.h"
#include "SoftwareMixer.h"
#include <vector>
#include <atomic>
#include <memory>
#include "AudioRingBuffer.h"


class OboeAudioPlayer : public oboe::AudioStreamDataCallback {
public:
    OboeAudioPlayer();
    ~OboeAudioPlayer();

    bool init();
    void playMusic(const std::string& filename);
    void update(); // Läuft single-threaded in deiner Gameloop

    SoftwareMixer& getMixer() { return m_mixer; }

    // Oboes nativer Hardware-Callback (Audio-Echtzeit-Thread)
    oboe::DataCallbackResult onAudioReady(
        oboe::AudioStream *audioStream, 
        void *audioData, 
        int32_t numFrames) override;

private:
    std::shared_ptr<oboe::AudioStream> stream;
    AudioDecoder decoder;
    AudioRingBuffer ringBuffer;
    SoftwareMixer m_mixer;
    bool m_isMusicPlaying = false;
};