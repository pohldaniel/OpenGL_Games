#pragma once
#include <AL/al.h>
#include <string>
#include "Cache.h"

class SoundBuffer{

	struct CacheEntryBuffer {
		unsigned short index;
		CacheEntryBuffer(const std::string& path, const size_t index);
	};

	struct CacheEntrySource {
		unsigned short index;
		CacheEntrySource(const std::string& path, const size_t index);
	};

public:

	
	SoundBuffer();
	SoundBuffer(SoundBuffer const& rhs);
	SoundBuffer& operator=(const SoundBuffer& rhs);

	~SoundBuffer();
	void init();

	void play(const std::string& file);
	void playChanneled(const std::string& file);
	void setVolume(float volume);

private:

	ALuint m_source;
	ALint m_playState;

	unsigned short m_bufferIndex;
	unsigned short m_sourceIndex;
	bool m_sourceInit = false;

	std::vector<ALuint> m_sources;
	CacheLRU<std::string, SoundBuffer::CacheEntrySource> m_soundSourceCache;

	static std::vector<ALuint> Buffer;
	static CacheLRU<std::string, SoundBuffer::CacheEntryBuffer> SoundBufferCache;
	static bool CacheInit;
	static unsigned short Instances;
};

