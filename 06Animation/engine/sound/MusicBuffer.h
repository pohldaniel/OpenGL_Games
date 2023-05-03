#pragma once
#include <AL\al.h>
#include <sndfile.h>
#include <string>
#include <iostream>

class MusicBuffer{
public:

	MusicBuffer() =default;
	MusicBuffer(MusicBuffer const& rhs);
	MusicBuffer& operator=(const MusicBuffer& rhs);

	~MusicBuffer();

	void play();
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
	SNDFILE* m_sndFile = nullptr;
	SF_INFO m_sfinfo;
	short* m_membuf = nullptr;
	std::size_t m_frameSize = 0;
	ALenum m_format;
	bool  m_loop = false;;	
};

