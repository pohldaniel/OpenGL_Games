#pragma once
#include <AL\al.h>
#include <sndfile.h>
#include <string>
#include <iostream>

class MusicBuffer
{
public:

	MusicBuffer() =default;
	MusicBuffer(MusicBuffer const& rhs);
	MusicBuffer& operator=(const MusicBuffer& rhs);

	~MusicBuffer();

	void Play();
	void Pause();
	void Stop();
	void Resume();

	void UpdateBufferStream();

	ALint getSource();

	bool isPlaying();
	void SetLooping(const bool& loop);
	void loadFromFile(const std::string& path);
	void setVolume(float volume);
private:
	ALuint p_Source;
	static const int BUFFER_SAMPLES = 8192;
	static const int NUM_BUFFERS = 4;
	ALuint p_Buffers[NUM_BUFFERS];
	SNDFILE* p_SndFile = nullptr;
	SF_INFO p_Sfinfo;
	short* p_Membuf = nullptr;
	std::size_t m_frameSize = 0;
	ALenum p_Format;
	bool  m_loop = false;;
	
};

