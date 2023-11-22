#pragma once
#include <unordered_map>
#include "Constants.h"
#include "engine/Clock.h"
#include "engine/ui/Text.h"

class Countdown  {
public:
	Countdown();
	~Countdown();

	void render();
	void update();

	unsigned currentFrame() const;
private:
	
	Text* m_counter;
	Clock clock;

	unsigned m_currentFrame = 0;

	std::unordered_map<std::string, SoundBuffer> sounds;

	std::string str[4] ={"3", "2", "1", "GO!"};

	bool isRunning = true;
	void initCounter();

	SoundEffectsPlayer m_effectsPlayer;
};
