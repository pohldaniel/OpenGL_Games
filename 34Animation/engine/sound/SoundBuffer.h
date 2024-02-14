#pragma once
#include <AL/al.h>
#include <string>
#include "Cache.h"

class SoundBuffer{

	struct CacheEntryBuffer {
		unsigned short index;
		CacheEntryBuffer(const std::string& file, const size_t index);
		CacheEntryBuffer() = default;
	};

	struct CacheEntrySource {
		short index;
		CacheEntrySource(const std::string& file, const size_t index);
		CacheEntrySource();
	};

public:
	
	SoundBuffer();
	SoundBuffer(SoundBuffer const& rhs);
	SoundBuffer& operator=(const SoundBuffer& rhs);

	~SoundBuffer();
	void create(unsigned short cacheSizeSources = 10u, unsigned short channelSize = 20u, float volume = 1.0f);

	void play(const std::string& file);
	void playOverlayed(const std::string& file);
	void playStacked(const std::string& file);
	void stop(const std::string& file);
	void pause(const std::string& file);
	void resume(const std::string& file);
	void setVolume(float volume);
	void resetStack();

	//////////////////////////////////////////old approach without cache /////////////////////////////
	void playChannel(unsigned int channel);
	void stopChannel(unsigned int channel);
	void replayChannel(unsigned int channel);
	void pauseChannel(unsigned int channel);
	void resumeChannel(unsigned int channel);
	void setVolumeChannel(unsigned int channel, float volume);
	void setPitchChannel(unsigned int channel, float pitvh);
	void loadChannel(const std::string& file, unsigned int channel);
	bool isPlaying(unsigned int channel);
	void setLoopingChannel(unsigned int channel, const bool& loop);

	void cleanup();
	static void Init(unsigned short cacheSizeBuffer = 10u);

private:

	ALuint m_source;
	ALint m_playState;

	unsigned short m_bufferIndex;
	unsigned short m_sourceIndex;

	unsigned short m_maxChannels;
	unsigned short m_parallelIndex;
	unsigned short m_sourceCutoff;

	bool m_sourceInit = false;

	std::vector<ALuint> m_sources;
	std::vector<ALuint> m_buffers;
	CacheLRU<std::string, SoundBuffer::CacheEntrySource> m_soundSourceCache;

	static std::vector<ALuint> Buffer;
	static CacheLRU<std::string, SoundBuffer::CacheEntryBuffer> SoundBufferCache;
	static unsigned short Instances;

};

