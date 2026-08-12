#pragma once
#include "AudioSystem.h"
#include "OboeAudioPlayer.h"

class OboeAudioSystem : public AudioSystem {
public:
    bool init() override {
        return m_player.init();
    }

    void playMusic(const std::string& filename) override {
        m_player.playMusic(filename);
    }

    void playSFX(const std::string& filename) override {
        // Schiebt den Soundeffekt in den lock-freien SoftwareMixer
        m_player.getMixer().playSFX(filename);
    }

    void update() override {
        m_player.update(); // Füllt periodisch die Musik-Frames aus FFmpeg in den Ringbuffer
    }

    SoftwareMixer& getMixer() override {
        return m_player.getMixer();
    }

private:
    OboeAudioPlayer m_player;
};