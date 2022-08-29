#include "AssimpAnimator.h"
#include <iostream>
#include "AssimpAnimatedModel.h"
#include "AssimpAnimation.h"

Vector3f AssimpAnimator::getInterpolated(Vector3f start, Vector3f end, float progression) {

	float x = start[0] + (end[0] - start[0]) * progression;
	float y = start[1] + (end[1] - start[1]) * progression;
	float z = start[2] + (end[2] - start[2]) * progression;
	return Vector3f(x, y, z);
}

Quaternion AssimpAnimator::interpolateQuat(Quaternion a, Quaternion b, float blend) {
	Quaternion result = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
	float dot = a[3] * b[3] + a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
	float blendI = 1.0f - blend;

	if (dot < 0.0f) {
		result[3] = blendI * a[3] + blend * -b[3];
		result[0] = blendI * a[0] + blend * -b[0];
		result[1] = blendI * a[1] + blend * -b[1];
		result[2] = blendI * a[2] + blend * -b[2];

	}else {
		result[3] = blendI * a[3] + blend * b[3];
		result[0] = blendI * a[0] + blend * b[0];
		result[1] = blendI * a[1] + blend * b[1];
		result[2] = blendI * a[2] + blend * b[2];
	}

	Quaternion::Normalize(result);
	return result;
}

float AssimpAnimator::getProgression(float lastTimeStamp, float nextTimeStamp, float animationTime) {
	float currentTime = animationTime - lastTimeStamp;
	float totalTime = nextTimeStamp - lastTimeStamp;	
	return currentTime / totalTime;
}


AssimpAnimator::AssimpAnimator(AssimpAnimatedModel *model) {
	m_model = model;
}

void AssimpAnimator::startAnimation(const std::string & animationName) {

	for (auto animation : m_animations) {

		if (animation->getName() == animationName) {
			m_animationTime = 0;
			m_currentAnimation = animation;
			m_ticksPerSecond = animation->m_ticksPerSecond;
		}
	}
}

void AssimpAnimator::addAnimation(AssimpAnimation* animation) {
	m_animations.push_back(std::shared_ptr<AssimpAnimation>(animation));
}

void AssimpAnimator::update(float elapsedTime) {
	
	m_animationTime += elapsedTime * m_ticksPerSecond;

	if (m_animationTime > m_currentAnimation->getDuration()) {
		m_animationTime = fmod(m_animationTime, m_currentAnimation->getDuration());
	}

	m_model->m_meshes[0]->currentPose = calculateCurrentAnimationPose();	
}

std::unordered_map<std::string, Matrix4f> AssimpAnimator::calculateCurrentAnimationPose() {

	AssimpKeyFrameData keyFrames = m_currentAnimation->m_keyFrames;

	std::unordered_map<std::string, Matrix4f> currentPose;
	for (auto boneName : m_currentAnimation->m_boneList) {
		int index = keyFrames.getPositionIndex(m_animationTime, boneName);
		float progression = getProgression(keyFrames.positions.at(boneName)[index].first, keyFrames.positions.at(boneName)[index + 1].first, m_animationTime);
		Vector3f position = getInterpolated(keyFrames.positions.at(boneName)[index].second, keyFrames.positions.at(boneName)[index + 1].second, progression);

		index = keyFrames.getScaleIndex(m_animationTime, boneName);
		progression = getProgression(keyFrames.scales.at(boneName)[index].first, keyFrames.scales.at(boneName)[index + 1].first, m_animationTime);
		Vector3f scale = getInterpolated(keyFrames.scales.at(boneName)[index].second, keyFrames.scales.at(boneName)[index + 1].second, progression);

		index = keyFrames.getRotationIndex(m_animationTime, boneName);
		progression = getProgression(keyFrames.rotations.at(boneName)[index].first, keyFrames.rotations.at(boneName)[index + 1].first, m_animationTime);
		Quaternion rot = interpolateQuat(keyFrames.rotations.at(boneName)[index].second, keyFrames.rotations.at(boneName)[index + 1].second, progression);
		
		Matrix4f mat = rot.toMatrix4f();
		Matrix4f trans;
		trans.translate(position[0], position[1], position[2]);
		Matrix4f sca;
		sca.scale(scale[0], scale[1], scale[2]);

		currentPose.insert(std::make_pair(boneName, sca * trans * mat));
	}

	return currentPose;
}