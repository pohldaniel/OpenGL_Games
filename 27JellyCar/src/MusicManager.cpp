#include <Globals.h>

#include "MusicManager.h"

MusicManager MusicManager::s_instance;

void MusicManager::init(float carVolume, float soundsVolume, float musicVolume) {


	Globals::musicManager.get("background").setVolume(musicVolume);
	Globals::soundManager.get("hit").setVolume(soundsVolume);
	Globals::soundManager.get("car").setVolume(carVolume);

	m_carVolume = carVolume;
	m_soundsVolume = soundsVolume;
	m_musicVolume = musicVolume;
	m_musicPlaying = false;

	loadSounds();
	srand(time(NULL));
}

void MusicManager::loadSounds() {

	for (size_t i = 1; i < 14; i++){
		std::string hitName = "Assets/Jelly/Sounds/hit" + std::to_string(i) + ".wav";
		m_soundList.push_back(hitName);
	}

	for (size_t i = 1; i < 8; i++){
		std::string songName = "Assets/Jelly/Music/song" + std::to_string(i) + ".ogg";
		m_musicList.push_back(songName);
	}
}

void MusicManager::playHitSound(){
	if (m_soundsVolume > 0.0f){
		int soundNumber = rand() % 13;
		Globals::soundManager.get("hit").playStacked(m_soundList[soundNumber]);
	}
}

void MusicManager::playMusic(){

	if (m_musicVolume > 0.0f){
		if (Globals::musicManager.get("background").isPlaying()){			
			Globals::musicManager.get("background").stop();
		}

		int soundNumber = rand() % 7;
		Globals::musicManager.get("background").play(m_musicList[soundNumber]);
	}
}

void MusicManager::stopMusic(){

	if (m_musicPlaying){
		Globals::musicManager.get("background").stop();
		m_musicPlaying = false;
	}
}

void MusicManager::playFastEngine(){
	if (m_carVolume > 0.0f){
		Globals::soundManager.get("car").stopChannel(0u);
		Globals::soundManager.get("car").playChannel(1u);
	}
}

void MusicManager::playSlowEngine(){
	if (m_carVolume > 0.0f){
		Globals::soundManager.get("car").stopChannel(1u);
		Globals::soundManager.get("car").playChannel(0u);
	}
}

void MusicManager::stopEngineSound(){
	Globals::soundManager.get("car").stopChannel(0u);
	Globals::soundManager.get("car").stopChannel(1u);
}

void MusicManager::setVolume(AudioHelperSoundEnum sound, float volume){
	switch (sound){
		case Car:{
			if (m_carVolume <= 0.0f && volume > m_carVolume && !Globals::soundManager.get("car").isPlaying(1u)){
				Globals::soundManager.get("car").playChannel(1u);
			}

			m_carVolume = volume;

			Globals::soundManager.get("car").setVolumeChannel(0u, volume);
			Globals::soundManager.get("car").setVolumeChannel(1u, volume);
			break;

		}case Sounds:{
			m_soundsVolume = volume;
			Globals::soundManager.get("hit").setVolume(volume);
			break;

		}case Music:{
			m_musicVolume = volume;

			if (!Globals::musicManager.get("background").isPlaying()) {
				Globals::musicManager.get("background").resume();
			}

			Globals::musicManager.get("background").setVolume(volume);

			break;
		}default: break;
	}
}

float MusicManager::getVolume(AudioHelperSoundEnum sound){
	switch (sound){
		case Car:{
			return m_carVolume;
			break;
		}case Sounds:{
			return m_soundsVolume;
			break;
		}case Music:{
			return m_musicVolume;
			break;
		}		
		default: break;
	}
	return 0.0f;
}

MusicManager& MusicManager::Get() {
	return s_instance;
}