#pragma once

#include <stdint.h>
#include <daisysp.h>

class AudioEffect {

public:

    AudioEffect(const std::string& name);

    virtual ~AudioEffect() = default;
    virtual void process(float* buffer, int32_t numSamples) = 0;
    virtual void setEnabled(bool enabled);

    const std::string& getName() const;
    bool isEnabled() const;

protected:

    std::string m_name;
    bool m_enabled = false;
};

class ChorusEffect : public AudioEffect {

public:

    ChorusEffect(const std::string& name, float sampleRate = 44100.0f);
    void process(float* buffer, int32_t numSamples) override;

private:

    daisysp::Chorus m_chorusL;
    daisysp::Chorus m_chorusR;
};

class FadeEffect : public AudioEffect {

public:

    FadeEffect(const std::string& name);
    void process(float* buffer, int32_t numSamples) override;
    void setEnabled(bool enabled) override;
 
private:

    float m_timeline = 0.0f;
    float m_filterLastL = 0.0f;
    float m_filterLastR = 0.0f;
    bool wantDisable = false;
};