#include "SoundBuffer.h"
#include "sndfile.h"
#include <inttypes.h>
#include "AL\alext.h"
#include <iostream>

CacheLRU<std::string, SoundBuffer::CacheEntryBuffer> SoundBuffer::SoundBufferCache = CacheLRU<std::string, SoundBuffer::CacheEntryBuffer>(20);
std::vector<ALuint> SoundBuffer::Buffer = std::vector<ALuint>(20);
bool SoundBuffer::CacheInit = false;
unsigned short SoundBuffer::Instances = 0;

SoundBuffer::SoundBuffer(SoundBuffer const& rhs) : m_soundSourceCache(rhs.m_soundSourceCache) {}

SoundBuffer& SoundBuffer::operator=(const SoundBuffer& rhs) {	
	m_soundSourceCache = rhs.m_soundSourceCache;
	return *this;
}

SoundBuffer::SoundBuffer() :m_soundSourceCache(CacheLRU<std::string, SoundBuffer::CacheEntrySource>(10)) {
	Instances++;
}

void SoundBuffer::init() {

	if (!m_sourceInit) {
		m_sources = std::vector<ALuint>(10);
		alGenSources(10, m_sources.data());
		m_source = m_sources[0];
		m_sourceInit = true;
	}

	if (!CacheInit) {
		alGenBuffers(10, Buffer.data());
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

void SoundBuffer::playChanneled(const std::string& file) {
	SoundBufferCache.Put(file);
	const CacheEntryBuffer& cacheEntryBuffer = SoundBufferCache.Get(file);

	m_soundSourceCache.Put(file);
	const CacheEntrySource& cacheEntrySource = m_soundSourceCache.Get(file);


	if (m_bufferIndex != cacheEntryBuffer.index) {
		m_bufferIndex = cacheEntryBuffer.index;
		//m_buffer = cacheEntry.m_buffer;
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

SoundBuffer::CacheEntryBuffer::CacheEntryBuffer(const std::string& path, const size_t _index) {
	ALenum format;
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
	} else if (sfinfo.channels == 4) {
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

	index = static_cast<unsigned short>(_index);	
	alBufferData(Buffer[index], format, membuf, num_bytes, sfinfo.samplerate);

	free(membuf);
	sf_close(sndFile);
}

SoundBuffer::CacheEntrySource::CacheEntrySource(const std::string& path, const size_t _index) {
	index = static_cast<unsigned short>(_index);
}