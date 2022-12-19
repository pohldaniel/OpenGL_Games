#include "SoundBuffer.h"
#include "sndfile.h"
#include <inttypes.h>
#include "AL\alext.h"
#include <iostream>

CacheLRU<std::string, SoundBuffer::CacheEntryBuffer> SoundBuffer::SoundBufferCache;
std::vector<ALuint> SoundBuffer::Buffer;
bool SoundBuffer::CacheInit = false;
unsigned short SoundBuffer::Instances = 0;

SoundBuffer::SoundBuffer(SoundBuffer const& rhs) : m_soundSourceCache(rhs.m_soundSourceCache) {}

SoundBuffer& SoundBuffer::operator=(const SoundBuffer& rhs) {	
	m_soundSourceCache = rhs.m_soundSourceCache;
	return *this;
}

SoundBuffer::SoundBuffer() {
	Instances++;
}

void SoundBuffer::init(unsigned short cacheSizeBuffer, unsigned short cacheSizeSources, unsigned short channelSize, float volume) {

	if (!m_sourceInit) {
		m_soundSourceCache.Init(cacheSizeSources);

		m_sources = std::vector<ALuint>(channelSize);
		alGenSources(channelSize, m_sources.data());

		setVolume(volume);
		m_source = m_sources[0];
		m_sourceInit = true;
	}

	if (!CacheInit) {
		SoundBufferCache.Init(cacheSizeBuffer);
		Buffer = std::vector<ALuint>(cacheSizeBuffer);

		alGenBuffers(cacheSizeBuffer, Buffer.data());
		CacheInit = true;
	}
	
}

SoundBuffer::~SoundBuffer() {
	Instances--;

	if(m_sourceInit)
		alDeleteSources(10, m_sources.data());

	if (Instances == 0) {
		alDeleteBuffers(10, Buffer.data());
	}
	
}

void SoundBuffer::play(const std::string& file) {
	SoundBufferCache.Put(file);
	const CacheEntryBuffer& cacheEntryBuffer = SoundBufferCache.Get(file);

	m_soundSourceCache.Put(file);
	const CacheEntrySource& cacheEntrySource = m_soundSourceCache.Get(file);

	if (m_bufferIndex != cacheEntryBuffer.index) {
		m_bufferIndex = cacheEntryBuffer.index;
	}

	if (m_sourceIndex != cacheEntrySource.index) {
		m_sourceIndex = cacheEntrySource.index;
	}

	alGetSourcei(m_source, AL_SOURCE_STATE, &m_playState);

	if (m_playState == AL_PLAYING) {
		return;
	}

	m_source = m_sources[m_sourceIndex];

	alSourcei(m_source, AL_BUFFER, (ALint)Buffer[m_bufferIndex]);
	alSourcePlay(m_source);
}

void SoundBuffer::playParallel(const std::string& file) {
	SoundBufferCache.Put(file);
	const CacheEntryBuffer& cacheEntryBuffer = SoundBufferCache.Get(file);

	m_soundSourceCache.Put(file);
	const CacheEntrySource& cacheEntrySource = m_soundSourceCache.Get(file);


	if (m_bufferIndex != cacheEntryBuffer.index) {
		m_bufferIndex = cacheEntryBuffer.index;
	}

	if (m_sourceIndex != cacheEntrySource.index) {
		m_sourceIndex = cacheEntrySource.index;
	}

	ALint playState;
	alGetSourcei(m_sources[m_sourceIndex], AL_SOURCE_STATE, &playState);

	if (playState != AL_PLAYING) {
		alSourcei(m_sources[m_sourceIndex], AL_BUFFER, (ALint)Buffer[m_bufferIndex]);
		alSourcePlay(m_sources[m_sourceIndex]);
	}
}

void SoundBuffer::setVolume(float volume) {
	
	for (auto const& source : m_sources) {
		alSourcef(source, AL_GAIN, volume);
	}
}

void SoundBuffer::stop(const std::string& file) {
	const CacheEntrySource& cacheEntrySource = m_soundSourceCache.Get(file);
	alSourceStop(m_sources[cacheEntrySource.index]);
}

void SoundBuffer::pause(const std::string& file) {
	const CacheEntrySource& cacheEntrySource = m_soundSourceCache.Get(file);
	alSourcePause(m_sources[cacheEntrySource.index]);
}

void SoundBuffer::resume(const std::string& file) {
	const CacheEntrySource& cacheEntrySource = m_soundSourceCache.Get(file);
	alSourcePlay(m_sources[cacheEntrySource.index]);
}

void SoundBuffer::playChannel(unsigned int channel) {
	
	ALint playState;
	alGetSourcei(m_sources[channel], AL_SOURCE_STATE, &playState);

	if (playState != AL_PLAYING) {
		alSourcePlay(m_sources[channel]);
	}
}

void SoundBuffer::stopChannel(unsigned int channel) {	
	alSourceStop(m_sources[channel]);
}

void SoundBuffer::pauseChannel(unsigned int channel) {
	alSourcePause(m_sources[channel]);
}

void SoundBuffer::resumeChannel(unsigned int channel) {
	alSourcePlay(m_sources[channel]);
}

void SoundBuffer::setVolumeChannel(unsigned int channel, float volume){
	alSourcef(m_sources[channel], AL_GAIN, volume);
}

void SoundBuffer::loadChannel(const std::string& file, unsigned int channel) {
	ALenum err, format;
	SNDFILE* sndfile;
	SF_INFO sfinfo;
	short* membuf;
	sf_count_t num_frames;
	ALsizei num_bytes;
	ALuint buffer;

	// Open the audio file and check that it's usable.
	sndfile = sf_open(file.c_str(), SFM_READ, &sfinfo);
	if (!sndfile) {
		fprintf(stderr, "Could not open audio in %s: %s\n", file.c_str(), sf_strerror(sndfile));
		return;
	}

	if (sfinfo.frames < 1 || sfinfo.frames >(sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels) {
		fprintf(stderr, "Bad sample count in %s (%" PRId64 ")\n", file.c_str(), sfinfo.frames);
		sf_close(sndfile);
		return;
	}

	// Get the sound format, and figure out the OpenAL format */
	format = AL_NONE;
	if (sfinfo.channels == 1)
		format = AL_FORMAT_MONO16;
	else if (sfinfo.channels == 2)
		format = AL_FORMAT_STEREO16;
	else if (sfinfo.channels == 3) {
		if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
			format = AL_FORMAT_BFORMAT2D_16;
	} else if (sfinfo.channels == 4) {
		if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
			format = AL_FORMAT_BFORMAT3D_16;
	}

	if (!format) {
		fprintf(stderr, "Unsupported channel count: %d\n", sfinfo.channels);
		sf_close(sndfile);
		return;
	}

	// Decode the whole audio file to a buffer.
	membuf = static_cast<short*>(malloc((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short)));

	num_frames = sf_readf_short(sndfile, membuf, sfinfo.frames);
	if (num_frames < 1) {
		free(membuf);
		sf_close(sndfile);
		fprintf(stderr, "Failed to read samples in %s (%" PRId64 ")\n", file.c_str(), num_frames);
		return;
	}
	num_bytes = (ALsizei)(num_frames * sfinfo.channels) * (ALsizei)sizeof(short);

	// Buffer the audio data into a new buffer object, then free the data and close the file.

	alGenBuffers(1, &buffer);
	alBufferData(buffer, format, membuf, num_bytes, sfinfo.samplerate);
	alSourcei(m_sources[channel], AL_BUFFER, (ALint)buffer);
	
	alDeleteBuffers(1, &buffer);
	free(membuf);
	sf_close(sndfile);

	// Check if an error occured, and clean up if so.
	err = alGetError();
	if (err != AL_NO_ERROR) {
		fprintf(stderr, "OpenAL Error: %s\n", alGetString(err));
		if (buffer && alIsBuffer(buffer))
			alDeleteBuffers(1, &buffer);
		return;
	}
}

SoundBuffer::CacheEntryBuffer::CacheEntryBuffer(const std::string& file, const size_t _index) {
	ALenum format;
	SF_INFO sfinfo;
	SNDFILE* sndFile = sf_open(file.c_str(), SFM_READ, &sfinfo);
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
	} else if (sfinfo.channels == 4) {
		if (sf_command(sndFile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
			format = AL_FORMAT_BFORMAT3D_16;
	}

	if (!format) {
		sf_close(sndFile);
		sndFile = NULL;
		throw("Unsupported channel count from file");
	}

	// Decode the whole audio file to a buffer.
	membuf = static_cast<short*>(malloc((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short)));

	num_frames = sf_readf_short(sndFile, membuf, sfinfo.frames);
	num_bytes = (ALsizei)(num_frames * sfinfo.channels) * (ALsizei)sizeof(short);

	index = static_cast<unsigned short>(_index);	
	alBufferData(Buffer[index], format, membuf, num_bytes, sfinfo.samplerate);

	free(membuf);
	sf_close(sndFile);
}

SoundBuffer::CacheEntrySource::CacheEntrySource(const std::string& file, const size_t _index) {
	index = static_cast<unsigned short>(_index);
}