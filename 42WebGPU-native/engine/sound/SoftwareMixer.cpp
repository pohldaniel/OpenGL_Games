#include "SoftwareMixer.h"

SoftwareMixer::SoftwareMixer() {
    m_channels.resize(32u);
    m_filterCutoff = 1.0f;
    m_volume = 1.0f;
}

void SoftwareMixer::mixAudio(float* outputBuffer, int32_t numSamples, AudioEffectProcessor* effect) {

    const float kFilter = m_filterCutoff;
    const float kVolumeCenter = 0.707f;
    const float kCrossfeed = 0.12f;
    const float kSfxGain = 0.5f;
    const float currentVolume = m_volume;

    for (auto& channel : m_channels) {
        int currentStatus = channel.status;
        if (currentStatus == 0) continue;

        if (currentStatus == 1) {
            channel.status = 2;
            channel.lastSampleL = 0.0f;
            channel.lastSampleR = 0.0f;
        }

        const auto& soundData = *(channel.pcmData);

        for (int32_t i = 0; i < numSamples; i += 2) {
            size_t frameIdxA = static_cast<size_t>(channel.progress);
            size_t frameIdxB = frameIdxA + 1;

            size_t sampleIdxA_L = frameIdxA * 2;
            size_t sampleIdxA_R = sampleIdxA_L + 1;
            size_t sampleIdxB_L = frameIdxB * 2;
            size_t sampleIdxB_R = sampleIdxB_L + 1;

            if (sampleIdxA_L < soundData.size()) {
                float sampleA_L = soundData[sampleIdxA_L];
                float sampleA_R = (sampleIdxA_R < soundData.size()) ? soundData[sampleIdxA_R] : 0.0f;
                float sampleB_L = (sampleIdxB_L < soundData.size()) ? soundData[sampleIdxB_L] : 0.0f;
                float sampleB_R = (sampleIdxB_R < soundData.size()) ? soundData[sampleIdxB_R] : 0.0f;

                float weightB = channel.progress - static_cast<float>(frameIdxA);
                float weightA = 1.0f - weightB;

                float interpolatedL = (sampleA_L * weightA) + (sampleB_L * weightB);
                float interpolatedR = (sampleA_R * weightA) + (sampleB_R * weightB);

                float currentRawL = interpolatedL * kVolumeCenter;
                float currentRawR = interpolatedR * kVolumeCenter;

                channel.lastSampleL = channel.lastSampleL + kFilter * (currentRawL - channel.lastSampleL);
                channel.lastSampleR = channel.lastSampleR + kFilter * (currentRawR - channel.lastSampleR);

                float focusedL = channel.lastSampleL * (1.0f - kCrossfeed) + channel.lastSampleR * kCrossfeed;
                float focusedR = channel.lastSampleR * (1.0f - kCrossfeed) + channel.lastSampleL * kCrossfeed;

                outputBuffer[i] += focusedL * kSfxGain;
                outputBuffer[i + 1] += focusedR * kSfxGain;

                channel.progress += channel.pitchFactor;
            }else {
                channel.status = 0;
                break;
            }
        }
    }

    for (int32_t i = 0; i < numSamples; i += 2) {
        outputBuffer[i] *= currentVolume;
        outputBuffer[i + 1] *= currentVolume;
    }

    if (effect != nullptr) {
        effect->process(outputBuffer, numSamples);
    }

    for (int32_t i = 0; i < numSamples; i++) {
        outputBuffer[i] = std::clamp(outputBuffer[i], -1.0f, 1.0f);
    }
}

void SoftwareMixer::setVolume(float volume) {
    m_volume = std::clamp(volume, 0.0f, 1.0f);
}

float SoftwareMixer::getVolume() const {
    return m_volume;
}

void SoftwareMixer::setFilter(float cutoff) { 
    m_filterCutoff = std::clamp(cutoff, 0.01f, 1.0f); 
}

void SoftwareMixer::addEffect(std::unique_ptr<AudioNode> effect) {
    m_globalEffects.push_back(std::move(effect));
}

void SoftwareMixer::addMusicEffect(std::unique_ptr<AudioNode> fx) {
    m_musicEffects.push_back(std::move(fx)); 
}

void SoftwareMixer::setEnabled(const std::string& id, bool enabled) {
    for (auto& fx : m_musicEffects) {
        if (fx->getId() == id) {
            fx->setEnabled(enabled);           
            return;
        }
    }
    for (auto& effect : m_globalEffects) {
        if (effect->getId() == id) {
            effect->setEnabled(enabled);
            return;
        }
    }
}