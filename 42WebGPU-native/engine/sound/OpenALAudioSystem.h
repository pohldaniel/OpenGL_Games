#pragma once
#include "AudioSystem.h"
#include "OpenALPlayer.h" // Dein bestehender Musik-Player
#include "OpenALSFXPlayer.h" // Der Source-Pool-Player für Effekte

class OpenALAudioSystem : public AudioSystem {
public:
    bool init() override {

        m_device = alcOpenDevice(nullptr);
        if (!m_device) return false;

        m_context = alcCreateContext(m_device, nullptr);
        if (!m_context || !alcMakeContextCurrent(m_context)) return false;

        if (!m_musicPlayer.init()) return false;
        m_sfxPlayer.init(32); // 32 Soundkanäle bereitstellen
        return true;
    }

    void shutDown() override {
        if (m_context) {
            alcMakeContextCurrent(nullptr);
            alcDestroyContext(m_context);
        }

        if (m_device) 
            alcCloseDevice(m_device);
    }

    void playMusic(const std::string& filename) override {
        m_musicPlayer.play(filename);
    }

    void playSFX(const std::string& filename) override {
        m_sfxPlayer.play(filename);
    }

    void update() override {
        m_musicPlayer.update(); // Streamt MP3-Frames nach
    }

    SoftwareMixer& getMixer() override {
        static SoftwareMixer dummyMixer;
        return dummyMixer;
    }
   
private:
    OpenALPlayer m_musicPlayer;
    OpenALSFXPlayer m_sfxPlayer;

    ALCdevice* m_device = nullptr;
    ALCcontext* m_context = nullptr;
};