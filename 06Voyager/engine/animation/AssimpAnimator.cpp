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

void AssimpAnimator::update(float deltaTime) {
	
	m_animationTime += deltaTime * m_ticksPerSecond;

	if (m_animationTime > m_currentAnimation->getDuration()) {
		m_animationTime = fmod(m_animationTime, m_currentAnimation->getDuration());
	}

	m_model->m_meshes[0]->currentPose = calculateCurrentAnimationPose();	
}

void AssimpAnimator::blendTwoAnimations(std::string pBaseAnimation, std::string pLayeredAnimation, float blendFactor, float deltaTime) {
	// Speed multipliers to correctly transition from one animation to another
	float a = 1.0f;
	float b = m_animations[0]->getDuration() / m_animations[1]->getDuration();
	const float animSpeedMultiplierUp = (1.0f - blendFactor) * a + b * blendFactor; // Lerp

	a = m_animations[1]->getDuration() / m_animations[0]->getDuration();
	b = 1.0f;
	const float animSpeedMultiplierDown = (1.0f - blendFactor) * a + b * blendFactor; // Lerp

	// Current time of each animation, "scaled" by the above speed multiplier variables
	static float currentTimeBase = 0.0f;
	currentTimeBase += m_animations[0]->m_ticksPerSecond * deltaTime * animSpeedMultiplierUp;
	currentTimeBase = fmod(currentTimeBase, m_animations[0]->getDuration());

	static float currentTimeLayered = 0.0f;
	currentTimeLayered += m_animations[1]->m_ticksPerSecond * deltaTime * animSpeedMultiplierDown;
	currentTimeLayered = fmod(currentTimeLayered, m_animations[1]->getDuration());

	//std::cout << "Blend Factor: " << blendFactor << std::endl;
	
	m_model->m_meshes[0]->currentPose = calculateBlendedAnimationPose(currentTimeBase, currentTimeLayered, blendFactor);
}

std::unordered_map<std::string, Matrix4f> AssimpAnimator::calculateBlendedAnimationPose(const float currentTimeBase, const float currentTimeLayered, const float blendFactor) {
	AssimpKeyFrameData keyFramesBase = m_animations[0]->m_keyFrames;
	AssimpKeyFrameData keyFramesLayer = m_animations[1]->m_keyFrames;

	std::unordered_map<std::string, Matrix4f> currentPose;

	std::sort(m_animations[0]->m_boneList.begin(), m_animations[0]->m_boneList.end());
	std::sort(m_animations[1]->m_boneList.begin(), m_animations[1]->m_boneList.end());

	std::vector<std::string> intersection;

	std::set_intersection(m_animations[0]->m_boneList.begin(), m_animations[0]->m_boneList.end(),
		m_animations[1]->m_boneList.begin(), m_animations[1]->m_boneList.end(),
		std::back_inserter(intersection));


	for (auto boneName : intersection) {
		int index = keyFramesBase.getPositionIndex(currentTimeBase, boneName);
		float progression = getProgression(keyFramesBase.positions.at(boneName)[index].first, keyFramesBase.positions.at(boneName)[index + 1].first, currentTimeBase);
		Vector3f positionBase = getInterpolated(keyFramesBase.positions.at(boneName)[index].second, keyFramesBase.positions.at(boneName)[index + 1].second, progression);

		index = keyFramesBase.getScaleIndex(currentTimeBase, boneName);
		progression = getProgression(keyFramesBase.scales.at(boneName)[index].first, keyFramesBase.scales.at(boneName)[index + 1].first, currentTimeBase);
		Vector3f scaleBase = getInterpolated(keyFramesBase.scales.at(boneName)[index].second, keyFramesBase.scales.at(boneName)[index + 1].second, progression);

		index = keyFramesBase.getRotationIndex(currentTimeBase, boneName);
		progression = getProgression(keyFramesBase.rotations.at(boneName)[index].first, keyFramesBase.rotations.at(boneName)[index + 1].first, currentTimeBase);
		Quaternion rotBase = interpolateQuat(keyFramesBase.rotations.at(boneName)[index].second, keyFramesBase.rotations.at(boneName)[index + 1].second, progression);


		index = keyFramesLayer.getPositionIndex(currentTimeLayered, boneName);
		progression = getProgression(keyFramesLayer.positions.at(boneName)[index].first, keyFramesLayer.positions.at(boneName)[index + 1].first, currentTimeLayered);
		Vector3f positionLayer = getInterpolated(keyFramesLayer.positions.at(boneName)[index].second, keyFramesLayer.positions.at(boneName)[index + 1].second, progression);

		index = keyFramesLayer.getScaleIndex(currentTimeLayered, boneName);
		progression = getProgression(keyFramesLayer.scales.at(boneName)[index].first, keyFramesLayer.scales.at(boneName)[index + 1].first, currentTimeLayered);
		Vector3f scaleLayer = getInterpolated(keyFramesLayer.scales.at(boneName)[index].second, keyFramesLayer.scales.at(boneName)[index + 1].second, progression);

		index = keyFramesLayer.getRotationIndex(currentTimeLayered, boneName);
		progression = getProgression(keyFramesLayer.rotations.at(boneName)[index].first, keyFramesLayer.rotations.at(boneName)[index + 1].first, currentTimeLayered);
		Quaternion rotLayer = interpolateQuat(keyFramesLayer.rotations.at(boneName)[index].second, keyFramesLayer.rotations.at(boneName)[index + 1].second, progression);


		Vector3f position = getInterpolated(positionBase, positionLayer, blendFactor);
		Vector3f scale = getInterpolated(scaleBase, scaleLayer, blendFactor);
		Quaternion rot = interpolateQuat(rotBase, rotLayer, blendFactor);

		Matrix4f mat = rot.toMatrix4f();
		Matrix4f trans;
		trans.translate(position[0], position[1], position[2]);
		Matrix4f sca;
		sca.scale(scale[0], scale[1], scale[2]);

		currentPose.insert(std::make_pair(boneName, sca * trans * mat));
	}

	if (blendFactor != 1.0f) {
		std::vector<std::string> diffRight;

		std::set_difference(m_animations[0]->m_boneList.begin(), m_animations[0]->m_boneList.end(),
			m_animations[1]->m_boneList.begin(), m_animations[1]->m_boneList.end(), std::inserter(diffRight, diffRight.begin()));

		for (auto boneName : diffRight) {
			int index = keyFramesBase.getPositionIndex(currentTimeBase, boneName);
			float progression = getProgression(keyFramesBase.positions.at(boneName)[index].first, keyFramesBase.positions.at(boneName)[index + 1].first, currentTimeBase);
			Vector3f positionBase = getInterpolated(keyFramesBase.positions.at(boneName)[index].second, keyFramesBase.positions.at(boneName)[index + 1].second, progression);

			index = keyFramesBase.getScaleIndex(currentTimeBase, boneName);
			progression = getProgression(keyFramesBase.scales.at(boneName)[index].first, keyFramesBase.scales.at(boneName)[index + 1].first, currentTimeBase);
			Vector3f scaleBase = getInterpolated(keyFramesBase.scales.at(boneName)[index].second, keyFramesBase.scales.at(boneName)[index + 1].second, progression);

			index = keyFramesBase.getRotationIndex(currentTimeBase, boneName);
			progression = getProgression(keyFramesBase.rotations.at(boneName)[index].first, keyFramesBase.rotations.at(boneName)[index + 1].first, currentTimeBase);
			Quaternion rotBase = interpolateQuat(keyFramesBase.rotations.at(boneName)[index].second, keyFramesBase.rotations.at(boneName)[index + 1].second, progression);

			Matrix4f mat = rotBase.toMatrix4f();
			Matrix4f trans;
			trans.translate(positionBase[0], positionBase[1], positionBase[2]);
			Matrix4f sca;
			sca.scale(scaleBase[0], scaleBase[1], scaleBase[2]);

			currentPose.insert(std::make_pair(boneName, sca * trans * mat));
		}
	}

	if (blendFactor != 0.0f) {
		std::vector<std::string> diffLeft;

		std::set_difference(m_animations[1]->m_boneList.begin(), m_animations[1]->m_boneList.end(),
			m_animations[0]->m_boneList.begin(), m_animations[0]->m_boneList.end(), std::inserter(diffLeft, diffLeft.begin()));

		for (auto boneName : diffLeft) {
			int index = keyFramesLayer.getPositionIndex(currentTimeLayered, boneName);
			float progression = getProgression(keyFramesLayer.positions.at(boneName)[index].first, keyFramesLayer.positions.at(boneName)[index + 1].first, currentTimeLayered);
			Vector3f positionLayer = getInterpolated(keyFramesLayer.positions.at(boneName)[index].second, keyFramesLayer.positions.at(boneName)[index + 1].second, progression);

			index = keyFramesLayer.getScaleIndex(currentTimeLayered, boneName);
			progression = getProgression(keyFramesLayer.scales.at(boneName)[index].first, keyFramesLayer.scales.at(boneName)[index + 1].first, currentTimeLayered);
			Vector3f scaleLayer = getInterpolated(keyFramesLayer.scales.at(boneName)[index].second, keyFramesLayer.scales.at(boneName)[index + 1].second, progression);

			index = keyFramesLayer.getRotationIndex(currentTimeLayered, boneName);
			progression = getProgression(keyFramesLayer.rotations.at(boneName)[index].first, keyFramesLayer.rotations.at(boneName)[index + 1].first, currentTimeLayered);
			Quaternion rotLayer = interpolateQuat(keyFramesLayer.rotations.at(boneName)[index].second, keyFramesLayer.rotations.at(boneName)[index + 1].second, progression);

			Matrix4f mat = rotLayer.toMatrix4f();
			Matrix4f trans;
			trans.translate(positionLayer[0], positionLayer[1], positionLayer[2]);
			Matrix4f sca;
			sca.scale(scaleLayer[0], scaleLayer[1], scaleLayer[2]);

			currentPose.insert(std::make_pair(boneName, sca * trans * mat));
		}
	}

	return currentPose;
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