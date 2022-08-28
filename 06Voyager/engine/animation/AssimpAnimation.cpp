#include <iostream>
#include "AssimpAnimation.h"

void AssimpAnimation::loadAnimationFbx(const std::string &filename, std::string sourceName, std::string destName, unsigned int animationOffset, unsigned int animationCuttOff, unsigned int timeShift) {
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
				m_keyFrames[k - animationOffset].pose.insert(std::pair<std::string, AssimpBoneTransformData>(boneNameId, AssimpBoneTransformData(boneNameId, position, Quaternion(-quat.x, -quat.y, -quat.z, quat.w), scale)));
			}
		}
	}
}

void AssimpAnimation::loadAnimationDae(const std::string &filename, std::string sourceName, std::string destName, unsigned int animationOffset, unsigned int animationCuttOff) {
	
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
		
		m_ticksPerSecond = aiAnimation->mTicksPerSecond;
		m_duration = (animationCuttOff == 0 && animationOffset == 0) ? aiAnimation->mDuration : 0;
		m_name = destName;

		for (unsigned int c = 0; c < aiAnimation->mNumChannels; c++) {
			m_animationTicks = std::max(aiAnimation->mChannels[c]->mNumPositionKeys, std::max(aiAnimation->mChannels[c]->mNumScalingKeys, aiAnimation->mChannels[c]->mNumRotationKeys));
			animationCuttOff = animationCuttOff == 0 ? m_animationTicks : animationCuttOff;
		} 

		bool isResized = false;
		for (unsigned int c = 0; c < aiAnimation->mNumChannels; c++) {
			if (!isResized) {
				m_keyFrames.resize(animationCuttOff - animationOffset);
				isResized = true;
			}

			std::string boneNameId = aiAnimation->mChannels[c]->mNodeName.data;

			for (unsigned int k = animationOffset; k < animationCuttOff; k++) {

				if(m_animationTicks == aiAnimation->mChannels[c]->mNumPositionKeys) {
					m_keyFrames[k - animationOffset].time = aiAnimation->mChannels[c]->mPositionKeys[k - animationOffset].mTime;
				}else if(m_animationTicks == aiAnimation->mChannels[c]->mNumScalingKeys) {
					m_keyFrames[k - animationOffset].time = aiAnimation->mChannels[c]->mScalingKeys[k - animationOffset].mTime;
				}else {
					m_keyFrames[k - animationOffset].time = aiAnimation->mChannels[c]->mRotationKeys[k - animationOffset].mTime;
				}

				if (animationOffset != 0 || animationCuttOff != 0) {
					m_duration = std::max(m_duration, m_keyFrames[k - animationOffset].time);
				}
				
				Vector3f position = aiAnimation->mChannels[c]->mNumPositionKeys == 1 ? Vector3f(aiAnimation->mChannels[c]->mPositionKeys[0].mValue.x, aiAnimation->mChannels[c]->mPositionKeys[0].mValue.y, aiAnimation->mChannels[c]->mPositionKeys[0].mValue.z) : Vector3f(aiAnimation->mChannels[c]->mPositionKeys[k].mValue.x, aiAnimation->mChannels[c]->mPositionKeys[k].mValue.y, aiAnimation->mChannels[c]->mPositionKeys[k].mValue.z);
				Vector3f scale = aiAnimation->mChannels[c]->mNumScalingKeys == 1 ? Vector3f(aiAnimation->mChannels[c]->mScalingKeys[0].mValue.x, aiAnimation->mChannels[c]->mScalingKeys[0].mValue.z, aiAnimation->mChannels[c]->mScalingKeys[0].mValue.y) : Vector3f(aiAnimation->mChannels[c]->mScalingKeys[k].mValue.x, aiAnimation->mChannels[c]->mScalingKeys[k].mValue.z, aiAnimation->mChannels[c]->mScalingKeys[k].mValue.y);
				aiQuaternion quat = aiAnimation->mChannels[c]->mNumRotationKeys == 1 ? aiAnimation->mChannels[c]->mRotationKeys[0].mValue : aiAnimation->mChannels[c]->mRotationKeys[k].mValue;

				//becarefull use the conjugated quternion <-> transpose the rotation matrix
				m_keyFrames[k - animationOffset].pose.insert(std::pair<std::string, AssimpBoneTransformData>(boneNameId, AssimpBoneTransformData(boneNameId, position, Quaternion(-quat.x, -quat.y, -quat.z, quat.w), scale)));
			}
		}		
	}
}