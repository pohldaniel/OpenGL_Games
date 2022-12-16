#pragma once
#include <AL\al.h>
#include <AL\alext.h>
#include <sndfile.h>
#include <malloc.h>

#include "Cache.h"


struct CacheEntry {
	SNDFILE* sndFile = nullptr;
	SF_INFO sfinfo;
	short* membuf = nullptr;
	std::size_t frameSize = 0;
	ALenum format;
	const int BUFFER_SAMPLES = 8192;

	CacheEntry(const std::string& path);
	~CacheEntry();
};

class MusicBuffer{

public:

	MusicBuffer();
	MusicBuffer(MusicBuffer const& rhs);
	MusicBuffer& operator=(const MusicBuffer& rhs);

	~MusicBuffer();

	void play(const std::string& file);
	void pause();
	void stop();
	void resume();

	void updateBufferStream();

	ALint getSource();

	bool isPlaying();
	void setLooping(const bool& loop);
	void loadFromFile(const std::string& path);
	void setVolume(float volume);

private:
	ALuint m_source;
	static const int BUFFER_SAMPLES = 8192;
	static const int NUM_BUFFERS = 4;
	ALuint m_buffers[NUM_BUFFERS];
	bool  m_loop = false;

	SNDFILE* m_sndFile = nullptr;
	SF_INFO m_sfinfo;
	short* m_membuf = nullptr;
	std::size_t m_frameSize = 0;
	ALenum m_format;

	const CacheEntry* m_cacheEntry;
	CacheLRU<std::string, CacheEntry> m_musicBufferCache;
};

