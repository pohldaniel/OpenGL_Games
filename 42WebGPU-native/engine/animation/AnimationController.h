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

	bool play(const std::string& name, unsigned char layer, bool looped, float fadeInTime = 0.0f);
	bool playExclusive(const std::string& name, unsigned char layer, bool looped, float fadeTime = 0.0f);
	bool stop(const std::string& name, float fadeOutTime = 0.0f);
	void stopLayer(unsigned char layer, float fadeOutTime = 0.0f);
	void stopAll(float fadeTime = 0.0f);
	bool fade(const std::string& name, float targetWeight, float fadeTime);
	bool fadeOthers(const std::string& name, float targetWeight, float fadeTime);
	bool fadeOtherExclusive(const std::string& targetName, float targetWeight, const float restTime, float weightOffset = 0.0f);
	bool setTime(const std::string& name, float time);
	bool isAtEnd(const std::string& name) const;

	bool setSpeed(const std::string& name, float speed);
	float getTime(const std::string& name) const;
	float getRestTime(const std::string& name);

	AnimationState* getAnimationState(const std::string& name) const;
	AnimationState* addAnimationState(const Animation& animation);
	AnimationState* addAnimationStateFront(const Animation& animation, bool invertWeight = false, float weightOffset = 0.0f, float fadeTimeOffset = 0.0f);
	AnimationState* addAnimationStateFront2(const Animation& animation, bool invertWeight = false, float weightOffset = 0.0f, float fadeTimeOffset = 0.0f);
	const bool hasAnimationControl(const std::string& name) const;

	void removeAnimationState(AnimationState* state);
	void findAnimation(const std::string& name, unsigned& index, AnimationState*& state) const;
	AnimatedModel* getAnimationNode();

	std::vector<AnimationControl> m_animationControls;

private:

	
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