#pragma once

#include <fstream>
#include <sstream>
#include <filesystem>

#include <engine/Rect.h>
#include <unordered_map>

#include "Enums.h"

enum AnimationLoopState {
	ONCE,
	REPEAT
};

struct AnimationFrame {
	const TextureRect& rect;
	float normalizedTime;
};

struct Animation {

	friend class AnimationManager;

	void loadAnimation16(std::string name, const std::vector<TextureRect>& textureRects);
	void loadAnimation8(std::string name, const std::vector<TextureRect>& textureRects);
	const std::vector<AnimationFrame>& getAnimationFrames() const;
	const float getDuration() const;
	const unsigned short getTexturesPerDirection(int direction) const;
	const AnimationLoopState getAnimationLoopState() const;

private:

	std::vector<AnimationFrame> m_animationFrames;
	std::unordered_map<int, unsigned short> m_texturesPerDirection;
	AnimationLoopState m_animationLoopState;
	float m_duration;
};

class AnimationManager {

public:

	Animation& getAnimation(std::string name);
	static AnimationManager& Get();

	void loadAnimation16(std::string name, const std::vector<TextureRect>& textureRects);
	void loadAnimation8(std::string name, const std::vector<TextureRect>& textureRects);

private:
	AnimationManager() = default;

	std::unordered_map<std::string, Animation> m_animations;
	static AnimationManager s_instance;
};