#pragma once
#include <vector>
#include <atomic>
#include <algorithm>
#include <cstdint>
#include <string>
#include <cmath>
#include <cstdlib>
#include <memory>

#include "EffectNodes.h"

struct ActiveSound {
    const std::vector<int16_t>* pcmData = nullptr;
    float progress = 0.0f;
    float pitchFactor = 1.0f;
    int status = 0;


    float lastSampleL = 0.0f;
    float lastSampleR = 0.0f;

    ActiveSound() : pcmData(nullptr), progress(0.0f), status(0) {  }

    ActiveSound(const ActiveSound& other) {
        pcmData = other.pcmData;
        progress = other.progress;
        pitchFactor = other.pitchFactor;
        status = other.status;
        lastSampleL = other.lastSampleL;
        lastSampleR = other.lastSampleR;
    }

    ActiveSound& operator=(const ActiveSound& other) {
        if (this != &other) {
            pcmData = other.pcmData;
            progress = other.progress;
            pitchFactor = other.pitchFactor;
            status = other.status;
            lastSampleL = other.lastSampleL;
            lastSampleR = other.lastSampleR;
        }
        return *this;
    }
};

class SoftwareMixer {

    friend class RtAudioEffect;

public:

    SoftwareMixer();
    ~SoftwareMixer() = default;

    void mixAudio(int16_t* outputBuffer, int32_t numSamples);
    void setFilter(float cutoff);
    void setVolume(float volume);
    float getVolume() const;
    void addEffect(std::unique_ptr<AudioNode> effect);
    void addMusicEffect(std::unique_ptr<AudioNode> fx);

    void setEnabled(const std::string& id, bool enabled);
    void setMusicFilter(bool enabled);
    void triggerVinylScratch(bool active);

private:

    std::vector<std::unique_ptr<AudioNode>> m_globalEffects;
    std::vector<std::unique_ptr<AudioNode>> m_musicEffects;

    std::vector<ActiveSound> m_channels;
    std::atomic<float> m_filterCutoff;
    std::atomic<float> m_volume;

    bool m_musicFilterEnabled = false;
    float m_musicLastL = 0.0f;
    float m_musicLastR = 0.0f;

    bool m_scratchActive = false;
    float m_scratchTimeline = 0.0f;
    float m_scratchFilterLastL = 0.0f;
    float m_scratchFilterLastR = 0.0f;
};