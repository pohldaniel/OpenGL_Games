#pragma once
#include <string>
#include "AnimationState.h"

struct AnimationControl {
	AnimationControl() :
		m_speed(1.0f),
		m_targetWeight(0.0f),
		m_fadeTime(0.0f),
		m_autoFadeTime(0.0f),
		m_removeOnCompletion(true),
		m_invertWeight(false),
		m_weightOffset(0.0f),
		m_fadeTimeOffset(0.0f)
	{
	}

	std::string m_name;
	float m_speed;
	float m_targetWeight;
	float m_fadeTime;
	float m_autoFadeTime;
	bool m_removeOnCompletion;

	bool m_invertWeight;
	float m_weightOffset;
	float m_fadeTimeOffset;
};

class AnimatedModel;
class AnimationController {

public:

	AnimationController(AnimatedModel* animatedModel);
	~AnimationController();

	void update(float dt);
	void fadeAndPlay(const std::string& target, float fadeInTime, float fadeBackTime = 0.0f);
	bool play(const std::string& name, bool looped, float fadeInTime = 0.0f);

	AnimatedModel* getAnimatedModel();

private:

	AnimationState* addAnimationStateFront(const Animation& animation, float fadeTime, float targetWeight, bool invertWeight = false, float weightOffset = 0.0f, float fadeTimeOffset = 0.0f);
	AnimationState* getAnimationState(const std::string& name) const;
	void findAnimation(const std::string& name, unsigned& index, AnimationState*& state) const;
	void removeAnimationState(AnimationState* state);

	std::vector<AnimationControl> m_animationControls;
	AnimatedModel* m_animatedModel;
};

class AnimationManager {

public:

	Animation& getAnimation(const std::string& name);
	void removeAnimation(const std::string& name);
	static AnimationManager& Get();

	bool containsAnimation(const std::string& name);

private:
	AnimationManager() = default;

	std::unordered_map<std::string, Animation> m_animations;
	static AnimationManager Instance;
};