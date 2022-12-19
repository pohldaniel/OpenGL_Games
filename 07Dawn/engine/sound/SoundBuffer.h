#pragma once
#include <AL/al.h>
#include <string>
#include "Cache.h"

class SoundBuffer{

	struct CacheEntryBuffer {
		unsigned short index;
		CacheEntryBuffer(const std::string& file, const size_t index);
	};

	struct CacheEntrySource {
		unsigned short index;
		CacheEntrySource(const std::string& file, const size_t index);
	};

public:

	
	SoundBuffer();
	SoundBuffer(SoundBuffer const& rhs);
	SoundBuffer& operator=(const SoundBuffer& rhs);

	~SoundBuffer();
	void init(unsigned short cacheSizeBuffer = 20u, unsigned short cacheSizeSources = 10u, unsigned short channelSize = 11u, float volume = 1.0f);

	void play(const std::string& file);
	void playParallel(const std::string& file);	
	void stop(const std::string& file);
	void pause(const std::string& file);
	void resume(const std::string& file);
	void setVolume(float volume);

	void playChannel(unsigned int channel);
	void stopChannel(unsigned int channel);
	void pauseChannel(unsigned int channel);
	void resumeChannel(unsigned int channel);
	void setVolumeChannel(unsigned int channel, float volume);
	void loadChannel(const std::string& file, unsigned int channel);
	

	
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

