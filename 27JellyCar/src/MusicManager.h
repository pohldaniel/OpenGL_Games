#pragma once
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>

class MusicManager {

public:

	enum AudioHelperSoundEnum{
		Car,
		Sounds,
		Music
	};

	void init(float carVolume, float soundsVolume, float musicVolume);
	void playHitSound();
	void playMusic();
	void stopMusic();
	void playFastEngine();
	void playSlowEngine();
	void stopEngineSound();

	void setVolume(AudioHelperSoundEnum sound, float volume);
	float getVolume(AudioHelperSoundEnum sound);

	static MusicManager& Get();

private:

	MusicManager() = default;
	void loadSounds();

	float m_carVolume;
	float m_soundsVolume;
	float m_musicVolume;
	bool m_musicPlaying;

	std::vector<std::string> m_soundList;
	std::vector<std::string> m_musicList;

	static MusicManager s_instance;
};