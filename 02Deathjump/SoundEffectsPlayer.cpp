#include "SoundEffectsPlayer.h"
#include <iostream>

void SoundEffectsPlayer::init() {
	alGenSources(1, &p_Source);
}

SoundEffectsPlayer::~SoundEffectsPlayer(){
	alDeleteSources(1, &p_Source);
}

void SoundEffectsPlayer::Play(const ALuint& buffer_to_play){
	p_Buffer = buffer_to_play;
	alSourcei(p_Source, AL_BUFFER, (ALint)p_Buffer);
	alSourcePlay(p_Source);
}

void SoundEffectsPlayer::setVolume(float volume) {
	alSourcef(p_Source, AL_GAIN, volume);
}

void SoundEffectsPlayer::Stop(){
	alSourceStop(p_Source);
}

void SoundEffectsPlayer::Pause(){
	alSourcePause(p_Source);
}

void SoundEffectsPlayer::Resume(){
	alSourcePlay(p_Source);
}

/*void SoundEffectsPlayer::SetBufferToPlay(const ALuint& buffer_to_play){
	if (buffer_to_play != p_Buffer)
	{
		p_Buffer = buffer_to_play;
		alSourcei(p_Source, AL_BUFFER, (ALint)p_Buffer);
	}
}*/

void SoundEffectsPlayer::SetLooping(const bool& loop){
	alSourcei(p_Source, AL_LOOPING, (ALint)loop);
}

bool SoundEffectsPlayer::isPlaying(){	
	ALint playState;
	alGetSourcei(p_Source, AL_SOURCE_STATE, &playState);
	return (playState == AL_PLAYING);
}
