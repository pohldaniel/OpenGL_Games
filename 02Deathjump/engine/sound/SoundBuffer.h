#pragma once
#include <AL/al.h>
#include <vector>
#include <string>

class SoundBuffer{
public:
	SoundBuffer() = default;
	~SoundBuffer();

	void loadFromFile(const std::string& path);
	unsigned int getBuffer();

private:
	unsigned int m_buffer;
};

