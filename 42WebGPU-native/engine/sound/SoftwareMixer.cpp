#include "SoftwareMixer.h"

SoftwareMixer::SoftwareMixer() {
    m_channels.resize(32u);
    m_filterCutoff.store(1.0f);
    m_volume.store(1.0f);
}

void SoftwareMixer::mixAudio(int16_t* outputBuffer, int32_t numSamples) {
    const float kFilter = m_filterCutoff.load();
    const float kVolumeCenter = 0.707f;
    const float kCrossfeed = 0.12f;
    const float currentVolume = m_volume.load();

    for (int32_t i = 0; i < numSamples; i += 2) {
        int32_t mixedSampleL = 0;
        int32_t mixedSampleR = 0;

        for (auto& channel : m_channels) {
            int currentStatus = channel.status.load();
            if (currentStatus == 0) continue;

            if (currentStatus == 1) {
                channel.status.store(2);
                channel.lastSampleL = 0.0f;
                channel.lastSampleR = 0.0f;
            }

            const auto& soundData = *(channel.pcmData);
            size_t idxA = static_cast<size_t>(channel.progress);
            size_t idxB = idxA + 1;

            if (idxA < soundData.size()) {

                // --- TECHNIK 3: LINEARE INTERPOLATION (GLÄTTUNG) ---
                float sampleA = static_cast<float>(soundData[idxA]);
                float sampleB = (idxB < soundData.size()) ? static_cast<float>(soundData[idxB]) : 0.0f;

                // Gewichtung zwischen den beiden Samples berechnen
                float weightB = channel.progress - static_cast<float>(idxA);
                float weightA = 1.0f - weightB;

                // Das perfekt geglättete Zwischen-Sample ermitteln
                float interpolatedSample = (sampleA * weightA) + (sampleB * weightB);

                // -3dB Zentrierung anwenden
                float currentRawSample = interpolatedSample * kVolumeCenter;

                // Tiefpassfilter anwenden (Wucht & Dumpfheit)
                channel.lastSampleL = channel.lastSampleL + kFilter * (currentRawSample - channel.lastSampleL);
                channel.lastSampleR = channel.lastSampleR + kFilter * (currentRawSample - channel.lastSampleR);

                // Stereo-Crossfeed (Fokus)
                float focusedL = channel.lastSampleL * (1.0f - kCrossfeed) + channel.lastSampleR * kCrossfeed;
                float focusedR = channel.lastSampleR * (1.0f - kCrossfeed) + channel.lastSampleL * kCrossfeed;

                mixedSampleL += static_cast<int32_t>(focusedL);
                mixedSampleR += static_cast<int32_t>(focusedR);

               

                // Fortschritt um den individuellen Pitch-Faktor erhöhen
                channel.progress += channel.pitchFactor;
            }
            else {
                channel.status.store(0);
            }
        }

        const float kSfxGain = 0.5f;
        float finalSfxL = static_cast<float>(mixedSampleL) * kSfxGain;
        float finalSfxR = static_cast<float>(mixedSampleR) * kSfxGain;

        // Musik aus dem Ringbuffer (outputBuffer) und SFX zusammenrechnen
        float totalL = static_cast<float>(outputBuffer[i]) + finalSfxL;
        float totalR = static_cast<float>(outputBuffer[i + 1]) + finalSfxR;

        // --- NEU: DYNAMISCHE LAUTSTÄRKE FÜR DEN GESAMTEN RTAUDIO-STRANG ---
        totalL *= currentVolume;
        totalR *= currentVolume;

        // Sättigungsschutz anwenden und in die Soundkarte schreiben
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