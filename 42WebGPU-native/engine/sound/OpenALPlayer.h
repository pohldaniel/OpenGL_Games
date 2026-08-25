#pragma once
#include <algorithm>
#include <AL/al.h>
#include <AL/alc.h>
#include <iostream>
#include "AudioDecoder.h"
#include "../video_new/IAudioOutput.h"

class OpenALPlayer : public IAudioOutput {
public:
    OpenALPlayer();
    ~OpenALPlayer();

    bool init() override;
    void play(const std::string& filename);
    void update();

    void streamMovieAudio(const std::vector<uint8_t>& pcmData);
    void prepareForMovie();


    void enqueueData(const std::vector<uint8_t>& pcmData) override;

    void setVolume(float volume) override {
        if (m_source) {
            alSourcef(m_source, AL_GAIN, std::clamp(volume, 0.0f, 1.0f));
        }
    }

    float getVolume() override {
        ALfloat volume;
        alGetSourcef(m_source, AL_GAIN, &volume);
        return volume;
    }

    void pause() override {
        alSourcePause(m_source);
    }

    void resume() override {
        ALint state;
        alGetSourcei(m_source, AL_SOURCE_STATE, &state);

        if(state == AL_PAUSED) 
            alSourcePlay(m_source);      
    }

    AudioRingBuffer& getRingBuffer() override {
        return m_ringBuffer;
    }

private:
    
    ALuint m_source = 0;
    ALuint m_buffers[2] = { 0, 0 };

    AudioDecoder m_decoder;
    bool m_isPlaying = false;

    void streamBuffer(ALuint bufferId);
    AudioRingBuffer m_ringBuffer;
    std::vector<uint8_t> m_audioAccumulator;
};