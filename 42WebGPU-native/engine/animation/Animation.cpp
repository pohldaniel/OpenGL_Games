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
			unsigned int numKeyFrames = aiAnimation->mChannels[c]->mNumPositionKeys;
			newTrack->m_keyFrames.resize(numKeyFrames);

			for (size_t j = 0; j < numKeyFrames; ++j) {
				AnimationKeyFrame& newKeyFrame = newTrack->m_keyFrames[j];
				newKeyFrame.m_time = aiAnimation->mChannels[c]->mPositionKeys[j].mTime / 1000.0f;
				newKeyFrame.m_position.set(aiAnimation->mChannels[c]->mPositionKeys[j].mValue.x, aiAnimation->mChannels[c]->mPositionKeys[j].mValue.y, aiAnimation->mChannels[c]->mPositionKeys[j].mValue.z);
				newKeyFrame.m_scale.set(aiAnimation->mChannels[c]->mScalingKeys[j].mValue.x, aiAnimation->mChannels[c]->mScalingKeys[j].mValue.y, aiAnimation->mChannels[c]->mScalingKeys[j].mValue.z);
				aiQuaternion quat = aiAnimation->mChannels[c]->mRotationKeys[j].mValue;
				newKeyFrame.m_rotation.set(quat.x, quat.y, quat.z, quat.w);
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