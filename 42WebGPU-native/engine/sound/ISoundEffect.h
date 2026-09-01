#pragma once

class ISoundEffect {
public:
    virtual ~ISoundEffect() = default;
    virtual void init() = 0;
    virtual void play(const std::string& file) = 0;
};