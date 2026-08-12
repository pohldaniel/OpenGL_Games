#pragma once
#include "AudioSystem.h"
#include "OpenALPlayer.h" // Dein bestehender Musik-Player
#include "OpenALSFXPlayer.h" // Der Source-Pool-Player für Effekte

class OpenALAudioSystem : public AudioSystem {
public:
    bool init() override {
        if (!m_musicPlayer.init()) return false;
        m_sfxPlayer.init(32); // 32 Soundkanäle bereitstellen
        return true;
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
};