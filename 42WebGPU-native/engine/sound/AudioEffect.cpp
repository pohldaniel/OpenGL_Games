#include "AudioEffect.h"

AudioEffect::AudioEffect(const std::string& name) : m_name(name) {

}

const std::string& AudioEffect::getName() const {
    return m_name;
}

void AudioEffect::setEnabled(bool enabled) {
    m_enabled = enabled;
}

bool AudioEffect::isEnabled() const {
    return m_enabled;
}

ChorusEffect::ChorusEffect(const std::string& name, float sampleRate) : AudioEffect(name) {
    m_chorusL.Init(sampleRate); 
    m_chorusL.SetLfoFreq(0.4f);  
    m_chorusL.SetLfoDepth(0.3f);

    m_chorusR.Init(sampleRate); 
    m_chorusR.SetLfoFreq(0.45f); 
    m_chorusR.SetLfoDepth(0.3f);
}

void ChorusEffect::process(float* buffer, int32_t numSamples) {
    for (int32_t i = 0; i < numSamples; i += 2) {
        buffer[i] = m_chorusL.Process(buffer[i]);
        buffer[i + 1] = m_chorusR.Process(buffer[i + 1]);
    }
}

FadeEffect::FadeEffect(const std::string& name) : AudioEffect(name) {

}

void FadeEffect::setEnabled(bool enabled) {
    if (enabled)
        m_enabled = true;

    if (!enabled)
        wantDisable = true;
}

void FadeEffect::process(float* buffer, int32_t numSamples) {

    if (m_enabled && !wantDisable) {
        if (m_timeline < 1.0f) {
            m_timeline += 0.002f;
        }
    }

    if (wantDisable) {
        if (m_timeline > 0.0f) {
            m_timeline -= 0.005f;
        }
    }

    if (m_timeline > 0.001f) {

        float alpha = std::clamp(0.2f * (1.0f - m_timeline), 0.001f, 1.0f);
        float volumeFactor = std::clamp(1.0f - (m_timeline * m_timeline), 0.0f, 1.0f);

        for (int32_t i = 0; i < numSamples; i += 2) {
            buffer[i] = (m_filterLastL + alpha * (buffer[i] - m_filterLastL)) * volumeFactor;
            buffer[i + 1] = (m_filterLastR + alpha * (buffer[i + 1] - m_filterLastR)) * volumeFactor;
        }
    }

    if (m_timeline <= 0.0f && wantDisable) {
        m_enabled = false;
        wantDisable = false;
    }
}