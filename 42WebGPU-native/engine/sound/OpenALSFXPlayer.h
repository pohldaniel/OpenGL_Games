#pragma once
#include <AL/al.h>
#include <vector>
#include <unordered_map>

#include "SoundCache.h"

class OpenALSFXPlayer {
public:
    void init(int maxChannels = 32) {
        m_sources.resize(maxChannels);
        alGenSources(maxChannels, m_sources.data());
    }

    void play(const std::string& file) {
        SoundEffect& sfx = SFXCache::get(file);
        ALuint alBuffer = getOrCreateBuffer(file, sfx);

        // Finde eine Source, die gerade NICHT spielt
        ALuint sourceToUse = m_sources[m_nextSourceIndex];
        for (ALuint source : m_sources) {
            ALint state;
            alGetSourcei(source, AL_SOURCE_STATE, &state);
            if (state != AL_PLAYING) {
                sourceToUse = source;
                break;
            }
        }

        // Sound abspielen (schneidet ggf. die alte Source ab, falls alle voll sind)
        alSourceStop(sourceToUse);
        alSourcei(sourceToUse, AL_BUFFER, alBuffer);
        alSourcePlay(sourceToUse);

        // Ring-Index weiterschalten als Fallback
        m_nextSourceIndex = (m_nextSourceIndex + 1) % m_sources.size();
    }

private:
    std::vector<ALuint> m_sources;
    std::unordered_map<std::string, ALuint> m_buffers;
    size_t m_nextSourceIndex = 0;

    ALuint getOrCreateBuffer(const std::string& file, const SoundEffect& sfx) {
        if (m_buffers.find(file) == m_buffers.end()) {
            ALuint buf;
            alGenBuffers(1, &buf);

            ALenum format = (sfx.channels == 2) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;

            // KORREKTUR: pcmData.size() * sizeof(int16_t) berechnet die exakte Byte-Größe!
            size_t dataSizeInBytes = sfx.pcmData.size() * sizeof(int16_t);
            alBufferData(buf, format, sfx.pcmData.data(), static_cast<ALsizei>(dataSizeInBytes), sfx.sampleRate);
            m_buffers[file] = buf;
        }
        return m_buffers[file];
    }
};