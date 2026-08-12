#pragma once
#include <string>

class AudioSystem {
public:
    virtual ~AudioSystem() = default;

    virtual bool init() = 0;
    virtual void playMusic(const std::string& filename) = 0;
    virtual void playSFX(const std::string& filename) = 0;
    virtual void update() = 0;
    virtual SoftwareMixer& getMixer() = 0;
};