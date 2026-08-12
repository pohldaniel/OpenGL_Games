#include <string>
#include "SoftwareMixer.h"

class OboeAudioSFXPlayer {
public:
    OboeAudioSFXPlayer(SoftwareMixer& mixer) : m_mixer(mixer) {}

    void play(const std::string& filename) {
        // Reicht das Abspielen direkt an das Software-Mischpult weiter
        m_mixer.playSFX(filename);
    }

private:
    SoftwareMixer& m_mixer;
};