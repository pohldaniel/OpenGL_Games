#include "AudioService.h"

AudioService::AudioService() : m_fmodSystem(nullptr){
	// Init Fmod
	FMOD_RESULT fmodResult;
	fmodResult = FMOD::System_Create(&m_fmodSystem);
	if (fmodResult != FMOD_OK) {
		//spdlog::error("[FMOD] {} {}", fmodResult, FMOD_ErrorString(fmodResult));
		//debug_break();
	}
	fmodResult = m_fmodSystem->init(512, FMOD_INIT_NORMAL, 0);
	if (fmodResult != FMOD_OK) {
		//spdlog::error("[FMOD] {} {}", fmodResult, FMOD_ErrorString(fmodResult));
		//debug_break();
	}

	// Resize array to the number of items inside AudioFile enum
	m_sounds.resize(5);

	// Load sounds 
	FMOD::Sound* sound;
	if (FMOD_OK != m_fmodSystem->createSound("res/audio/musics/cpu-showdown.mp3", FMOD_DEFAULT, 0, &sound)) {
		//spdlog::error("[FMOD] Error while loading a sound");
		//debug_break();
	}
	m_sounds.at(AudioFiles::MUSIC_WAVE) = sound;

	if (FMOD_OK != m_fmodSystem->createSound("res/audio/effects/clink.mp3", FMOD_DEFAULT, 0, &sound)) {
		//spdlog::error("[FMOD] Error while loading a sound");
		//debug_break();
	}
	m_sounds.at(AudioFiles::EFFECT_CLICK) = sound;
}

AudioService::~AudioService() {
	for (std::size_t i = 0; i < m_sounds.size(); ++i) {
		FMOD::Sound* sound = m_sounds.at(i);
		sound->release();
	}
}

void AudioService::playSound(int soundID) {
	m_fmodSystem->playSound(m_sounds.at(soundID), 0, false, &m_channel);
}

void AudioService::stopSound(int soundID) {
}

void AudioService::stopAllSounds() {
	m_channel->stop();
}
