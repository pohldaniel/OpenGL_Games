#pragma once
#include <vector>
#include <atomic>
#include <algorithm>
#include <cstdint>
#include <string>
#include <cmath>
#include <cstdlib>

struct ActiveSound {
    const std::vector<int16_t>* pcmData = nullptr;
    float progress = 0.0f;        // JETZT ALS FLOAT für stufenlosen Pitch
    float pitchFactor = 1.0f;     // Dynamischer Pitch pro Schuss
    std::atomic<int> status{ 0 };

    // Filter-Gedächtnis
    float lastSampleL = 0.0f;
    float lastSampleR = 0.0f;

    ActiveSound() : pcmData(nullptr), progress(0.0f) { status.store(0); }

    ActiveSound(const ActiveSound& other) {
        pcmData = other.pcmData;
        progress = other.progress;
        pitchFactor = other.pitchFactor;
        status.store(other.status.load());
        lastSampleL = other.lastSampleL;
        lastSampleR = other.lastSampleR;
    }

    ActiveSound& operator=(const ActiveSound& other) {
        if (this != &other) {
            pcmData = other.pcmData;
            progress = other.progress;
            pitchFactor = other.pitchFactor;
            status.store(other.status.load());
            lastSampleL = other.lastSampleL;
            lastSampleR = other.lastSampleR;
        }
        return *this;
    }
};

class SoftwareMixer {
public:
    SoftwareMixer() {
        m_channels.resize(32);
        m_filterCutoff.store(1.0f);
        m_volume.store(0.5f);
    }

    void playSFX(const std::string& file);
    void mixAudio(int16_t* outputBuffer, int32_t numSamples);
    void setFilter(float cutoff) { m_filterCutoff.store(std::clamp(cutoff, 0.01f, 1.0f)); }

    void setVolume(float volume) {
        m_volume.store(std::clamp(volume, 0.0f, 1.0f));
    }

    float getVolume() const {
        return m_volume.load();
    }

private:
    std::vector<ActiveSound> m_channels;
    std::atomic<float> m_filterCutoff;
    std::atomic<float> m_volume;
};