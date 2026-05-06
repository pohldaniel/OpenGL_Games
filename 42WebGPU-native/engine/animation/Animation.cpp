#include <iostream>
#include <assimp/Importer.hpp> 
#include <assimp/scene.h>

#include "Animation.h"

void AnimationTrack::findKeyFrameIndex(float time, size_t& index) const {
	if (time < 0.0f)
		time = 0.0f;

	if (index >= m_keyFrames.size())
		index = m_keyFrames.size() - 1;

	while (index && time < m_keyFrames[index].m_time)
		--index;

	while (index < m_keyFrames.size() - 1 && time >= m_keyFrames[index + 1].m_time)
		++index;
}

Animation::Animation() : m_length(0.0f) {

}

Animation::~Animation() {

}

void Animation::loadAnimationAssimp(const std::string& filename, std::string sourceName, std::string destName) {
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

		m_animationName = destName;
		m_length = aiAnimation->mDuration / 1000.0f;
		m_tracks.clear();

		for (unsigned int c = 0; c < aiAnimation->mNumChannels; c++) {
			AnimationTrack* newTrack = createTrack(aiAnimation->mChannels[c]->mNodeName.data);

			newTrack->m_channelMask = CHANNEL_POSITION + CHANNEL_ROTATION + CHANNEL_SCALE;
			size_t numKeyFrames = std::max(aiAnimation->mChannels[c]->mNumPositionKeys, std::max(aiAnimation->mChannels[c]->mNumRotationKeys, aiAnimation->mChannels[c]->mNumScalingKeys));		
			newTrack->m_keyFrames.resize(numKeyFrames);

			Vector3f prevPosition;
			Vector3f prevScale;
			Quaternion prevRot;

			for (size_t j = 0; j < numKeyFrames; ++j) {
				AnimationKeyFrame& newKeyFrame = newTrack->m_keyFrames[j];
				newKeyFrame.m_time = numKeyFrames == aiAnimation->mChannels[c]->mNumPositionKeys ? aiAnimation->mChannels[c]->mPositionKeys[j].mTime / 1000.0f : 
					                 numKeyFrames == aiAnimation->mChannels[c]->mNumRotationKeys ? aiAnimation->mChannels[c]->mRotationKeys[j].mTime / 1000.0f :
					                 aiAnimation->mChannels[c]->mScalingKeys[j].mTime / 1000.0f;

				if (j < aiAnimation->mChannels[c]->mNumPositionKeys) {
					newKeyFrame.m_position.set(aiAnimation->mChannels[c]->mPositionKeys[j].mValue.x, aiAnimation->mChannels[c]->mPositionKeys[j].mValue.y, aiAnimation->mChannels[c]->mPositionKeys[j].mValue.z);
					prevPosition = newKeyFrame.m_position;
				}else
					newKeyFrame.m_position.set(prevPosition[0], prevPosition[1], prevPosition[2]);

				if (j < aiAnimation->mChannels[c]->mNumScalingKeys) {
					newKeyFrame.m_scale.set(aiAnimation->mChannels[c]->mScalingKeys[j].mValue.x, aiAnimation->mChannels[c]->mScalingKeys[j].mValue.y, aiAnimation->mChannels[c]->mScalingKeys[j].mValue.z);
					prevScale = newKeyFrame.m_scale;
				}else
					newKeyFrame.m_scale.set(prevScale[0], prevScale[1], prevScale[2]);

				if (j < aiAnimation->mChannels[c]->mNumRotationKeys) {
					aiQuaternion quat = aiAnimation->mChannels[c]->mRotationKeys[j].mValue;
					newKeyFrame.m_rotation.set(quat.x, quat.y, quat.z, quat.w);
					prevRot = newKeyFrame.m_rotation;
				}else
					newKeyFrame.m_rotation.set(prevRot[0], prevRot[1], prevRot[2], prevRot[2]);
			}

		}
	}
}

AnimationTrack* Animation::createTrack(const std::string& name) {
	AnimationTrack* oldTrack = findTrack(name);
	if (oldTrack)
		return oldTrack;

	AnimationTrack& newTrack = m_tracks[name];
	newTrack.m_name = name;
	return &newTrack;
}

AnimationTrack* Animation::findTrack(const std::string& name_) const {
	auto it = m_tracks.find(name_);
	return it != m_tracks.end() ? const_cast<AnimationTrack*>(&(it->second)) : nullptr;
}

const std::string& Animation::getAnimationName() const {
	return m_animationName;
}

float Animation::getLength() const {
	return m_length;
}

const std::map<std::string, AnimationTrack>& Animation::getTracks() const {
	return m_tracks;
}

size_t Animation::getNumTracks() const {
	return m_tracks.size();
}

void Animation::setPositionOfTrack(const std::string& name, const float x, const float y, const float z) {
	AnimationTrack* track = findTrack(name);
	if (track) {
		for (auto& keyFrame : track->m_keyFrames) {
			keyFrame.m_position.set(x, y, z);
		}
	}
}

void Animation::setScaleOfTrack(const std::string& name, const float sx, const float sy, const float sz) {
	AnimationTrack* track = findTrack(name);
	if (track) {
		for (auto& keyFrame : track->m_keyFrames) {
			keyFrame.m_scale.set(sx, sy, sz);
		}
	}
}

void Animation::scaleTrack(const std::string& name, const float sx, const float sy, const float sz) {
	AnimationTrack* track = findTrack(name);
	if (track) {
		for (auto& keyFrame : track->m_keyFrames) {
			keyFrame.m_scale.scale(sx, sy, sz);
		}
	}
}