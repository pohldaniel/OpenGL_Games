#include <iostream>
#include "AssimpAnimation.h"
#include "AssimpAnimatedModel.h"

void AssimpAnimation::loadAnimationFbx(const std::string &filename, std::string sourceName, std::string destName, unsigned int animationOffset, unsigned int animationCuttOff, unsigned int timeShift) {
	Assimp::Importer Importer;
	const aiScene* aiScene = Importer.ReadFile(filename, NULL);

	if (!aiScene) {
		std::cout << filename << "  " << Importer.GetErrorString() << std::endl;
		return;
	}

	/*for (unsigned int i = 0; i < aiScene->mNumAnimations; i++) {
	const aiAnimation* aiAnimation = aiScene->mAnimations[i];

	if (sourceName.compare(aiAnimation->mName.data) != 0) {
	continue;
	}
	std::cout << aiAnimation->mName.data << std::endl;
	m_name = destName;
	m_ticksPerSecond = aiAnimation->mTicksPerSecond;
	for (unsigned int c = 0; c < aiAnimation->mNumChannels; c++) {
	m_animationTicks = std::max(aiAnimation->mChannels[c]->mNumPositionKeys, std::max(aiAnimation->mChannels[c]->mNumScalingKeys, aiAnimation->mChannels[c]->mNumRotationKeys));
	}
	m_duration = m_animationTicks;
	m_duration = animationCuttOff - (animationOffset + 5);
	bool isResized = false;
	for (unsigned int c = 0; c < aiAnimation->mNumChannels; c++) {
	if (!isResized) {
	m_keyFrames.resize(animationCuttOff - animationOffset);
	isResized = true;
	}
	std::string boneNameId = aiAnimation->mChannels[c]->mNodeName.data;
	for (unsigned int k = animationOffset; k < animationCuttOff; k++) {
	if (m_animationTicks == aiAnimation->mChannels[c]->mNumPositionKeys) {
	m_keyFrames[k - animationOffset].time = aiAnimation->mChannels[c]->mPositionKeys[k - animationOffset].mTime - timeShift;
	}
	else if (m_animationTicks == aiAnimation->mChannels[c]->mNumScalingKeys) {
	m_keyFrames[k - animationOffset].time = aiAnimation->mChannels[c]->mScalingKeys[k - animationOffset].mTime - timeShift;
	}
	else {
	m_keyFrames[k - animationOffset].time = aiAnimation->mChannels[c]->mRotationKeys[k - animationOffset].mTime - timeShift;
	}
	Vector3f position = aiAnimation->mChannels[c]->mNumPositionKeys == 1 ? Vector3f(aiAnimation->mChannels[c]->mPositionKeys[0].mValue.x, aiAnimation->mChannels[c]->mPositionKeys[0].mValue.y, aiAnimation->mChannels[c]->mPositionKeys[0].mValue.z) : Vector3f(aiAnimation->mChannels[c]->mPositionKeys[k].mValue.x, aiAnimation->mChannels[c]->mPositionKeys[k].mValue.y, aiAnimation->mChannels[c]->mPositionKeys[k].mValue.z);
	Vector3f scale = aiAnimation->mChannels[c]->mNumScalingKeys == 1 ? Vector3f(aiAnimation->mChannels[c]->mScalingKeys[0].mValue.x, aiAnimation->mChannels[c]->mScalingKeys[0].mValue.z, aiAnimation->mChannels[c]->mScalingKeys[0].mValue.y) : Vector3f(aiAnimation->mChannels[c]->mScalingKeys[k].mValue.x, aiAnimation->mChannels[c]->mScalingKeys[k].mValue.z, aiAnimation->mChannels[c]->mScalingKeys[k].mValue.y);
	aiQuaternion quat = aiAnimation->mChannels[c]->mNumRotationKeys == 1 ? aiAnimation->mChannels[c]->mRotationKeys[0].mValue : aiAnimation->mChannels[c]->mRotationKeys[k].mValue;
	//becarefull use the conjugated quternion <-> transpose the rotation matrix
	m_keyFrames[k - animationOffset].pose.insert(std::pair<std::string, AssimpBoneTransformData>(boneNameId, AssimpBoneTransformData( position, Quaternion(-quat.x, -quat.y, -quat.z, quat.w), scale)));
	}
	}
	}*/
}

void AssimpAnimation::loadAnimation(const std::string &filename, std::string sourceName, std::string destName, bool looping, unsigned int animationOffset, unsigned int animationCuttOff, float timeScale) {

	Assimp::Importer Importer;
	const aiScene* aiScene = Importer.ReadFile(filename, NULL);

	if (!aiScene) {
		std::cout << filename << "  " << Importer.GetErrorString() << std::endl;
		return;
	}

	for (unsigned int i = 0; i < aiScene->mNumAnimations; i++) {
		
		const aiAnimation* aiAnimation = aiScene->mAnimations[i];
		if (sourceName.compare(aiAnimation->mName.data) != 0) {
			continue;
		}

		m_name = destName;
		m_looping = looping;
		//m_ticksPerSecond = aiAnimation->mTicksPerSecond;
		m_ticksPerSecond = 1.0f;

		m_duration = (animationCuttOff == 0 && animationOffset == 0) ? aiAnimation->mDuration : 0;
		m_startTime = 0.0f;
		m_endTime = m_startTime + m_duration;

		for (unsigned int c = 0; c < aiAnimation->mNumChannels; c++) {
			m_animationTicks = std::max(aiAnimation->mChannels[c]->mNumPositionKeys, std::max(aiAnimation->mChannels[c]->mNumScalingKeys, aiAnimation->mChannels[c]->mNumRotationKeys));
			animationCuttOff = animationCuttOff == 0 ? 0 : m_animationTicks;
		}

		for (unsigned int c = 0; c < aiAnimation->mNumChannels; c++) {
			std::string boneNameId = aiAnimation->mChannels[c]->mNodeName.data;
			m_boneList.push_back(boneNameId);

			unsigned int keyFrames = animationCuttOff == 0 ? aiAnimation->mChannels[c]->mNumPositionKeys : std::min(aiAnimation->mChannels[c]->mNumPositionKeys, animationCuttOff);
			for (unsigned int k = animationOffset; k < keyFrames; k++) {
				Vector3f position = Vector3f(aiAnimation->mChannels[c]->mPositionKeys[k].mValue.x, aiAnimation->mChannels[c]->mPositionKeys[k].mValue.y, aiAnimation->mChannels[c]->mPositionKeys[k].mValue.z);
				m_keyFrames.positions[boneNameId].push_back(PositionKey((float)aiAnimation->mChannels[c]->mPositionKeys[k - animationOffset].mTime / timeScale, position));

				if (animationOffset != 0 || animationCuttOff != 0) {
					m_endTime = std::max(m_endTime, (float)aiAnimation->mChannels[c]->mPositionKeys[k - animationOffset].mTime);
				}
			}

			if (looping) {
				Vector3f position = Vector3f(aiAnimation->mChannels[c]->mPositionKeys[0].mValue.x, aiAnimation->mChannels[c]->mPositionKeys[0].mValue.y, aiAnimation->mChannels[c]->mPositionKeys[0].mValue.z);
				m_keyFrames.positions[boneNameId].push_back(PositionKey((float)aiAnimation->mChannels[c]->mPositionKeys[1 - animationOffset].mTime / timeScale, position));
			}

			keyFrames = animationCuttOff == 0 ? aiAnimation->mChannels[c]->mNumScalingKeys : std::min(aiAnimation->mChannels[c]->mNumScalingKeys, animationCuttOff);
			for (unsigned int k = animationOffset; k < keyFrames; k++) {
				Vector3f scale = Vector3f(aiAnimation->mChannels[c]->mScalingKeys[k].mValue.x, aiAnimation->mChannels[c]->mScalingKeys[k].mValue.y, aiAnimation->mChannels[c]->mScalingKeys[k].mValue.z);
				m_keyFrames.scales[boneNameId].push_back(ScaleKey((float)aiAnimation->mChannels[c]->mScalingKeys[k - animationOffset].mTime / timeScale, scale));

				if (animationOffset != 0 || animationCuttOff != 0) {
					m_endTime = std::max(m_endTime, (float)aiAnimation->mChannels[c]->mPositionKeys[k - animationOffset].mTime);
				}
			}

			if (looping) {
				Vector3f scale = Vector3f(aiAnimation->mChannels[c]->mScalingKeys[0].mValue.x, aiAnimation->mChannels[c]->mScalingKeys[0].mValue.y, aiAnimation->mChannels[c]->mScalingKeys[0].mValue.z);
				m_keyFrames.scales[boneNameId].push_back(PositionKey((float)aiAnimation->mChannels[c]->mScalingKeys[1 - animationOffset].mTime / timeScale, scale));
			}

			keyFrames = animationCuttOff == 0 ? aiAnimation->mChannels[c]->mNumRotationKeys : std::min(aiAnimation->mChannels[c]->mNumRotationKeys, animationCuttOff);
			for (unsigned int k = animationOffset; k < keyFrames; k++) {
				aiQuaternion quat = aiAnimation->mChannels[c]->mRotationKeys[k].mValue;
				m_keyFrames.rotations[boneNameId].push_back(RotationKey((float)aiAnimation->mChannels[c]->mRotationKeys[k - animationOffset].mTime / timeScale, Quaternion(quat.x, quat.y, quat.z, quat.w)));

				if (animationOffset != 0 || animationCuttOff != 0) {
					m_endTime = std::max(m_endTime, (float)aiAnimation->mChannels[c]->mPositionKeys[k - animationOffset].mTime);
				}
			}

			if (looping) {
				aiQuaternion quat = aiAnimation->mChannels[c]->mRotationKeys[0].mValue;
				m_keyFrames.rotations[boneNameId].push_back(RotationKey((float)aiAnimation->mChannels[c]->mRotationKeys[1 - animationOffset].mTime / timeScale, Quaternion(quat.x, quat.y, quat.z, quat.w)));
			}
		}
	}

	m_duration = m_duration / timeScale;
	m_startTime = m_startTime / timeScale;
	m_endTime = m_endTime / timeScale;
}