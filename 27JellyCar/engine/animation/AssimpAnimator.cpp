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

void AssimpAnimator::startAnimation(const std::string& current) {
	m_animationTime = 0;
	m_currentAnimation = m_animations.at(current);
	m_ticksPerSecond = m_currentAnimation->m_ticksPerSecond;
}

void AssimpAnimator::addAnimation(AssimpAnimation* animation) {
	m_animations.insert(std::make_pair(animation->getName(), std::shared_ptr<AssimpAnimation>(animation)));
}

void AssimpAnimator::update(float deltaTime) {
	
	m_animationTime += deltaTime * m_ticksPerSecond;

	if (m_animationTime > m_currentAnimation->getDuration()) {
		m_animationTime = fmod(m_animationTime, m_currentAnimation->getDuration());
	}

	m_model->m_meshes[0]->currentPose = calculateCurrentAnimationPose(m_animationTime);
}

void AssimpAnimator::addTwoAnimations(float deltaTime, std::string current, std::string layer, float speed) {
	if (layer.compare(m_layer) != 0) {
		replaceBasePose(*m_animations.at(layer));
		m_current = current;
		m_layer = layer;
	}

	m_currentAnimation = m_animations.at(current);
	m_layeredAnimation = m_animations.at(layer);

	m_animationTime += deltaTime * m_currentAnimation->m_ticksPerSecond * speed;
	if (m_animationTime > m_currentAnimation->getDuration()) {
		m_animationTime = fmod(m_animationTime, m_currentAnimation->getDuration());
	}

	m_layeredTime += deltaTime * m_additiveDirection * m_layeredAnimation->m_ticksPerSecond * speed;
	if (m_layeredTime < 0.0f) {
		m_layeredTime = 0.0f;
		m_additiveDirection *= -1.0f;
	}

	if (m_layeredTime > 1.0f) {
		m_layeredTime = 1.0f;
		m_additiveDirection *= -1.0f;
	}

	float addTime = m_layeredAnimation->m_startTime + (m_layeredAnimation->m_duration * m_layeredTime);
	m_model->m_meshes[0]->currentPose = calculateAdditiveAnimationPose(m_animationTime, addTime, *m_currentAnimation, *m_layeredAnimation);
}

void AssimpAnimator::addTwoAnimationsDisjoint(float deltaTime, std::string current, std::string layer, float speed) {
	if (layer.compare(m_layer) != 0) {
		replaceBasePose(*m_animations.at(layer));
		m_current = current;
		m_layer = layer;
	}

	m_currentAnimation = m_animations.at(current);
	m_layeredAnimation = m_animations.at(layer);

	m_animationTime += deltaTime * m_currentAnimation->m_ticksPerSecond * speed;
	if (m_animationTime > m_currentAnimation->getDuration()) {
		m_animationTime = fmod(m_animationTime, m_currentAnimation->getDuration());
	}

	m_layeredTime += deltaTime * m_additiveDirection * m_layeredAnimation->m_ticksPerSecond * speed;
	if (m_layeredTime < 0.0f) {
		m_layeredTime = 0.0f;
		m_additiveDirection *= -1.0f;
	}

	if (m_layeredTime > 1.0f) {
		m_layeredTime = 1.0f;
		m_additiveDirection *= -1.0f;
	}

	float addTime = m_layeredAnimation->m_startTime + (m_layeredAnimation->m_duration * m_layeredTime);
	m_model->m_meshes[0]->currentPose = calculateAdditiveAnimationPoseDisjoint(m_animationTime, addTime, *m_currentAnimation, *m_layeredAnimation);
}

void AssimpAnimator::blendTwoAnimations(float deltaTime, std::string current, std::string layer, float blendTime,  float speed) {
	m_currentAnimation = m_animations.at(current);
	m_layeredAnimation = m_animations.at(layer);

	float a = 1.0f;
	float b = m_currentAnimation->getDuration() / m_layeredAnimation->getDuration();
	const float animSpeedMultiplierUp = (1.0f - blendTime) * a + b * blendTime;

	a = m_layeredAnimation->getDuration() / m_currentAnimation->getDuration();
	b = 1.0f;
	const float animSpeedMultiplierDown = (1.0f - blendTime) * a + b * blendTime;

	m_animationTime += m_currentAnimation->m_ticksPerSecond * deltaTime * animSpeedMultiplierUp;
	if (m_animationTime > m_currentAnimation->getDuration()) {
		m_animationTime = fmod(m_animationTime, m_currentAnimation->getDuration());
	}

	m_layeredTime += m_layeredAnimation->m_ticksPerSecond * deltaTime * animSpeedMultiplierDown;
	if (m_layeredTime > m_layeredAnimation->getDuration()) {
		m_layeredTime = fmod(m_layeredTime, m_layeredAnimation->getDuration());
	}

	m_model->m_meshes[0]->currentPose = calculateBlendedPose(m_animationTime, m_layeredTime, *m_currentAnimation, *m_layeredAnimation, blendTime);

	mBlendTime += deltaTime;
	if (mBlendTime >= 2.0f) {
		mBlendTime = 0.0f;
		mInvertBlend = !mInvertBlend;
	}
}

void AssimpAnimator::blendTwoAnimationsDisjoint(float deltaTime, std::string current, std::string layer, float blendTime, float speed) {
	m_currentAnimation = m_animations.at(current);
	m_layeredAnimation = m_animations.at(layer);

	float a = 1.0f;
	float b = m_currentAnimation->getDuration() / m_layeredAnimation->getDuration();
	const float animSpeedMultiplierUp = (1.0f - blendTime) * a + b * blendTime;

	a = m_layeredAnimation->getDuration() / m_currentAnimation->getDuration();
	b = 1.0f;
	const float animSpeedMultiplierDown = (1.0f - blendTime) * a + b * blendTime;


	m_animationTime += m_currentAnimation->m_ticksPerSecond * deltaTime * animSpeedMultiplierUp;
	if (m_animationTime > m_currentAnimation->getDuration()) {
		m_animationTime = fmod(m_animationTime, m_currentAnimation->getDuration());
	}

	m_layeredTime += m_layeredAnimation->m_ticksPerSecond * deltaTime * animSpeedMultiplierDown;
	if (m_layeredTime > m_layeredAnimation->getDuration()) {
		m_layeredTime = fmod(m_layeredTime, m_layeredAnimation->getDuration());
	}

	m_model->m_meshes[0]->currentPose = calculateBlendedPoseDisjoint(m_animationTime, m_layeredTime, *m_currentAnimation, *m_layeredAnimation, blendTime);

	mBlendTime += deltaTime;
	if (mBlendTime >= 2.0f) {
		mBlendTime = 0.0f;
		mInvertBlend = !mInvertBlend;
	}
}

std::unordered_map<std::string, Matrix4f> AssimpAnimator::calculateBlendedPose(float currentTime, float layeredTime, AssimpAnimation& curretAnimation, AssimpAnimation& animationLayer, float blendTime) {
	AssimpKeyFrameData keyFramesA = curretAnimation.m_keyFrames;
	AssimpKeyFrameData keyFramesB = animationLayer.m_keyFrames;

	std::unordered_map<std::string, Matrix4f> currentPose;

	for (auto boneName : curretAnimation.m_boneList) {

		int indexA = keyFramesA.getPositionIndex(currentTime, boneName);
		float progressionA = getProgression(keyFramesA.positions.at(boneName)[indexA].first, keyFramesA.positions.at(boneName)[indexA + 1].first, currentTime);
		Vector3f positionA = getInterpolated(keyFramesA.positions.at(boneName)[indexA].second, keyFramesA.positions.at(boneName)[indexA + 1].second, progressionA);

		indexA = keyFramesA.getScaleIndex(currentTime, boneName);
		progressionA = getProgression(keyFramesA.scales.at(boneName)[indexA].first, keyFramesA.scales.at(boneName)[indexA + 1].first, currentTime);
		Vector3f scaleA = getInterpolated(keyFramesA.scales.at(boneName)[indexA].second, keyFramesA.scales.at(boneName)[indexA + 1].second, progressionA);

		indexA = keyFramesA.getRotationIndex(currentTime, boneName);
		progressionA = getProgression(keyFramesA.rotations.at(boneName)[indexA].first, keyFramesA.rotations.at(boneName)[indexA + 1].first, currentTime);
		Quaternion rotA = interpolateQuat(keyFramesA.rotations.at(boneName)[indexA].second, keyFramesA.rotations.at(boneName)[indexA + 1].second, progressionA);

		int indexB = keyFramesB.getPositionIndex(layeredTime, boneName);
		float progressionB = getProgression(keyFramesB.positions.at(boneName)[indexB].first, keyFramesB.positions.at(boneName)[indexB + 1].first, layeredTime);
		Vector3f positionB = getInterpolated(keyFramesB.positions.at(boneName)[indexB].second, keyFramesB.positions.at(boneName)[indexB + 1].second, progressionB);

		indexB = keyFramesB.getScaleIndex(layeredTime, boneName);
		progressionB = getProgression(keyFramesB.scales.at(boneName)[indexB].first, keyFramesB.scales.at(boneName)[indexB + 1].first, layeredTime);
		Vector3f scaleB = getInterpolated(keyFramesB.scales.at(boneName)[indexB].second, keyFramesB.scales.at(boneName)[indexB + 1].second, progressionB);

		indexB = keyFramesB.getRotationIndex(layeredTime, boneName);
		progressionB = getProgression(keyFramesB.rotations.at(boneName)[indexB].first, keyFramesB.rotations.at(boneName)[indexB + 1].first, layeredTime);
		Quaternion rotB = interpolateQuat(keyFramesB.rotations.at(boneName)[indexB].second, keyFramesB.rotations.at(boneName)[indexB + 1].second, progressionB);

		Vector3f position = getInterpolated(positionA, positionB, blendTime);
		Vector3f scale = getInterpolated(scaleA, scaleB, blendTime);
		Quaternion rot = interpolateQuat(rotA, rotB, blendTime);

		Matrix4f mat = rot.toMatrix4f();
		Matrix4f trans;
		trans.translate(position[0], position[1], position[2]);
		Matrix4f sca;
		sca.scale(scale[0], scale[1], scale[2]);

		currentPose.insert(std::make_pair(boneName, sca * trans * mat));
	}

	return currentPose;
}

std::unordered_map<std::string, Matrix4f> AssimpAnimator::calculateBlendedPoseDisjoint(float currentTime, float layeredTime, AssimpAnimation& curretAnimation, AssimpAnimation& animationLayer, float blendTime) {
	AssimpKeyFrameData keyFramesA = curretAnimation.m_keyFrames;
	AssimpKeyFrameData keyFramesB = animationLayer.m_keyFrames;

	std::unordered_map<std::string, Matrix4f> currentPose;

	AssimpKeyFrameBaseData poseB;

	std::set<std::string> bonesB;

	for (auto boneName : animationLayer.m_boneList) {
		float progression;
		Vector3f position;
		Vector3f scale;
		Quaternion rot;

		int index = keyFramesB.getPositionIndex(layeredTime, boneName);
		//if(index == -1) {
		//	position = Vector3f(0.0f, 0.0f, 0.0f);
		//}else {
		progression = getProgression(keyFramesB.positions.at(boneName)[index].first, keyFramesB.positions.at(boneName)[index + 1].first, layeredTime);
		position = getInterpolated(keyFramesB.positions.at(boneName)[index].second, keyFramesB.positions.at(boneName)[index + 1].second, progression);
		//}
		index = keyFramesB.getScaleIndex(layeredTime, boneName);

		//if(index == -1) {
		//	scale = Vector3f(1.0f, 1.0f, 1.0f);
		//}else {
		progression = getProgression(keyFramesB.scales.at(boneName)[index].first, keyFramesB.scales.at(boneName)[index + 1].first, layeredTime);
		scale = getInterpolated(keyFramesB.scales.at(boneName)[index].second, keyFramesB.scales.at(boneName)[index + 1].second, progression);
		//}
		index = keyFramesB.getRotationIndex(layeredTime, boneName);

		//if (index == -1) {
		//	rot.identity();
		//}else {
		progression = getProgression(keyFramesB.rotations.at(boneName)[index].first, keyFramesB.rotations.at(boneName)[index + 1].first, layeredTime);
		rot = interpolateQuat(keyFramesB.rotations.at(boneName)[index].second, keyFramesB.rotations.at(boneName)[index + 1].second, progression);
		//}

		poseB.positions.insert(std::make_pair(boneName, position));
		poseB.scales.insert(std::make_pair(boneName, scale));
		poseB.rotations.insert(std::make_pair(boneName, rot));

		bonesB.insert(boneName);

	}

	for (auto boneName : curretAnimation.m_boneList) {
		float progressionA;
		Vector3f positionA;
		Vector3f scaleA;
		Quaternion rotA;

		int indexA = keyFramesA.getPositionIndex(currentTime, boneName);

		//if (indexA == -1) {
		//	positionA = Vector3f(0.0f, 0.0f, 0.0f);
		//}else {
		progressionA = getProgression(keyFramesA.positions.at(boneName)[indexA].first, keyFramesA.positions.at(boneName)[indexA + 1].first, currentTime);
		positionA = getInterpolated(keyFramesA.positions.at(boneName)[indexA].second, keyFramesA.positions.at(boneName)[indexA + 1].second, progressionA);
		//}
		indexA = keyFramesA.getScaleIndex(currentTime, boneName);

		//if (indexA == -1) {
		//	scaleA = Vector3f(1.0f, 1.0f, 1.0f);
		//}else {
		progressionA = getProgression(keyFramesA.scales.at(boneName)[indexA].first, keyFramesA.scales.at(boneName)[indexA + 1].first, currentTime);
		scaleA = getInterpolated(keyFramesA.scales.at(boneName)[indexA].second, keyFramesA.scales.at(boneName)[indexA + 1].second, progressionA);
		//}
		indexA = keyFramesA.getRotationIndex(currentTime, boneName);

		//if (indexA == -1) {
		//rotA.identity();
		//}else {
		progressionA = getProgression(keyFramesA.rotations.at(boneName)[indexA].first, keyFramesA.rotations.at(boneName)[indexA + 1].first, currentTime);
		rotA = interpolateQuat(keyFramesA.rotations.at(boneName)[indexA].second, keyFramesA.rotations.at(boneName)[indexA + 1].second, progressionA);
		//}

		if (bonesB.find(boneName) != bonesB.end()) {
			Vector3f positionB = poseB.positions.at(boneName);
			Vector3f scaleB = poseB.scales.at(boneName);
			Quaternion rotB = poseB.rotations.at(boneName);

			Vector3f position = getInterpolated(positionA, positionB, blendTime);
			Vector3f scale = getInterpolated(scaleA, scaleB, blendTime);
			Quaternion rot = interpolateQuat(rotA, rotB, blendTime);

			Matrix4f mat = rot.toMatrix4f();
			Matrix4f trans;
			trans.translate(position[0], position[1], position[2]);
			Matrix4f sca;
			sca.scale(scale[0], scale[1], scale[2]);

			currentPose.insert(std::make_pair(boneName, sca * trans * mat));
			bonesB.erase(boneName);

		}else {
			Matrix4f mat = rotA.toMatrix4f();
			Matrix4f trans;
			trans.translate(positionA[0], positionA[1], positionA[2]);
			Matrix4f sca;
			sca.scale(scaleA[0], scaleA[1], scaleA[2]);

			currentPose.insert(std::make_pair(boneName, sca * trans * mat));
		}
	}

	std::set<std::string>::iterator it;
	for (it = bonesB.begin(); it != bonesB.end(); ++it) {
		std::string boneName = *it;

		Vector3f position = poseB.positions.at(boneName);
		Vector3f scale = poseB.scales.at(boneName);
		Quaternion rot = poseB.rotations.at(boneName);

		Matrix4f mat = rot.toMatrix4f();
		Matrix4f trans;
		trans.translate(position[0], position[1], position[2]);
		Matrix4f sca;
		sca.scale(scale[0], scale[1], scale[2]);

		currentPose.insert(std::make_pair(boneName, sca * trans * mat));
	}

	return currentPose;
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

std::unordered_map<std::string, Matrix4f> AssimpAnimator::calculateAdditiveAnimationPose(float time, float addTime, AssimpAnimation& animation, AssimpAnimation& animationAdd) {
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

std::unordered_map<std::string, Matrix4f> AssimpAnimator::calculateAdditiveAnimationPoseDisjoint(float currentTime, float layeredTime, AssimpAnimation& animation, AssimpAnimation& animationLayer) {
	AssimpKeyFrameData keyFrames = animation.m_keyFrames;
	AssimpKeyFrameData keyFramesAdd = animationLayer.m_keyFrames;

	std::unordered_map<std::string, Matrix4f> currentPose;

	AssimpKeyFrameBaseData poseCurrent;
	std::set<std::string> bonesCurrent;
	
	
	for (auto boneName : animation.m_boneList) {

		int index = keyFrames.getPositionIndex(currentTime, boneName);
		float progression = getProgression(keyFrames.positions.at(boneName)[index].first, keyFrames.positions.at(boneName)[index + 1].first, currentTime);
		Vector3f position = getInterpolated(keyFrames.positions.at(boneName)[index].second, keyFrames.positions.at(boneName)[index + 1].second, progression);

		index = keyFrames.getScaleIndex(currentTime, boneName);
		progression = getProgression(keyFrames.scales.at(boneName)[index].first, keyFrames.scales.at(boneName)[index + 1].first, currentTime);
		Vector3f scale = getInterpolated(keyFrames.scales.at(boneName)[index].second, keyFrames.scales.at(boneName)[index + 1].second, progression);

		index = keyFrames.getRotationIndex(currentTime, boneName);
		progression = getProgression(keyFrames.rotations.at(boneName)[index].first, keyFrames.rotations.at(boneName)[index + 1].first, currentTime);
		Quaternion rot = interpolateQuat(keyFrames.rotations.at(boneName)[index].second, keyFrames.rotations.at(boneName)[index + 1].second, progression);

		poseCurrent.positions.insert(std::make_pair(boneName, position));
		poseCurrent.scales.insert(std::make_pair(boneName, scale));
		poseCurrent.rotations.insert(std::make_pair(boneName, rot));

		bonesCurrent.insert(boneName);
	}
	
	for (auto boneName : animationLayer.m_boneList) {
		int indexAdd = keyFramesAdd.getPositionIndex(layeredTime, boneName);
		float progressionAdd = getProgression(keyFramesAdd.positions.at(boneName)[indexAdd].first, keyFramesAdd.positions.at(boneName)[indexAdd + 1].first, layeredTime);
		Vector3f positionAdd = getInterpolated(keyFramesAdd.positions.at(boneName)[indexAdd].second, keyFramesAdd.positions.at(boneName)[indexAdd + 1].second, progressionAdd);

		indexAdd = keyFramesAdd.getScaleIndex(layeredTime, boneName);
		progressionAdd = getProgression(keyFramesAdd.scales.at(boneName)[indexAdd].first, keyFramesAdd.scales.at(boneName)[indexAdd + 1].first, layeredTime);
		Vector3f scaleAdd = getInterpolated(keyFramesAdd.scales.at(boneName)[indexAdd].second, keyFramesAdd.scales.at(boneName)[indexAdd + 1].second, progressionAdd);

		indexAdd = keyFramesAdd.getRotationIndex(layeredTime, boneName);
		progressionAdd = getProgression(keyFramesAdd.rotations.at(boneName)[indexAdd].first, keyFramesAdd.rotations.at(boneName)[indexAdd + 1].first, layeredTime);
		Quaternion rotAdd = interpolateQuat(keyFramesAdd.rotations.at(boneName)[indexAdd].second, keyFramesAdd.rotations.at(boneName)[indexAdd + 1].second, progressionAdd);
		
		if (bonesCurrent.find(boneName) != bonesCurrent.end()) {
			Vector3f positionCurrent = poseCurrent.positions.at(boneName);
			Vector3f scaleCurrent = poseCurrent.scales.at(boneName);
			Quaternion rotCurrent = poseCurrent.rotations.at(boneName);

			Vector3f positionBase = m_basePose.positions.at(boneName);
			Vector3f scaleBase = m_basePose.scales.at(boneName);
			Quaternion rotBase = m_basePose.rotations.at(boneName);
			rotBase.inverse();

			rotAdd = rotAdd * rotBase * rotCurrent;

			Matrix4f mat = rotAdd.toMatrix4f();
			Matrix4f trans;
			trans.translate(positionCurrent[0] + positionAdd[0] - positionBase[0], positionCurrent[1] + positionAdd[1] - positionBase[1], positionCurrent[2] + positionAdd[2] - positionBase[2]);
			Matrix4f sca;
			sca.scale(scaleCurrent[0] + scaleAdd[0] - scaleBase[0], scaleCurrent[1] + scaleAdd[1] - scaleBase[1], scaleCurrent[2] + scaleAdd[2] - scaleBase[2]);

			currentPose.insert(std::make_pair(boneName, sca * trans * mat));
			
			bonesCurrent.erase(boneName);

		}else {
			Matrix4f mat = rotAdd.toMatrix4f();
			Matrix4f trans;
			trans.translate(positionAdd[0], positionAdd[1], positionAdd[2]);
			Matrix4f sca;
			sca.scale(scaleAdd[0], scaleAdd[1], scaleAdd[2]);

			currentPose.insert(std::make_pair(boneName, sca * trans * mat));
		}
	}

	std::set<std::string>::iterator it;
	for (it = bonesCurrent.begin(); it != bonesCurrent.end(); ++it) {
		std::string boneName = *it;

		Vector3f position = poseCurrent.positions.at(boneName);
		Vector3f scale = poseCurrent.scales.at(boneName);
		Quaternion rot = poseCurrent.rotations.at(boneName);

		Matrix4f mat = rot.toMatrix4f();
		Matrix4f trans;
		trans.translate(position[0], position[1], position[2]);
		Matrix4f sca;
		sca.scale(scale[0], scale[1], scale[2]);

		currentPose.insert(std::make_pair(boneName, sca * trans * mat));
	}
	return currentPose;
}

std::unordered_map<std::string, Matrix4f> AssimpAnimator::calculateCurrentAnimationPose(float currentTime) {

	AssimpKeyFrameData keyFrames = m_currentAnimation->m_keyFrames;
	std::unordered_map<std::string, Matrix4f> currentPose;

	for (auto boneName : m_currentAnimation->m_boneList) {
		int index = keyFrames.getPositionIndex(currentTime, boneName);

		float progression = getProgression(keyFrames.positions.at(boneName)[index].first, keyFrames.positions.at(boneName)[index + 1].first, currentTime);
		Vector3f position = getInterpolated(keyFrames.positions.at(boneName)[index].second, keyFrames.positions.at(boneName)[index + 1].second, progression);

		index = keyFrames.getScaleIndex(currentTime, boneName);
		progression = getProgression(keyFrames.scales.at(boneName)[index].first, keyFrames.scales.at(boneName)[index + 1].first, currentTime);
		Vector3f scale = getInterpolated(keyFrames.scales.at(boneName)[index].second, keyFrames.scales.at(boneName)[index + 1].second, progression);

		index = keyFrames.getRotationIndex(currentTime, boneName);
		progression = getProgression(keyFrames.rotations.at(boneName)[index].first, keyFrames.rotations.at(boneName)[index + 1].first, currentTime);
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