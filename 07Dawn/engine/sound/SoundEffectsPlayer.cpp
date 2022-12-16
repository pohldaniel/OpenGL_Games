#include <iostream>
#include "SoundEffectsPlayer.h"


CacheLRU<std::string, CacheEntry2> SoundEffectsPlayer::SoundBufferCache = CacheLRU<std::string, CacheEntry2>(10);

void SoundEffectsPlayer::init() {
	alGenSources(1, &m_source);
}

SoundEffectsPlayer::~SoundEffectsPlayer(){
	alDeleteSources(1, &m_source);
}

void SoundEffectsPlayer::play(const ALuint& buffer_to_play){
	m_buffer = buffer_to_play;
	alSourcei(m_source, AL_BUFFER, (ALint)m_buffer);
	alSourcePlay(m_source);
}

void SoundEffectsPlayer::play(const std::string& file){
	SoundBufferCache.Put(file);
	const CacheEntry2* cacheEntry = SoundBufferCache.Get(file);

	if (m_cacheEntry != cacheEntry) {
	
	}
}

void SoundEffectsPlayer::setVolume(float volume) {
	alSourcef(m_source, AL_GAIN, volume);
}

void SoundEffectsPlayer::stop(){
	alSourceStop(m_source);
}

void SoundEffectsPlayer::pause(){
	alSourcePause(m_source);
}

void SoundEffectsPlayer::resume(){
	alSourcePlay(m_source);
}

void SoundEffectsPlayer::setLooping(const bool& loop){
	alSourcei(m_source, AL_LOOPING, (ALint)loop);
}

bool SoundEffectsPlayer::isPlaying(){	
	ALint playState;
	alGetSourcei(m_source, AL_SOURCE_STATE, &playState);
	return (playState == AL_PLAYING);
}

CacheEntry2::CacheEntry2(const std::string& path) {
	ALenum err, format;
	
	SF_INFO sfinfo;
	SNDFILE* sndFile = sf_open(path.c_str(), SFM_READ, &sfinfo);
	short* membuf;
	sf_count_t num_frames;
	ALsizei num_bytes;

	if (!sndFile) {
		throw("could not open provided music file -- check path");
	}

	// Get the sound format, and figure out the OpenAL format
	if (sfinfo.channels == 1)
		format = AL_FORMAT_MONO16;
	else if (sfinfo.channels == 2)
		format = AL_FORMAT_STEREO16;
	else if (sfinfo.channels == 3) {
		if (sf_command(sndFile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
			format = AL_FORMAT_BFORMAT2D_16;
	}
	else if (sfinfo.channels == 4) {
		if (sf_command(sndFile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
			format = AL_FORMAT_BFORMAT3D_16;
	}
	if (!format) {
		sf_close(sndFile);
		sndFile = NULL;
		throw("Unsupported channel count from file");
	}

	/* Decode the whole audio file to a buffer. */
	membuf = static_cast<short*>(malloc((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short)));
	num_frames = sf_readf_short(sndFile, membuf, sfinfo.frames);
	num_bytes = (ALsizei)(num_frames * sfinfo.channels) * (ALsizei)sizeof(short);

	alGenBuffers(1, &buffer);
	alBufferData(buffer, format, membuf, num_bytes, sfinfo.samplerate);

	free(membuf);
	sf_close(sndFile);
}

CacheEntry2::~CacheEntry2() {
	/*if (sndFile) {
		sf_close(sndFile);
		sndFile = nullptr;
	}*/

	
}