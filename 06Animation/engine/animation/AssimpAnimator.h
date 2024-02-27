#pragma once

#include <map>
#include <set>
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

	void addTwoAnimations(float deltaTime, std::string current, std::string layer, float speed = 1.0f);
	void addTwoAnimationsDisjoint(float deltaTime, std::string current, std::string layer, float speed = 1.0f);
	void blendTwoAnimations(float deltaTime, std::string current, std::string layer, float speed = 1.0f);
	void blendTwoAnimationsDisjoint(float deltaTime, std::string current, std::string layer, float speed = 1.0f);

private:
	AssimpAnimatedModel* m_model;

	std::unordered_map<std::string, std::shared_ptr<AssimpAnimation>> m_animations;

	std::shared_ptr<AssimpAnimation> m_currentAnimation;
	std::shared_ptr<AssimpAnimation> m_layeredAnimation;
	float m_animationTime = 0.0f;
	float m_ticksPerSecond = 1.0f;
	bool m_replaceBasePose = true;

	AssimpKeyFrameBaseData m_basePose;

	std::unordered_map<std::string, Matrix4f> calculateCurrentAnimationPose(float currentTime);
	std::unordered_map<std::string, Matrix4f> calculateAdditiveAnimationPose(float currentTime, float layeredTime, AssimpAnimation& animation, AssimpAnimation& animationLayer);
	std::unordered_map<std::string, Matrix4f> calculateAdditiveAnimationPoseDisjoint(float currentTime, float layeredTime, AssimpAnimation& animation, AssimpAnimation& animationLayer);
	std::unordered_map<std::string, Matrix4f> calculateBlendedPose(float currentTime, float layeredTime, AssimpAnimation& curretAnimation, AssimpAnimation& animationLayer, float blendTime);
	std::unordered_map<std::string, Matrix4f> calculateBlendedPoseDisjoint(float currentTime, float layeredTime, AssimpAnimation& curretAnimation, AssimpAnimation& animationLayer, float blendTime);

	Vector3f getInterpolated(Vector3f start, Vector3f end, float progression);
	Quaternion interpolateQuat(Quaternion a, Quaternion b, float blend);
	float getProgression(float lastTimeStamp, float nextTimeStamp, float animationTime);
	void replaceBasePose(AssimpAnimation& animation);

	std::string m_current;
	std::string m_layer;

	float m_layeredTime = 0.0f;
	float m_additiveDirection = 1.0f;

	float mBlendTime;
	bool mInvertBlend;
};

