#pragma once

#include <stdint.h>
#include <daisysp.h>

class AudioEffectProcessor {

public:
    virtual ~AudioEffectProcessor() = default;
    virtual void process(float* buffer, int32_t numSamples) = 0;
};

class DaisySpEffectProcessor : public AudioEffectProcessor {
public:
    DaisySpEffectProcessor(float sampleRate) {
        m_chorusL.Init(sampleRate); m_chorusL.SetLfoFreq(0.4f);  m_chorusL.SetLfoDepth(0.3f);
        m_chorusR.Init(sampleRate); m_chorusR.SetLfoFreq(0.45f); m_chorusR.SetLfoDepth(0.3f);
        // Hier ggf. auch Filter initialisieren...
    }

    void process(float* buffer, int32_t numSamples) override {
        for (int32_t i = 0; i < numSamples; i += 2) {
            // DaisySP Effekte in-place auf den Buffer anwenden
            buffer[i] = m_chorusL.Process(buffer[i]);
            buffer[i + 1] = m_chorusR.Process(buffer[i + 1]);
        }
    }

private:
    daisysp::Chorus m_chorusL;
    daisysp::Chorus m_chorusR;
};