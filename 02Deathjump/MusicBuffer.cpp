#include "MusicBuffer.h"
#include <cstddef>
#include <AL\alext.h>
#include <malloc.h>

MusicBuffer::MusicBuffer(MusicBuffer const& rhs) {
	if (rhs.m_frameSize > 0) {
		m_frameSize = rhs.m_frameSize;
		m_membuf = static_cast<short*>(malloc(rhs.m_frameSize));
		std::memcpy(m_membuf, rhs.m_membuf, rhs.m_frameSize);
	}
}

MusicBuffer& MusicBuffer::operator=(const MusicBuffer& rhs) {
	if (rhs.m_frameSize > 0) {
		m_frameSize = rhs.m_frameSize;
		m_membuf = static_cast<short*>(malloc(rhs.m_frameSize));
		std::memcpy(m_membuf, rhs.m_membuf, rhs.m_frameSize);
	}
	return *this;
}

void MusicBuffer::play(){
	alGetError();

	// Rewind the source position and clear the buffer queue 
	alSourceRewind(m_source);
	alSourcei(m_source, AL_BUFFER, 0);

	// Fill the buffer queue/
	for (ALsizei i = 0; i < NUM_BUFFERS; i++){
		// Get some data to give it to the buffer 
		sf_count_t slen = sf_readf_short(m_sndFile, m_membuf, BUFFER_SAMPLES);
		if (slen < 1) break;

		slen *= m_sfinfo.channels * (sf_count_t)sizeof(short);
		alBufferData(m_buffers[i], m_format, m_membuf, (ALsizei)slen, m_sfinfo.samplerate);
	}

	if (alGetError() != AL_NO_ERROR){
		throw("Error buffering for playback");
	}

	// Now queue and start playback!
	alSourceQueueBuffers(m_source, NUM_BUFFERS, m_buffers);
	alSourcePlay(m_source);
	if (alGetError() != AL_NO_ERROR){
		throw("Error starting playback");
	}
}

void MusicBuffer::setVolume(float volume) {
	alSourcef(m_source, AL_GAIN, volume);
}

void MusicBuffer::pause(){
	alSourcePause(m_source);
}

void MusicBuffer::stop(){
	alSourceStop(m_source);
	sf_seek(m_sndFile, 0, SF_SEEK_SET);
}

void MusicBuffer::resume(){
	alSourcePlay(m_source);
}

void MusicBuffer::updateBufferStream(){
	ALint processed, state;

	// clear error 
	alGetError();
	// Get relevant source info 
	alGetSourcei(m_source, AL_SOURCE_STATE, &state);
	alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processed);
	if (alGetError() != AL_NO_ERROR){
		throw("error checking music source state");
	}

	// Unqueue and handle each processed buffer 
	while (processed > 0 && state != AL_STOPPED){
		ALuint bufid;
		sf_count_t slen;
		alSourceUnqueueBuffers(m_source, 1, &bufid);
		processed--;

		// Read the next chunk of data, refill the buffer, and queue it
		// back on the source
		slen = sf_readf_short(m_sndFile, m_membuf, BUFFER_SAMPLES);
		if (slen > 0){		
			slen *= m_sfinfo.channels * (sf_count_t)sizeof(short);

			alBufferData(bufid, m_format, m_membuf, (ALsizei)slen, m_sfinfo.samplerate);
			alSourceQueueBuffers(m_source, 1, &bufid);

			if (slen < BUFFER_SAMPLES && m_loop) {
				sf_seek(m_sndFile, 0, SF_SEEK_SET);
			}
		}else if(m_loop){
			sf_seek(m_sndFile, 0, SF_SEEK_SET);
			slen = sf_readf_short(m_sndFile, m_membuf, BUFFER_SAMPLES);

			slen *= m_sfinfo.channels * (sf_count_t)sizeof(short);

			alBufferData(bufid, m_format, m_membuf, (ALsizei)slen, m_sfinfo.samplerate);
			alSourceQueueBuffers(m_source, 1, &bufid);
		}
		if (alGetError() != AL_NO_ERROR){
			throw("error buffering music data");
		}
	}

	// Make sure the source hasn't underrun 
	if (state != AL_PLAYING && state != AL_PAUSED && state != AL_STOPPED){
		ALint queued;
		// If no buffers are queued, playback is finished 
		alGetSourcei(m_source, AL_BUFFERS_QUEUED, &queued);
		if (queued == 0)
			return;

		alSourcePlay(m_source);
		if (alGetError() != AL_NO_ERROR){
			throw("error restarting music playback");
		}
	}
}

ALint MusicBuffer::getSource(){
	return m_source;
}

bool MusicBuffer::isPlaying(){
	ALint state;
	alGetSourcei(m_source, AL_SOURCE_STATE, &state);
	return (state == AL_PLAYING);
}

void  MusicBuffer::setLooping(const bool& loop) {
	m_loop = loop;
}

MusicBuffer::~MusicBuffer(){
	alDeleteSources(1, &m_source);
	alDeleteBuffers(NUM_BUFFERS, m_buffers);

	if (m_sndFile) {
		sf_close(m_sndFile);
		m_sndFile = nullptr;
	}

	//if (p_Membuf) {
		free(m_membuf);
		m_membuf = nullptr;
	//}	
}

void MusicBuffer::loadFromFile(const std::string& path) {
	alGenSources(1, &m_source);
	alGenBuffers(NUM_BUFFERS, m_buffers);

	m_sndFile = sf_open(path.c_str(), SFM_READ, &m_sfinfo);
	if (!m_sndFile){
		throw("could not open provided music file -- check path");
	}

	// Get the sound format, and figure out the OpenAL format
	if (m_sfinfo.channels == 1)
		m_format = AL_FORMAT_MONO16;
	else if (m_sfinfo.channels == 2)
		m_format = AL_FORMAT_STEREO16;
	else if (m_sfinfo.channels == 3){
		if (sf_command(m_sndFile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
			m_format = AL_FORMAT_BFORMAT2D_16;
	}
	else if (m_sfinfo.channels == 4){
		if (sf_command(m_sndFile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
			m_format = AL_FORMAT_BFORMAT3D_16;
	}
	if (!m_format){
		sf_close(m_sndFile);
		m_sndFile = NULL;
		throw("Unsupported channel count from file");
	}

	m_frameSize = ((size_t)BUFFER_SAMPLES * (size_t)m_sfinfo.channels) * sizeof(short);
	m_membuf = static_cast<short*>(malloc(m_frameSize));
}