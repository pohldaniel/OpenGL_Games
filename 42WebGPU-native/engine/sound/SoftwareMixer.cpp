#include "SoftwareMixer.h"

SoftwareMixer::SoftwareMixer() {
    m_channels.resize(32u);
    m_filterCutoff.store(1.0f);
    m_volume.store(1.0f);
}

void SoftwareMixer::mixAudio(int16_t* outputBuffer, int32_t numSamples) {
    
    for (auto& fx : m_musicEffects) {
        if (fx->isEnabled()) {
            fx->process(outputBuffer, numSamples);
        }
    }

    const float kFilter = m_filterCutoff.load();
    const float kVolumeCenter = 0.707f;
    const float kCrossfeed = 0.12f;
    const float kSfxGain = 0.5f;

    std::vector<int32_t> sfxAccumulatorL(numSamples / 2, 0);
    std::vector<int32_t> sfxAccumulatorR(numSamples / 2, 0);

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
            size_t sampleIdxB_R = sampleIdxB_R + 1;

            if (sampleIdxA_L < soundData.size()) {
                // Lineare Interpolation
                float sampleA_L = static_cast<float>(soundData[sampleIdxA_L]);
                float sampleA_R = (sampleIdxA_R < soundData.size()) ? static_cast<float>(soundData[sampleIdxA_R]) : 0.0f;
                float sampleB_L = (sampleIdxB_L < soundData.size()) ? static_cast<float>(soundData[sampleIdxB_L]) : 0.0f;
                float sampleB_R = (sampleIdxB_R < soundData.size()) ? static_cast<float>(soundData[sampleIdxB_R]) : 0.0f;

                float weightB = channel.progress - static_cast<float>(frameIdxA);
                float weightA = 1.0f - weightB;

                float interpolatedL = (sampleA_L * weightA) + (sampleB_L * weightB);
                float interpolatedR = (sampleA_R * weightA) + (sampleB_R * weightB);

                // Kanal-Tiefpassfilter & Zentrierung
                float currentRawL = interpolatedL * kVolumeCenter;
                float currentRawR = interpolatedR * kVolumeCenter;

                channel.lastSampleL = channel.lastSampleL + kFilter * (currentRawL - channel.lastSampleL);
                channel.lastSampleR = channel.lastSampleR + kFilter * (currentRawR - channel.lastSampleR);

                // Stereo-Crossfeed
                float focusedL = channel.lastSampleL * (1.0f - kCrossfeed) + channel.lastSampleR * kCrossfeed;
                float focusedR = channel.lastSampleR * (1.0f - kCrossfeed) + channel.lastSampleL * kCrossfeed;

                // In den temporären Akkumulator mischen
                sfxAccumulatorL[i / 2] += static_cast<int32_t>(focusedL);
                sfxAccumulatorR[i / 2] += static_cast<int32_t>(focusedR);

                channel.progress += channel.pitchFactor;
            }else {
                channel.status = 0;
                break;
            }
        }
    }

    std::vector<int16_t> sfxFinalBuffer(numSamples, 0);
    for (int32_t i = 0; i < numSamples; i += 2) {
        float finalSfxL = static_cast<float>(sfxAccumulatorL[i / 2]) * kSfxGain;
        float finalSfxR = static_cast<float>(sfxAccumulatorR[i / 2]) * kSfxGain;

        sfxFinalBuffer[i] = std::clamp(static_cast<int32_t>(finalSfxL), -32768, 32767);
        sfxFinalBuffer[i + 1] = std::clamp(static_cast<int32_t>(finalSfxR), -32768, 32767);
    }

    for (auto& effect : m_globalEffects) {
        if (effect->isEnabled()) {
            effect->process(sfxFinalBuffer.data(), numSamples);
        }
    }

    const float currentVolume = m_volume.load();

    for (int32_t i = 0; i < numSamples; i += 2) {
        float totalL = static_cast<float>(outputBuffer[i]) + static_cast<float>(sfxFinalBuffer[i]);
        float totalR = static_cast<float>(outputBuffer[i + 1]) + static_cast<float>(sfxFinalBuffer[i + 1]);

        totalL *= currentVolume;
        totalR *= currentVolume;

        outputBuffer[i] = std::clamp(static_cast<int32_t>(totalL), -32768, 32767);
        outputBuffer[i + 1] = std::clamp(static_cast<int32_t>(totalR), -32768, 32767);
    }
}

void SoftwareMixer::setVolume(float volume) {
    m_volume.store(std::clamp(volume, 0.0f, 1.0f));
}

float SoftwareMixer::getVolume() const {
    return m_volume.load();
}

void SoftwareMixer::setFilter(float cutoff) { 
    m_filterCutoff.store(std::clamp(cutoff, 0.01f, 1.0f)); 
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

void SoftwareMixer::setMusicFilter(bool enabled) {
    m_musicFilterEnabled = enabled;
}

void SoftwareMixer::triggerVinylScratch(bool active) {
    m_scratchActive = active; 
}