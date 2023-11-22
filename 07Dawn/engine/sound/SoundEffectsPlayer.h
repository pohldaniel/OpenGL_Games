#pragma once
#include <sndfile.h>
#include <AL\al.h>
#include <AL\alext.h>
#include <inttypes.h>
#include <iostream>

class SoundEffectsPlayer {

public:
	SoundEffectsPlayer() = default;
	~SoundEffectsPlayer();

	void loadFromFile(const std::string& path);
	void init();
	void play();
	void stop();
	void pause();
	void resume();

	void setLooping(const bool& loop);
	void setVolume(float volume);
	bool isPlaying();

private:
	ALuint m_source;
	ALuint m_buffer;
	ALint state;

	bool m_sourceInit = false;
};