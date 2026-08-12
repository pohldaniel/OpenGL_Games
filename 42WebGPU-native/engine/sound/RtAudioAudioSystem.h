#pragma once
#include "AudioSystem.h"
#include "RtAudioPlayer.h" // Modifiziert: nutzt nun den SoftwareMixer im Callback
#include "SoftwareMixer.h"

class RtAudioAudioSystem : public AudioSystem {
public:
    bool init() override {
        return m_player.init();
    }

    void playMusic(const std::string& filename) override {
        m_player.play(filename);
    }

    void playSFX(const std::string& filename) override {
        // Der SoftwareMixer mischt es im Hintergrund-Thread direkt dazu
        m_player.getMixer().playSFX(filename);
    }

    void update() override {
        m_player.update(); // Füllt die Musik in den Ringbuffer nach
    }

    virtual SoftwareMixer& getMixer() override {
        return m_player.getMixer();
    }

private:
    RtAudioPlayer m_player;
};