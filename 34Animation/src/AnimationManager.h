#pragma once

#include "Utils/SolidIO.h"
#include "Enums.h"

class AnimationManager {

public:

	Utils::Animation& getAnimation(Enums::animation_type type);
	void loadAll(std::string path);
	bool m_loadAnimations;

	static AnimationManager& Get();
	static std::vector<Utils::Animation> animations;
private:

	AnimationManager();
	static AnimationManager s_instance;
};