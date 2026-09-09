#include "SoftwareMixer.h"

SoftwareMixer::SoftwareMixer() {
    m_channels.resize(32u);
    m_filterCutoff = 1.0f;
    m_volume = 1.0f;
}

void SoftwareMixer::mixAudio(float* outputBuffer, int32_t numSamples) {

    const float kFilter = m_filterCutoff;
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

                // Glatterer Einpol-Filter
                channel.lastSampleL = channel.lastSampleL + kFilter * (interpolatedL - channel.lastSampleL);
                channel.lastSampleR = channel.lastSampleR + kFilter * (interpolatedR - channel.lastSampleR);
                outputBuffer[i] += channel.lastSampleL;
                outputBuffer[i + 1] += channel.lastSampleR;
   
                channel.progress += channel.pitchFactor;
            }else {
                channel.status = 0;
                break;
            }
        }
    }

    for (int32_t i = 0; i < numSamples; i += 2) {
        outputBuffer[i] *= m_volume;
        outputBuffer[i + 1] *= m_volume;
    }

    for (auto& effect : m_audioEffects) {
        if (effect->isEnabled()) {
            effect->process(outputBuffer, numSamples);
        }
    }

    for (int32_t i = 0; i < numSamples; i++) {
        outputBuffer[i] = std::clamp(outputBuffer[i], -1.0f, 1.0f);
    }
}

void SoftwareMixer::setVolume(float volume) {
    m_volume = volume;
}

float SoftwareMixer::getVolume() const {
    return m_volume;
}

void SoftwareMixer::setFilter(float cutoff) { 
    m_filterCutoff = std::clamp(cutoff, 0.01f, 1.0f); 
}

void SoftwareMixer::addAudioEffect(std::unique_ptr<AudioEffect> effect, bool enabled) {
    m_audioEffects.push_back(std::move(effect));
    if(enabled)
        m_audioEffects.back()->setEnabled(true);
}

void SoftwareMixer::setEnabled(const std::string& name, bool enabled) {
    for (auto& effect : m_audioEffects) {
        if (effect->getName() == name) {
            effect->setEnabled(enabled);
            return;
        }
    }
}