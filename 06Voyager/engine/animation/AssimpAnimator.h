#pragma once

#include <map>
#include <memory>
#include <unordered_map>
#include "../Vector.h"

class AssimpAnimation;
class AssimpAnimatedModel;

struct AssimpKeyFrameBaseData {
	std::map<std::string, Vector3f> positions;
	std::map<std::string, Vector3f> scales;
	std::map<std::string, Quaternion> rotations;
};


class AssimpAnimator {

public:
	AssimpAnimator(AssimpAnimatedModel* model);

	void startAnimation(const std::string& animationName);
	void addAnimation(AssimpAnimation* animation);
	void update(float elapsedTime);

	void blendTwoAnimations(std::string pBaseAnimation, std::string pLayeredAnimation, float blendFactor, float deltaTime);
	void addTwoAnimations(float addTime, std::string base, std::string layer);
private:
	AssimpAnimatedModel* m_model;

	//std::vector<std::shared_ptr<AssimpAnimation>> m_animations;
	std::unordered_map<std::string, std::shared_ptr<AssimpAnimation>> m_animations;

	std::shared_ptr<AssimpAnimation> m_currentAnimation;
	std::shared_ptr<AssimpAnimation> m_layeredAnimation;
	float m_animationTime;
	float m_ticksPerSecond = 0;
	bool m_replaceBasePose = true;

	AssimpKeyFrameBaseData m_basePose;

	std::unordered_map<std::string, Matrix4f> calculateCurrentAnimationPose();
	std::unordered_map<std::string, Matrix4f> calculateBlendedAnimationPose(const float currentTimeBase, const float currentTimeLayered, AssimpAnimation& animationBase, AssimpAnimation& animationAddLayer, const float blendFactor);
	std::unordered_map<std::string, Matrix4f> calculateCurrentAnimationPose(float time, float addTime, AssimpAnimation& animation, AssimpAnimation& animationAdd);

	Vector3f getInterpolated(Vector3f start, Vector3f end, float progression);
	Quaternion interpolateQuat(Quaternion a, Quaternion b, float blend);
	float getProgression(float lastTimeStamp, float nextTimeStamp, float animationTime);
	void replaceBasePose(AssimpAnimation& animation);

	std::string m_current;
	std::string m_layer;

	float m_additiveTime = 0.0f;
	float m_additiveDirection = 1.0f;
};

