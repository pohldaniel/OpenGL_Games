#pragma once
#include <AL\al.h>
#include <AL\alext.h>
#include <sndfile.h>
#include <malloc.h>
#include <string>

#include "Cache.h"

struct CacheEntry2 {

	ALuint buffer;
	CacheEntry2(const std::string& path);
	~CacheEntry2();
};

class SoundEffectsPlayer{

public:
	SoundEffectsPlayer() = default;
	~SoundEffectsPlayer();
	void init();
	void play(const ALuint& buffer_to_play);
	void play(const std::string& file);

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

	const CacheEntry2* m_cacheEntry;
	static CacheLRU<std::string, CacheEntry2> SoundBufferCache;
};

