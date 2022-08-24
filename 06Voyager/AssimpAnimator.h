#pragma once

#include <memory>
#include <unordered_map>
#include "engine/Vector.h"

class AssimpAnimation;
class AssimpAnimatedModel;

class AssimpAnimator {

public:
	AssimpAnimator(AssimpAnimatedModel* model);

	void startAnimation(const std::string& animationName);
	void addAnimation(AssimpAnimation* animation);
	void update(double elapsedTime);

private:
	AssimpAnimatedModel* m_model;

	std::vector<std::shared_ptr<AssimpAnimation>> m_animations;
	std::shared_ptr<AssimpAnimation> m_currentAnimation;
	double m_animationTime;

	std::unordered_map<std::string, Matrix4f> calculateCurrentAnimationPose();

	Vector3f GetInterpolated(Vector3f start, Vector3f end, float progression);
	Quaternion interpolateQuat(Quaternion a, Quaternion b, float blend);
};

