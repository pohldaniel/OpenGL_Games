#include "MusicBuffer.h"

CacheLRUP<std::string, MusicBuffer::CacheEntry> MusicBuffer::MusicBufferCache;

MusicBuffer::MusicBuffer(MusicBuffer const& rhs) {

	//m_source = rhs.m_source;	
	//m_buffers[0] = rhs.m_buffers[0];
	//m_buffers[1] = rhs.m_buffers[1];
	//m_buffers[2] = rhs.m_buffers[2];
	//m_buffers[3] = rhs.m_buffers[3];

	//alGenSources(1, &m_source);
	//alGenBuffers(NUM_BUFFERS, m_buffers);
}

MusicBuffer& MusicBuffer::operator=(const MusicBuffer& rhs) {

	//m_source = rhs.m_source;
	//m_buffers[0] = rhs.m_buffers[0];
	//m_buffers[1] = rhs.m_buffers[1];
	//m_buffers[2] = rhs.m_buffers[2];
	//m_buffers[3] = rhs.m_buffers[3];

	//alGenSources(1, &m_source);
	//alGenBuffers(NUM_BUFFERS, m_buffers);


	return *this;
}

MusicBuffer::MusicBuffer() {}

void MusicBuffer::create(float volume) {

	if (!m_sourceInit) {
		alGenSources(1, &m_source);
		alGenBuffers(NUM_BUFFERS, m_buffers);
		setVolume(volume);
		m_sourceInit = true;
		m_looper = std::make_unique<EndlessLooper>();
	}
}


MusicBuffer::~MusicBuffer() {
	if (m_sourceInit) {
		cleanup();
	}
}

void MusicBuffer::cleanup() {
	if (m_looper->running())
		m_looper->stop();

	alSourcei(m_source, AL_BUFFER, 0);
	alDeleteSources(1, &m_source);
	alDeleteBuffers(NUM_BUFFERS, m_buffers);
	m_source = 0;
	m_sourceInit = false;	
}

void MusicBuffer::play(const std::string& file) {

	MusicBufferCache.Put(file);
	const CacheEntry* cacheEntry = MusicBufferCache.Get(file);

	if (m_cacheEntry != cacheEntry) {
		m_sndFile = cacheEntry->sndFile;
		m_sfinfo = cacheEntry->sfinfo;
		m_membuf = cacheEntry->membuf;
		m_frameSize = cacheEntry->frameSize;
		m_format = cacheEntry->format;
		m_cacheEntry = cacheEntry;
		
	}

	alGetError();

	// Rewind the source position and clear the buffer queue 
	alSourceRewind(m_source);
	alSourcei(m_source, AL_BUFFER, 0);
	sf_seek(m_sndFile, 0, SF_SEEK_SET);
	// Fill the buffer queue/
	for (ALsizei i = 0; i < NUM_BUFFERS; i++) {
		// Get some data to give it to the buffer 
		sf_count_t slen = sf_readf_short(m_sndFile, m_membuf, BUFFER_SAMPLES);
		if (slen < 1) break;

		slen *= m_sfinfo.channels * (sf_count_t)sizeof(short);
		alBufferData(m_buffers[i], m_format, m_membuf, (ALsizei)slen, m_sfinfo.samplerate);
	}

	if (alGetError() != AL_NO_ERROR) {
		throw("Error buffering for playback");
	}

	// Now queue and start playback!
	alSourceQueueBuffers(m_source, NUM_BUFFERS, m_buffers);
	alSourcePlay(m_source);
	if (alGetError() != AL_NO_ERROR) {
		throw("Error starting playback");
	}
}

void MusicBuffer::setVolume(float volume) {
	alSourcef(m_source, AL_GAIN, volume);
}

void MusicBuffer::pause() {
	alSourcePause(m_source);
}

void MusicBuffer::stop() {
	alSourceStop(m_source);
	sf_seek(m_sndFile, 0, SF_SEEK_SET);
}

void MusicBuffer::resume() {

	alGetSourcei(m_source, AL_SOURCE_STATE, &m_playState);

	if (m_playState == AL_PLAYING) {
		return;
	}
	alSourcePlay(m_source);
}

void MusicBuffer::updateBufferStream() {
	ALint processed, state;

	// clear error 
	alGetError();
	// Get relevant source info 
	alGetSourcei(m_source, AL_SOURCE_STATE, &state);
	alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processed);
	if (alGetError() != AL_NO_ERROR) {
		throw("error checking music source state");
	}

	// Unqueue and handle each processed buffer 
	while (processed > 0 && state != AL_STOPPED) {
		ALuint bufid;
		sf_count_t slen;
		alSourceUnqueueBuffers(m_source, 1, &bufid);
		processed--;

		// Read the next chunk of data, refill the buffer, and queue it
		// back on the source
		slen = sf_readf_short(m_sndFile, m_membuf, BUFFER_SAMPLES);
		if (slen > 0) {
			slen *= m_sfinfo.channels * (sf_count_t)sizeof(short);

			alBufferData(bufid, m_format, m_membuf, (ALsizei)slen, m_sfinfo.samplerate);
			alSourceQueueBuffers(m_source, 1, &bufid);

			if (slen < BUFFER_SAMPLES && m_loop) {
				sf_seek(m_sndFile, 0, SF_SEEK_SET);
			}
		} else if (m_loop) {
			sf_seek(m_sndFile, 0, SF_SEEK_SET);
			slen = sf_readf_short(m_sndFile, m_membuf, BUFFER_SAMPLES);

			slen *= m_sfinfo.channels * (sf_count_t)sizeof(short);

			alBufferData(bufid, m_format, m_membuf, (ALsizei)slen, m_sfinfo.samplerate);
			alSourceQueueBuffers(m_source, 1, &bufid);
		}
		if (alGetError() != AL_NO_ERROR) {
			throw("error buffering music data");
		}
	}

	// Make sure the source hasn't underrun 
	if (state != AL_PLAYING && state != AL_PAUSED && state != AL_STOPPED) {
		ALint queued;
		// If no buffers are queued, playback is finished 
		alGetSourcei(m_source, AL_BUFFERS_QUEUED, &queued);
		if (queued == 0)
			return;

		alSourcePlay(m_source);
		if (alGetError() != AL_NO_ERROR) {
			throw("error restarting music playback");
		}
	}
}

ALint MusicBuffer::getSource() {
	return m_source;
}

bool MusicBuffer::isPlaying() {
	ALint state;
	alGetSourcei(m_source, AL_SOURCE_STATE, &state);
	return (state == AL_PLAYING);
}

void  MusicBuffer::setLooping(const bool& loop) {
	m_loop = loop;
}

MusicBuffer::CacheEntry::CacheEntry(const std::string& path) {
	sndFile = sf_open(path.c_str(), SFM_READ, &sfinfo);
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

	frameSize = ((size_t)BUFFER_SAMPLES * (size_t)sfinfo.channels) * sizeof(short);
	membuf = static_cast<short*>(malloc(frameSize));
}

MusicBuffer::CacheEntry::~CacheEntry() {
	if (sndFile) {
		sf_close(sndFile);
		sndFile = nullptr;
	}

	free(membuf);
	membuf = nullptr;
}

void MusicBuffer::Init(unsigned short cacheSize) {	
	MusicBufferCache.Init(cacheSize);
}

void MusicBuffer::run() {
	m_looper->run();
	auto const task = [&]() {
		updateBufferStream();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	};

	m_looper->addTask(task);
}