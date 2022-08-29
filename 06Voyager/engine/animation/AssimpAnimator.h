#pragma once

#include <memory>
#include <unordered_map>
#include "../Vector.h"

class AssimpAnimation;
class AssimpAnimatedModel;

class AssimpAnimator {

public:
	AssimpAnimator(AssimpAnimatedModel* model);

	void startAnimation(const std::string& animationName);
	void addAnimation(AssimpAnimation* animation);
	void update(float elapsedTime);

	void blendTwoAnimations(std::string pBaseAnimation, std::string pLayeredAnimation, float blendFactor, float deltaTime);

private:
	AssimpAnimatedModel* m_model;

	std::vector<std::shared_ptr<AssimpAnimation>> m_animations;
	std::shared_ptr<AssimpAnimation> m_currentAnimation;
	float m_animationTime;
	unsigned int m_ticksPerSecond = 0;

	std::unordered_map<std::string, Matrix4f> calculateCurrentAnimationPose();
	std::unordered_map<std::string, Matrix4f> calculateBlendedAnimationPose(const float currentTimeBase, const float currentTimeLayered, const float blendFactor);

	Vector3f getInterpolated(Vector3f start, Vector3f end, float progression);
	Quaternion interpolateQuat(Quaternion a, Quaternion b, float blend);
	float getProgression(float lastTimeStamp, float nextTimeStamp, float animationTime);
};

