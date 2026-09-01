#include <iostream>
#include "OpenALPlayer.h"

OpenALPlayer::OpenALPlayer() {
 
}

OpenALPlayer::~OpenALPlayer() {
    if (m_source) {
        alSourceStop(m_source);
        alDeleteSources(1, &m_source);
    }
    if (m_buffers[0]) 
        alDeleteBuffers(2, m_buffers);
    
}

bool OpenALPlayer::init() {
    alGenSources(1, &m_source);
    alGenBuffers(2, m_buffers);
    alSourcef(m_source, AL_GAIN, 0.5f);
    return true;
}

void OpenALPlayer::enqueueData(const std::vector<uint8_t>& pcmData) {
    if (m_source == 0) return;

    if (!pcmData.empty()) {
        m_audioAccumulator.insert(m_audioAccumulator.end(), pcmData.begin(), pcmData.end());
    }

    if (m_audioAccumulator.size() < 4096) {
        return;
    }

    ALint processed = 0;
    alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processed);

    if (processed > 0) {
        ALuint unqueued;
        alSourceUnqueueBuffers(m_source, 1, &unqueued);

        alBufferData(unqueued, AL_FORMAT_STEREO16, m_audioAccumulator.data(), m_audioAccumulator.size(), 44100);
        alSourceQueueBuffers(m_source, 1, &unqueued);

        m_audioAccumulator.clear();
    }else {
        ALint queued = 0;
        alGetSourcei(m_source, AL_BUFFERS_QUEUED, &queued);

        if (queued < 2) {
            ALuint targetBuffer = m_buffers[queued];
            alBufferData(targetBuffer, AL_FORMAT_STEREO16, m_audioAccumulator.data(), m_audioAccumulator.size(), 44100);
            alSourceQueueBuffers(m_source, 1, &targetBuffer);

            m_audioAccumulator.clear();
        }
    }

    ALint state;
    alGetSourcei(m_source, AL_SOURCE_STATE, &state);
    if (state != AL_PLAYING) {
        alSourcePlay(m_source);
    }
}

void OpenALPlayer::pause() {
    alSourcePause(m_source);
}

void OpenALPlayer::resume() {
    ALint state;
    alGetSourcei(m_source, AL_SOURCE_STATE, &state);

    if (state == AL_PAUSED)
        alSourcePlay(m_source);
}

void OpenALPlayer::setVolume(float volume) {
    if (m_source) {
        alSourcef(m_source, AL_GAIN, std::clamp(volume, 0.0f, 1.0f));
    }
}

float OpenALPlayer::getVolume() {
    ALfloat volume;
    alGetSourcef(m_source, AL_GAIN, &volume);
    return volume;
}

void OpenALPlayer::flush() {
    alSourceStop(m_source);
    alSourcei(m_source, AL_BUFFER, AL_NONE);
    alSourceRewind(m_source);
    m_audioAccumulator.clear();
}