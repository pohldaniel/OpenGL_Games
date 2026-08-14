#pragma once
#include <algorithm>
#include <AL/al.h>
#include <AL/alc.h>
#include <iostream>
#include "AudioDecoder.h"

class OpenALMovieStream {
public:
    OpenALMovieStream() : m_source(0) {
        m_buffers[0] = 0;
        m_buffers[1] = 0;
    }

    ~OpenALMovieStream() {
        close();
    }

    // WICHTIG: Erst aufrufen, wenn dein globales OpenAL-System initialisiert ist!
    bool init() {
        // Sicherstellen, dass wir sauber von vorne starten
        close();

        alGenSources(1, &m_source);
        // KORREKTUR: 2 Buffers generieren und im Array ablegen
        alGenBuffers(2, m_buffers);

        // Fehler-Check für die Zuweisung unter dem aktiven Kontext
        ALenum error = alGetError();
        if (error != AL_NO_ERROR || m_source == 0) {
            std::cerr << "OpenALMovieStream Fehler beim Erstellen der Source: " << error << std::endl;
            return false;
        }

        // Standardmäßig auf volle Lautstärke (1.0f) setzen
        alSourcef(m_source, AL_GAIN, 1.0f);
        return true;
    }

    void setVolume(float volume) {
        if (m_source) {
            alSourcef(m_source, AL_GAIN, std::clamp(volume, 0.0f, 1.0f));
        }
    }

    float getVolume() {
        ALfloat volume;
        alGetSourcef(m_source, AL_GAIN, &volume);
        return volume;
    }

    void streamAudio(const std::vector<uint8_t>& pcmData) {
        if (m_source == 0) return;

        // New frisch eingetroffene Daten am Ende unseres Sammlers anhängen
        if (!pcmData.empty()) {
            m_audioAccumulator.insert(m_audioAccumulator.end(), pcmData.begin(), pcmData.end());
        }

        // BEHINDERT MICRO-RUCKELN: Erst abschicken, wenn genug "Fleisch" da ist!
        // 4096 Bytes bei 44.1kHz Stereo entspricht einem lückenlosen Polster von ~23ms.
        // Bei 240 FPS sammelt er ca. 5-6 Frames lang, bevor er OpenAL füttert.
        if (m_audioAccumulator.size() < 4096) {
            return;
        }

        ALint processed = 0;
        alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processed);

        if (processed > 0) {
            ALuint unqueued;
            alSourceUnqueueBuffers(m_source, 1, &unqueued);

            // Wir übergeben die gesammelten Daten aus dem Akkumulator
            alBufferData(unqueued, AL_FORMAT_STEREO16, m_audioAccumulator.data(), m_audioAccumulator.size(), 44100);
            alSourceQueueBuffers(m_source, 1, &unqueued);

            // Den Sammler leeren, da die Daten erfolgreich bei der Soundkarte liegen
            m_audioAccumulator.clear();
        }
        else {
            ALint queued = 0;
            alGetSourcei(m_source, AL_BUFFERS_QUEUED, &queued);

            if (queued < 2) {
                ALuint targetBuffer = m_buffers[queued];
                alBufferData(targetBuffer, AL_FORMAT_STEREO16, m_audioAccumulator.data(), m_audioAccumulator.size(), 44100);
                alSourceQueueBuffers(m_source, 1, &targetBuffer);

                m_audioAccumulator.clear();
            }
            // Falls beide Buffer voll sind und noch kein Buffer "processed" ist,
            // behalten wir die Daten im m_audioAccumulator und warten auf den nächsten Frame!
        }

        // Stream-Sicherheitsschaltung gegen Stillstand
        ALint state;
        alGetSourcei(m_source, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING) {
            alSourcePlay(m_source);
        }
    }

    void close() {
        if (m_source) {
            alSourceStop(m_source);
            alDeleteSources(1, &m_source);
            m_source = 0;
        }
        if (m_buffers[0] != 0) {
            alDeleteBuffers(2, m_buffers);
            m_buffers[0] = 0;
            m_buffers[1] = 0;
        }
    }

private:
    ALuint m_source;
    ALuint m_buffers[2]; // KORREKTUR: Explizites Array mit 2 Slots
    std::vector<uint8_t> m_audioAccumulator;
};

class OpenALPlayer {
public:
    OpenALPlayer();
    ~OpenALPlayer();

    bool init();
    void play(const std::string& filename);
    void update();

    void streamMovieAudio(const std::vector<uint8_t>& pcmData);
    void prepareForMovie();

private:
    ALCdevice* device = nullptr;
    ALCcontext* context = nullptr;
    ALuint source = 0;
    ALuint buffers[2] = { 0, 0 }; // Double-Buffering

    AudioDecoder decoder;
    bool isPlaying = false;

    void streamBuffer(ALuint bufferId);
};