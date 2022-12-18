#pragma once

#include <iostream>
#include <AL\al.h>

class SoundEffectsPlayer {

public:
	SoundEffectsPlayer() = default;
	~SoundEffectsPlayer();
	void init();
	void play(const ALuint& buffer_to_play);
	void stop();
	void pause();
	void resume();

	void setLooping(const bool& loop);
	void setVolume(float volume);
	bool isPlaying();

private:
	ALuint m_source;
	ALuint m_buffer = 0;

	ALint state;
};