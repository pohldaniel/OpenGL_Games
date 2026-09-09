#pragma once

#include <vector>
#include <algorithm>
#include <cstdint>
#include <string>
#include <cmath>
#include <cstdlib>
#include <memory>

#include "AudioEffect.h"

struct ActiveSound {
    const std::vector<float>* pcmData = nullptr;
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

    void mixAudio(float* outputBuffer, int32_t numSamples);
    void setFilter(float cutoff);
    void setVolume(float volume);
    float getVolume() const;

    void addAudioEffect(std::unique_ptr<AudioEffect> effect, bool enabled = false);
    void setEnabled(const std::string& name, bool enabled);

private:

    std::vector<std::unique_ptr<AudioEffect>> m_audioEffects;
    std::vector<ActiveSound> m_channels;
    float m_filterCutoff;
    float m_volume;
};