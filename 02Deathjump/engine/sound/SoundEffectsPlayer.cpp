#include "SoundEffectsPlayer.h"

void SoundEffectsPlayer::init() {
	alGenSources(1, &m_source);
}

SoundEffectsPlayer::~SoundEffectsPlayer() {
	alDeleteSources(1, &m_source);
}

void SoundEffectsPlayer::play(const ALuint& buffer_to_play) {
	m_buffer = buffer_to_play;
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