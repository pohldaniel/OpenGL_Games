#pragma once
#include <AL/al.h>
#include <AL/alext.h>
#include <sndfile.h>
#include <malloc.h>
#include <iostream>

#include "Cache.h"

static const int BUFFER_SAMPLES = 8192;
static const int NUM_BUFFERS = 4;

class MusicBuffer{

public:

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

	MusicBuffer();
	MusicBuffer(MusicBuffer const& rhs);
	MusicBuffer& operator=(const MusicBuffer& rhs);
	~MusicBuffer();

	void create(float volume = 1.0f);
	void play(const std::string& file);
	void pause();
	void stop();
	void resume();
	void cleanup();
	void updateBufferStream();

	ALint getSource();

	bool isPlaying();
	void setLooping(const bool& loop);
	void setVolume(float volume);

	static void Init(unsigned short cacheSize = 3u);

private:

	ALuint m_buffers[NUM_BUFFERS];
	ALuint m_source = 0;

	bool m_sourceInit = false;	
	bool  m_loop = false;

	SNDFILE* m_sndFile = nullptr;
	SF_INFO m_sfinfo;
	short* m_membuf = nullptr;
	std::size_t m_frameSize = 0;
	ALenum m_format;
	ALint m_playState;

	const CacheEntry* m_cacheEntry;
	static CacheLRUP<std::string, CacheEntry> MusicBufferCache;
};

