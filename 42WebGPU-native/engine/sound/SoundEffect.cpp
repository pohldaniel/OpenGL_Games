#include "SoundEffect.h"

void SoundEffect::init(std::unique_ptr<ISoundEffect> soundEffect) {
    if (soundEffect) {
        m_soundEffect = std::move(soundEffect);
        m_soundEffect->init();
    }
}

void SoundEffect::play(const std::string& file) {
    m_soundEffect->play(file);
}