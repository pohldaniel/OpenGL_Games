#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <algorithm>

class AudioNode {

public:
    AudioNode(const std::string& id) : m_id(id) {

    }

    virtual ~AudioNode() = default;
    virtual void process(int16_t* buffer, int32_t numSamples) = 0;

    std::string getId() const {
        return m_id;
    }

    virtual void setEnabled(bool enabled) {
        m_enabled = enabled;
    }

    bool isEnabled() const {
        return m_enabled;
    }

protected:
    std::string m_id;
    bool m_enabled = false;
};

class LowPassFilterNode : public AudioNode {
private:
    float m_lastL = 0.0f;
    float m_lastR = 0.0f;
    float m_alpha = 0.05f; // Filter-Stärke (fest oder dynamisch)

public:
    LowPassFilterNode(const std::string& id) : AudioNode(id) {}

    void process(int16_t* buffer, int32_t numSamples) override {
        for (int32_t i = 0; i < numSamples; i += 2) {
            float musicL = static_cast<float>(buffer[i]);
            float musicR = static_cast<float>(buffer[i + 1]);

            m_lastL = m_lastL + m_alpha * (musicL - m_lastL);
            m_lastR = m_lastR + m_alpha * (musicR - m_lastR);

            buffer[i] = static_cast<int16_t>(m_lastL);
            buffer[i + 1] = static_cast<int16_t>(m_lastR);
        }
    }
};

class VinylScratchNode : public AudioNode {

private:

    float m_timeline = 0.0f;
    float m_filterLastL = 0.0f;
    float m_filterLastR = 0.0f;
    bool wantDisable = false;

public:

    VinylScratchNode(const std::string& id) : AudioNode(id) {}

    void setEnabled(bool enabled) override {
        if(enabled)
            m_enabled = true;

        if(!enabled)
            wantDisable = true;
    }

    void process(int16_t* buffer, int32_t numSamples) override {

        if (m_enabled && !wantDisable) {
            if (m_timeline < 1.0f) {
                m_timeline += 0.002f;
            }
        }
        
        if(wantDisable){
            if (m_timeline > 0.0f) {
                m_timeline -= 0.005f;
            }
        }
       
        if (m_timeline > 0.001f) {

            float alpha = std::clamp(0.2f * (1.0f - m_timeline), 0.001f, 1.0f);
            float volumeFactor = std::clamp(1.0f - (m_timeline * m_timeline), 0.0f, 1.0f);

            for (int32_t i = 0; i < numSamples; i += 2) {
                float musicL = static_cast<float>(buffer[i]);
                float musicR = static_cast<float>(buffer[i + 1]);

                // Filter anwenden
                m_filterLastL = m_filterLastL + alpha * (musicL - m_filterLastL);
                m_filterLastR = m_filterLastR + alpha * (musicR - m_filterLastR);

                // Sound leiser machen und Filter anwenden
                buffer[i] = static_cast<int16_t>(m_filterLastL * volumeFactor);
                buffer[i + 1] = static_cast<int16_t>(m_filterLastR * volumeFactor);
            }
        }

        if (m_timeline <= 0.0f && wantDisable) {
            m_enabled = false;
            wantDisable = false;
        }
            
    }
};

class SpaceDelayEffect : public AudioNode {
private:
    std::vector<int32_t> m_delayBufferL;
    std::vector<int32_t> m_delayBufferR;
    size_t m_writePos = 0;

    // EINSTELLUNGEN FÜR BRUTALEN EFFEKT:
    // 0.0 = Aus, 1.0 = Echo genauso laut wie der Schuss
    float m_wetLevel = 0.6f;
    // Wie stark das Echo nachhallt (0.0 = nur 1 Echo, 0.8 = extremer Nachhall)
    float m_feedback = 0.5f;
    // Verzögerung in Millisekunden (120ms ist perfekt für MG-Rhythmen)
    float m_delayMs = 120.0f;
    float m_sampleRate = 44100.0f;

public:
    SpaceDelayEffect(const std::string& id) : AudioNode(std::move(id)) {
        // Buffer für bis zu 1 Sekunde Verzögerung reservieren
        size_t bufferSize = static_cast<size_t>(m_sampleRate * 1.0f);
        m_delayBufferL.resize(bufferSize, 0);
        m_delayBufferR.resize(bufferSize, 0);
    }

    void process(int16_t* buffer, int32_t numSamples) override {
        size_t delaySamples = static_cast<size_t>((m_delayMs / 1000.0f) * m_sampleRate);

        // Sicherheitscheck, damit wir nicht über den Puffer hinauslesen
        if (delaySamples >= m_delayBufferL.size()) delaySamples = m_delayBufferL.size() - 1;

        for (int32_t i = 0; i < numSamples; i += 2) {
            float inL = static_cast<float>(buffer[i]);
            float inR = static_cast<float>(buffer[i + 1]);

            // 1. Berechne die Position, die wir aus der Vergangenheit auslesen
            int32_t readPos = static_cast<int32_t>(m_writePos) - static_cast<int32_t>(delaySamples);
            if (readPos < 0) readPos += m_delayBufferL.size();

            // 2. Das alte Echo aus dem Speicher holen
            float delayedL = static_cast<float>(m_delayBufferL[readPos]);
            float delayedR = static_cast<float>(m_delayBufferR[readPos]);

            // 3. Den neuen Sound + das alte Echo (mit Feedback) zurück in den Speicher schreiben
            m_delayBufferL[m_writePos] = static_cast<int32_t>(inL + (delayedL * m_feedback));
            m_delayBufferR[m_writePos] = static_cast<int32_t>(inR + (delayedR * m_feedback));

            // 4. Das Echo auf den aktuellen Sound draufmischen (Dry/Wet)
            float outL = inL + (delayedL * m_wetLevel);
            float outR = inR + (delayedR * m_wetLevel);

            // 5. In den Haupt-Audiopuffer zurückschreiben (mit Übersteuerungsschutz)
            buffer[i] = std::clamp(static_cast<int32_t>(outL), -32768, 32767);
            buffer[i + 1] = std::clamp(static_cast<int32_t>(outR), -32768, 32767);

            // Schreibzeiger weiterschieben
            m_writePos = (m_writePos + 1) % m_delayBufferL.size();
        }
    }
};

class ChorusEffect : public AudioNode {
private:
    std::vector<float> m_delayBufferL;
    std::vector<float> m_delayBufferR;
    size_t m_writePos = 0;
    float m_lfoPhase = 0.0f;

    // Parameter
    float m_depth = 0.03f;      // Stärke des Effekts (0.0 bis 1.0)
    float m_rate = 1.5f;        // Geschwindigkeit in Hz
    float m_sampleRate = 44100.0f;

public:
    ChorusEffect(const std::string& id) : AudioNode(id) {
        // Maximal 50ms Delay-Buffer aufbauen
        size_t bufferSize = static_cast<size_t>(m_sampleRate * 0.05f);
        m_delayBufferL.resize(bufferSize, 0.0f);
        m_delayBufferR.resize(bufferSize, 0.0f);
    }

    void process(int16_t* buffer, int32_t numSamples) override {
        for (int32_t i = 0; i < numSamples; i += 2) {
            float inL = static_cast<float>(buffer[i]);
            float inR = static_cast<float>(buffer[i + 1]);

            // 1. Aktuelles Sample in den Delay-Buffer schreiben
            m_delayBufferL[m_writePos] = inL;
            m_delayBufferR[m_writePos] = inR;

            // 2. LFO (Oszillator) für die Verzögerungs-Modulation berechnen
            m_lfoPhase += (2.0f * M_PI * m_rate) / m_sampleRate;
            if (m_lfoPhase > 2.0f * M_PI) m_lfoPhase -= 2.0f * M_PI;

            // Modulation berechnen (Schwankt z.B. zwischen 10ms und 30ms Delay)
            float modDelay = 0.02f + 0.01f * std::sin(m_lfoPhase);
            float delaySamples = modDelay * m_sampleRate;

            // 3. Lese-Position im Ringpuffer bestimmen (vereinfacht ohne Interpolation)
            int32_t readPos = static_cast<int32_t>(m_writePos) - static_cast<int32_t>(delaySamples);
            if (readPos < 0) readPos += m_delayBufferL.size();

            float delayedL = m_delayBufferL[readPos];
            float delayedR = m_delayBufferR[readPos];

            // 4. Original (Dry) und Effekt (Wet) mischen
            float outL = inL + delayedL * m_depth;
            float outR = inR + delayedR * m_depth;

            // 5. Zurückschreiben und Clamping
            buffer[i] = std::clamp(static_cast<int32_t>(outL), -32768, 32767);
            buffer[i + 1] = std::clamp(static_cast<int32_t>(outR), -32768, 32767);

            // Ring-Buffer Position erhöhen
            m_writePos = (m_writePos + 1) % m_delayBufferL.size();
        }
    }
};