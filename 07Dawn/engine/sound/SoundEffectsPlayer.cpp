#include "SoundEffectsPlayer.h"

void SoundEffectsPlayer::init() {
	if (!m_sourceInit) {
		alGenSources(1, &m_source);
		m_sourceInit = true;
	}
}

SoundEffectsPlayer::~SoundEffectsPlayer() {
	if (m_sourceInit) {
		alDeleteSources(1, &m_source);
		alDeleteBuffers(1, &m_buffer);
	}
}

void SoundEffectsPlayer::play() {
	alSourcei(m_source, AL_BUFFER, (ALint)m_buffer);
	alSourcePlay(m_source);
}

void SoundEffectsPlayer::setVolume(float volume) {
	alSourcef(m_source, AL_GAIN, volume);
}

void SoundEffectsPlayer::stop() {
	alSourceStop(m_source);
}

void SoundEffectsPlayer::pause() {
	alSourcePause(m_source);
}

void SoundEffectsPlayer::resume() {
	alSourcePlay(m_source);
}

void SoundEffectsPlayer::setLooping(const bool& loop) {
	alSourcei(m_source, AL_LOOPING, (ALint)loop);
}

bool SoundEffectsPlayer::isPlaying() {
	ALint playState;
	alGetSourcei(m_source, AL_SOURCE_STATE, &playState);
	return (playState == AL_PLAYING);
}

void SoundEffectsPlayer::loadFromFile(const std::string& path) {
	ALenum err, format;
	SNDFILE* sndfile;
	SF_INFO sfinfo;
	short* membuf;
	sf_count_t num_frames;
	ALsizei num_bytes;

	/* Open the audio file and check that it's usable. */
	sndfile = sf_open(path.c_str(), SFM_READ, &sfinfo);
	if (!sndfile) {
		fprintf(stderr, "Could not open audio in %s: %s\n", path.c_str(), sf_strerror(sndfile));
		return;
	}

	if (sfinfo.frames < 1 || sfinfo.frames >(sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels) {
		fprintf(stderr, "Bad sample count in %s (%" PRId64 ")\n", path.c_str(), sfinfo.frames);
		sf_close(sndfile);
		return;
	}

	/* Get the sound format, and figure out the OpenAL format */
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

	/* Decode the whole audio file to a buffer. */
	membuf = static_cast<short*>(malloc((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short)));

	num_frames = sf_readf_short(sndfile, membuf, sfinfo.frames);
	if (num_frames < 1) {
		free(membuf);
		sf_close(sndfile);
		fprintf(stderr, "Failed to read samples in %s (%" PRId64 ")\n", path.c_str(), num_frames);
		return;
	}
	num_bytes = (ALsizei)(num_frames * sfinfo.channels) * (ALsizei)sizeof(short);

	/* Buffer the audio data into a new buffer object, then free the data and
	* close the file.
	*/
	m_buffer = 0;
	alGenBuffers(1, &m_buffer);
	alBufferData(m_buffer, format, membuf, num_bytes, sfinfo.samplerate);

	free(membuf);
	sf_close(sndfile);

	/* Check if an error occured, and clean up if so. */
	err = alGetError();
	if (err != AL_NO_ERROR) {
		fprintf(stderr, "OpenAL Error: %s\n", alGetString(err));
		if (m_buffer && alIsBuffer(m_buffer))
			alDeleteBuffers(1, &m_buffer);
		return;
	}
}