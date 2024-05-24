#include <Utils/BinaryIO.h>
#include <assimp/Importer.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Animation.h"

void AnimationTrack::findKeyFrameIndex(float time, size_t& index) const {
	if (time < 0.0f)
		time = 0.0f;

	if (index >= m_keyFrames.size())
		index = m_keyFrames.size() - 1;

	// Check for being too far ahead
	while (index && time < m_keyFrames[index].m_time)
		--index;

	// Check for being too far behind
	while (index < m_keyFrames.size() - 1 && time >= m_keyFrames[index + 1].m_time)
		++index;
}

Animation::Animation() : m_length(0.0f) {
}

Animation::~Animation() {

}

void Animation::loadAnimationAni(std::string path) {
	std::ifstream file(path, std::ios::binary);

	std::string ret;
	ret.resize(4);
	file.read(&ret[0], 4 * sizeof(char));
	char metaData[4];

	std::string name;
	while (true) {
		file.read(metaData, sizeof(char));
		if (!metaData[0])
			break;
		else
			name += metaData[0];
	}

	m_animationName = name;
	m_animationNameHash = StringHash(name);

	file.read(metaData, sizeof(float));
	m_length = Utils::bytesToFloatLE(metaData[0], metaData[1], metaData[2], metaData[3]);
	m_tracks.clear();

	file.read(metaData, sizeof(unsigned int));
	unsigned int numTracks = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

	for (unsigned int i = 0; i < numTracks; ++i){

		std::string name;
		while (true) {
			file.read(metaData, sizeof(char));
			if (!metaData[0])
				break;
			else
				name += metaData[0];
		}
		AnimationTrack* newTrack = createTrack(name);

		file.read(metaData, sizeof(unsigned char));
		newTrack->m_channelMask = metaData[0];

		file.read(metaData, sizeof(unsigned int));
		unsigned int numKeyFrames = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
		newTrack->m_keyFrames.resize(numKeyFrames);

		for (size_t j = 0; j < numKeyFrames; ++j){
			AnimationKeyFrame& newKeyFrame = newTrack->m_keyFrames[j];

			file.read(metaData, sizeof(float));
			newKeyFrame.m_time = Utils::bytesToFloatLE(metaData[0], metaData[1], metaData[2], metaData[3]);

			char* buffer = new char[16];

			if (newTrack->m_channelMask & CHANNEL_POSITION) {
				file.read(buffer, 12);
				Utils::UFloat position[3];
				position[0].c[0] = buffer[0]; position[0].c[1] = buffer[1]; position[0].c[2] = buffer[2]; position[0].c[3] = buffer[3];
				position[1].c[0] = buffer[4]; position[1].c[1] = buffer[5]; position[1].c[2] = buffer[6]; position[1].c[3] = buffer[7];
				position[2].c[0] = buffer[8]; position[2].c[1] = buffer[9]; position[2].c[2] = buffer[10]; position[2].c[3] = buffer[11];
				newKeyFrame.m_position.set(position[0].flt, position[1].flt, position[2].flt);
			}

			if (newTrack->m_channelMask & CHANNEL_ROTATION) {
				file.read(buffer, 16);
				Utils::UFloat orientation[4];
				orientation[0].c[0] = buffer[0]; orientation[0].c[1] = buffer[1]; orientation[0].c[2] = buffer[2]; orientation[0].c[3] = buffer[3];
				orientation[1].c[0] = buffer[4]; orientation[1].c[1] = buffer[5]; orientation[1].c[2] = buffer[6]; orientation[1].c[3] = buffer[7];
				orientation[2].c[0] = buffer[8]; orientation[2].c[1] = buffer[9]; orientation[2].c[2] = buffer[10]; orientation[2].c[3] = buffer[11];
				orientation[3].c[0] = buffer[12]; orientation[3].c[1] = buffer[13]; orientation[3].c[2] = buffer[14]; orientation[3].c[3] = buffer[15];
				newKeyFrame.m_rotation.set(orientation[1].flt, orientation[2].flt, orientation[3].flt, orientation[0].flt);
			}

			if (newTrack->m_channelMask & CHANNEL_SCALE) {
				file.read(buffer, 12);
				Utils::UFloat scale[3];
				scale[0].c[0] = buffer[0]; scale[0].c[1] = buffer[1]; scale[0].c[2] = buffer[2]; scale[0].c[3] = buffer[3];
				scale[1].c[0] = buffer[4]; scale[1].c[1] = buffer[5]; scale[1].c[2] = buffer[6]; scale[1].c[3] = buffer[7];
				scale[2].c[0] = buffer[8]; scale[2].c[1] = buffer[9]; scale[2].c[2] = buffer[10]; scale[2].c[3] = buffer[11];
				newKeyFrame.m_scale.set(scale[0].flt, scale[1].flt, scale[2].flt);
			}
			delete buffer;
		}
	}
}

void Animation::loadAnimationAssimp(const std::string &filename, std::string sourceName, std::string destName) {
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
		m_animationNameHash = m_animationName;
		m_length = aiAnimation->mDuration / 1000.0f;
		m_tracks.clear();

		for(unsigned int c = 0; c < aiAnimation->mNumChannels; c++) {
			AnimationTrack* newTrack = createTrack(aiAnimation->mChannels[c]->mNodeName.data);

			newTrack->m_channelMask = CHANNEL_POSITION  + CHANNEL_ROTATION + CHANNEL_SCALE;
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

void Animation::setAnimationName(const std::string& name){
	m_animationName = name;
	m_animationNameHash = StringHash(name);
}

void Animation::setLength(float length){
	m_length = std::max(length, 0.0f);
}

AnimationTrack* Animation::createTrack(const std::string& name){
	StringHash nameHash(name);
	AnimationTrack* oldTrack = findTrack(nameHash);
	if (oldTrack)
		return oldTrack;

	AnimationTrack& newTrack = m_tracks[nameHash];
	newTrack.m_name = name;
	newTrack.m_nameHash = nameHash;
	return &newTrack;
}

void Animation::removeTrack(const std::string& name){
	auto it = m_tracks.find(StringHash(name));
	if (it != m_tracks.end())
		m_tracks.erase(it);
}

void Animation::removeAllTracks(){
	m_tracks.clear();
}

AnimationTrack* Animation::getTrack(size_t index) const{
	if (index >= m_tracks.size())
		return nullptr;

	size_t j = 0;
	for (auto it = m_tracks.begin(); it != m_tracks.end(); ++it){
		if (j == index)
			return const_cast<AnimationTrack*>(&(it->second));
		++j;
	}

	return nullptr;
}

AnimationTrack* Animation::findTrack(const std::string& name_) const{
	auto it = m_tracks.find(StringHash(name_));
	return it != m_tracks.end() ? const_cast<AnimationTrack*>(&(it->second)) : nullptr;
}

AnimationTrack* Animation::findTrack(StringHash nameHash_) const{
	auto it = m_tracks.find(nameHash_);
	return it != m_tracks.end() ? const_cast<AnimationTrack*>(&(it->second)) : nullptr;
}

void Animation::setPositionOfTrack(const std::string& name, const Vector3f& position) {
	AnimationTrack* track = findTrack(name);
	if (track) {
		for (auto& keyFrame : track->m_keyFrames) {
			keyFrame.m_position = position;
		}
	}
}

const std::string& Animation::getAnimationName() const {
	return m_animationName; 
}

StringHash Animation::getAnimationNameHash() const {
	return m_animationNameHash; 
}

float Animation::getLength() const {
	return m_length;
}

const std::map<StringHash, AnimationTrack>& Animation::getTracks() const {
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

void Animation::translateTrack(const std::string& name, const Vector3f& trans) {
	AnimationTrack* track = findTrack(name);
	if (track) {
		for (auto& keyFrame : track->m_keyFrames) {
			keyFrame.m_position.translate(trans);
		}
	}
}

void Animation::translateTrack(const std::string& name, const float dx, const float dy, const float dz) {
	AnimationTrack* track = findTrack(name);
	if (track) {
		for (auto& keyFrame : track->m_keyFrames) {
			keyFrame.m_position.translate(dx, dy, dz);
		}
	}
}

void Animation::setScaleOfTrack(const std::string& name, const Vector3f& scale) {
	AnimationTrack* track = findTrack(name);
	if (track) {
		for (auto& keyFrame : track->m_keyFrames) {
			keyFrame.m_scale = scale;
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

void Animation::setScaleOfTrack(const std::string& name, const float s) {
	AnimationTrack* track = findTrack(name);
	if (track) {
		for (auto& keyFrame : track->m_keyFrames) {
			keyFrame.m_scale.set(s, s, s);
		}
	}
}

void Animation::scaleTrack(const std::string& name, const Vector3f& scale) {
	AnimationTrack* track = findTrack(name);
	if (track) {
		for (auto& keyFrame : track->m_keyFrames) {
			keyFrame.m_scale.scale(scale);
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

void Animation::scaleTrack(const std::string& name, const float s) {
	AnimationTrack* track = findTrack(name);
	if (track) {
		for (auto& keyFrame : track->m_keyFrames) {
			keyFrame.m_scale.scale(s, s, s);
		}
	}
}