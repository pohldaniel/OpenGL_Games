#pragma once

#include "Utils/SolidIO.h"

class AnimationManager {

public:

	Utils::Animation& getAnimation(Utils::animation_type type);
	void loadAll(std::string path);

	static AnimationManager& Get();
	static std::vector<Utils::Animation> animations;
private:

	AnimationManager();
	static AnimationManager s_instance;
};