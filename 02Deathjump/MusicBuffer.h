#pragma once
#include <AL\al.h>
#include <sndfile.h>
#include <string>
class MusicBuffer
{
public:

	MusicBuffer() = default;
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
	
private:
	ALuint p_Source;
	static const int BUFFER_SAMPLES = 8192;
	static const int NUM_BUFFERS = 4;
	ALuint p_Buffers[NUM_BUFFERS];
	SNDFILE* p_SndFile;
	SF_INFO p_Sfinfo;
	short* p_Membuf;
	ALenum p_Format;

	
};

