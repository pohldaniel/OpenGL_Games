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

void AssimpAnimator::startAnimation(const std::string & current) {
	m_animationTime = 0;
	m_currentAnimation = m_animations.at(current);
	m_ticksPerSecond = m_currentAnimation->m_ticksPerSecond;
}

void AssimpAnimator::addAnimation(AssimpAnimation* animation) {
	m_animations.insert(std::make_pair(animation->getName(), std::shared_ptr<AssimpAnimation>(animation)));
}

void AssimpAnimator::update(float deltaTime) {
	
	m_animationTime += deltaTime * m_ticksPerSecond * 10.0f;

	if (m_animationTime > m_currentAnimation->getDuration()) {
		m_animationTime = fmod(m_animationTime, m_currentAnimation->getDuration());
	}

	//std::cout << "Time1: " << m_animationTime <<  std::endl;

	m_model->m_meshes[0]->currentPose = calculateCurrentAnimationPose();	
}

void AssimpAnimator::addTwoAnimations(float deltaTime, std::string current, std::string layer) {
	if (layer.compare(m_layer) != 0) {
		replaceBasePose(*m_animations.at(layer));
		m_current = current;
		m_layer = layer;
	}
	m_currentAnimation = m_animations.at(current);
	m_layeredAnimation = m_animations.at(layer);

	m_animationTime += deltaTime;
	if (m_animationTime > m_currentAnimation->getDuration()) {
		m_animationTime = fmod(m_animationTime, m_currentAnimation->getDuration());
	}

	

	m_additiveTime += deltaTime * m_additiveDirection;


	if (m_additiveTime < 0.0f) {
		m_additiveTime = 0.0f;
		m_additiveDirection *= -1.0f;
	}

	if (m_additiveTime > 1.0f) {
		m_additiveTime = 1.0f;
		m_additiveDirection *= -1.0f;
	}

	float addTime = m_layeredAnimation->m_startTime + (m_layeredAnimation->m_duration * m_additiveTime);
	m_model->m_meshes[0]->currentPose = calculateCurrentAnimationPose(m_animationTime, addTime, *m_currentAnimation, *m_layeredAnimation);
}

void AssimpAnimator::replaceBasePose(AssimpAnimation& animation) {
	m_basePose.positions.clear();
	m_basePose.scales.clear();
	m_basePose.rotations.clear();

	AssimpKeyFrameData keyFrames = animation.m_keyFrames;

	for (auto boneName : animation.m_boneList) {
		float progression = getProgression(keyFrames.positions.at(boneName)[0].first, keyFrames.positions.at(boneName)[1].first, 0.0f);
		Vector3f position = getInterpolated(keyFrames.positions.at(boneName)[0].second, keyFrames.positions.at(boneName)[1].second, progression);

		progression = getProgression(keyFrames.scales.at(boneName)[0].first, keyFrames.scales.at(boneName)[1].first, 0.0f);
		Vector3f scale = getInterpolated(keyFrames.scales.at(boneName)[0].second, keyFrames.scales.at(boneName)[1].second, progression);

		progression = getProgression(keyFrames.rotations.at(boneName)[0].first, keyFrames.rotations.at(boneName)[1].first, 0.0f);
		Quaternion rot = interpolateQuat(keyFrames.rotations.at(boneName)[0].second, keyFrames.rotations.at(boneName)[1].second, progression);

		m_basePose.positions.insert(std::make_pair(boneName, position));
		m_basePose.scales.insert(std::make_pair(boneName, scale));
		m_basePose.rotations.insert(std::make_pair(boneName, rot));
	}
}

std::unordered_map<std::string, Matrix4f> AssimpAnimator::calculateCurrentAnimationPose(float time, float addTime, AssimpAnimation& animation, AssimpAnimation& animationAdd) {
	AssimpKeyFrameData keyFrames = animation.m_keyFrames;
	AssimpKeyFrameData keyFramesAdd = animationAdd.m_keyFrames;
	
	std::unordered_map<std::string, Matrix4f> currentPose;

	for (auto boneName : animationAdd.m_boneList) {
		
		int index = keyFrames.getPositionIndex(time, boneName);
		float progression = getProgression(keyFrames.positions.at(boneName)[index].first, keyFrames.positions.at(boneName)[index + 1].first, time);
		Vector3f position = getInterpolated(keyFrames.positions.at(boneName)[index].second, keyFrames.positions.at(boneName)[index + 1].second, progression);

		index = keyFrames.getScaleIndex(time, boneName);
		progression = getProgression(keyFrames.scales.at(boneName)[index].first, keyFrames.scales.at(boneName)[index + 1].first, time);
		Vector3f scale = getInterpolated(keyFrames.scales.at(boneName)[index].second, keyFrames.scales.at(boneName)[index + 1].second, progression);

		index = keyFrames.getRotationIndex(time, boneName);
		progression = getProgression(keyFrames.rotations.at(boneName)[index].first, keyFrames.rotations.at(boneName)[index + 1].first, time);
		Quaternion rot = interpolateQuat(keyFrames.rotations.at(boneName)[index].second, keyFrames.rotations.at(boneName)[index + 1].second, progression);

		int indexAdd = keyFramesAdd.getPositionIndex(addTime, boneName);
		float progressionAdd = getProgression(keyFramesAdd.positions.at(boneName)[indexAdd].first, keyFramesAdd.positions.at(boneName)[indexAdd + 1].first, addTime);
		Vector3f positionAdd = getInterpolated(keyFramesAdd.positions.at(boneName)[indexAdd].second, keyFramesAdd.positions.at(boneName)[indexAdd + 1].second, progressionAdd);
		 
		indexAdd = keyFramesAdd.getScaleIndex(addTime, boneName);
		progressionAdd = getProgression(keyFramesAdd.scales.at(boneName)[indexAdd].first, keyFramesAdd.scales.at(boneName)[indexAdd + 1].first, addTime);
		Vector3f scaleAdd = getInterpolated(keyFramesAdd.scales.at(boneName)[indexAdd].second, keyFramesAdd.scales.at(boneName)[indexAdd + 1].second, progressionAdd);

		indexAdd = keyFramesAdd.getRotationIndex(addTime, boneName);
		progressionAdd = getProgression(keyFramesAdd.rotations.at(boneName)[indexAdd].first, keyFramesAdd.rotations.at(boneName)[indexAdd + 1].first, addTime);
		Quaternion rotAdd = interpolateQuat(keyFramesAdd.rotations.at(boneName)[indexAdd].second, keyFramesAdd.rotations.at(boneName)[indexAdd + 1].second, progressionAdd);

		Vector3f positionBase = m_basePose.positions.at(boneName);
		Vector3f scaleBase = m_basePose.scales.at(boneName);
		Quaternion rotBase = m_basePose.rotations.at(boneName);

		rotBase.inverse();

		rot = rotAdd * rotBase * rot;

		Matrix4f mat = rot.toMatrix4f();
		Matrix4f trans;
		trans.translate(position[0] + positionAdd[0] - positionBase[0], position[1] + positionAdd[1] - positionBase[1], position[2] + positionAdd[2] - positionBase[2]);
		Matrix4f sca;
		sca.scale(scale[0] + scaleAdd[0] - scaleBase[0], scale[1] + scaleAdd[1] - scaleBase[1], scale[2] + scaleAdd[2] - scaleBase[2]);

		currentPose.insert(std::make_pair(boneName, sca * trans * mat));
	}

	return currentPose;
}

void AssimpAnimator::blendTwoAnimations(std::string current, std::string layer, float blendFactor, float deltaTime) {
	m_currentAnimation = m_animations.at(current);
	m_layeredAnimation = m_animations.at(layer);
	
	// Speed multipliers to correctly transition from one animation to another
	float a = 1.0f;
	float b = m_currentAnimation->getDuration() / m_layeredAnimation->getDuration();
	const float animSpeedMultiplierUp = (1.0f - blendFactor) * a + b * blendFactor; // Lerp

	a = m_layeredAnimation->getDuration() / m_currentAnimation->getDuration();
	b = 1.0f;
	const float animSpeedMultiplierDown = (1.0f - blendFactor) * a + b * blendFactor; // Lerp

	// Current time of each animation, "scaled" by the above speed multiplier variables
	static float currentTimeBase = 0.0f;
	currentTimeBase += m_currentAnimation->m_ticksPerSecond * deltaTime * animSpeedMultiplierUp;
	currentTimeBase = fmod(currentTimeBase, m_currentAnimation->getDuration());

	static float currentTimeLayered = 0.0f;
	currentTimeLayered += m_layeredAnimation->m_ticksPerSecond * deltaTime * animSpeedMultiplierDown;
	currentTimeLayered = fmod(currentTimeLayered, m_layeredAnimation->getDuration());
	m_model->m_meshes[0]->currentPose = calculateBlendedAnimationPose(currentTimeBase, currentTimeLayered, *m_currentAnimation, *m_layeredAnimation, blendFactor);
}

std::unordered_map<std::string, Matrix4f> AssimpAnimator::calculateBlendedAnimationPose(const float currentTimeBase, const float currentTimeLayered, AssimpAnimation& animationBase, AssimpAnimation& animationAddLayer, const float blendFactor) {
	AssimpKeyFrameData keyFramesBase = animationBase.m_keyFrames;
	AssimpKeyFrameData keyFramesLayer = animationAddLayer.m_keyFrames;

	std::unordered_map<std::string, Matrix4f> currentPose;

	std::sort(animationBase.m_boneList.begin(), animationBase.m_boneList.end());
	std::sort(animationAddLayer.m_boneList.begin(), animationAddLayer.m_boneList.end());

	std::vector<std::string> intersection;

	std::set_intersection(animationBase.m_boneList.begin(), animationBase.m_boneList.end(),
		animationAddLayer.m_boneList.begin(), animationAddLayer.m_boneList.end(),
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

		std::set_difference(animationBase.m_boneList.begin(), animationBase.m_boneList.end(),
			animationAddLayer.m_boneList.begin(), animationAddLayer.m_boneList.end(), std::inserter(diffRight, diffRight.begin()));

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

		std::set_difference(animationAddLayer.m_boneList.begin(), animationAddLayer.m_boneList.end(),
			animationBase.m_boneList.begin(), animationBase.m_boneList.end(), std::inserter(diffLeft, diffLeft.begin()));

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