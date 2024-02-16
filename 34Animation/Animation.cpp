#include <Utils/SolidIO.h>
#include "Animation.h"

void AnimationTrack::FindKeyFrameIndex(float time, size_t& index) const {
	if (time < 0.0f)
		time = 0.0f;

	if (index >= keyFrames.size())
		index = keyFrames.size() - 1;

	// Check for being too far ahead
	while (index && time < keyFrames[index].time)
		--index;

	// Check for being too far behind
	while (index < keyFrames.size() - 1 && time >= keyFrames[index + 1].time)
		++index;
}

Animation::Animation() :length(0.0f) {
}

Animation::~Animation() {
}

void Animation::loadAni(std::string path) {
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

	animationName = name;
	animationNameHash = animationName;

	file.read(metaData, sizeof(float));
	length = Utils::bytesToFloatLE(metaData[0], metaData[1], metaData[2], metaData[3]);
	tracks.clear();

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
		AnimationTrack* newTrack = CreateTrack(name);

		file.read(metaData, sizeof(unsigned char));
		newTrack->channelMask = metaData[0];

		file.read(metaData, sizeof(unsigned int));
		unsigned int numKeyFrames = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
		newTrack->keyFrames.resize(numKeyFrames);

		for (size_t j = 0; j < numKeyFrames; ++j){
			AnimationKeyFrame& newKeyFrame = newTrack->keyFrames[j];

			file.read(metaData, sizeof(float));
			newKeyFrame.time = Utils::bytesToFloatLE(metaData[0], metaData[1], metaData[2], metaData[3]);

			char* buffer = new char[16];

			if (newTrack->channelMask & CHANNEL_POSITION) {
				file.read(buffer, 12);
				Utils::UFloat position[3];
				position[0].c[0] = buffer[0]; position[0].c[1] = buffer[1]; position[0].c[2] = buffer[2]; position[0].c[3] = buffer[3];
				position[1].c[0] = buffer[4]; position[1].c[1] = buffer[5]; position[1].c[2] = buffer[6]; position[1].c[3] = buffer[7];
				position[2].c[0] = buffer[8]; position[2].c[1] = buffer[9]; position[2].c[2] = buffer[10]; position[2].c[3] = buffer[11];
				newKeyFrame.position.set(position[0].flt, position[1].flt, position[2].flt);
			}

			if (newTrack->channelMask & CHANNEL_ROTATION) {
				file.read(buffer, 16);
				Utils::UFloat orientation[4];
				orientation[0].c[0] = buffer[0]; orientation[0].c[1] = buffer[1]; orientation[0].c[2] = buffer[2]; orientation[0].c[3] = buffer[3];
				orientation[1].c[0] = buffer[4]; orientation[1].c[1] = buffer[5]; orientation[1].c[2] = buffer[6]; orientation[1].c[3] = buffer[7];
				orientation[2].c[0] = buffer[8]; orientation[2].c[1] = buffer[9]; orientation[2].c[2] = buffer[10]; orientation[2].c[3] = buffer[11];
				orientation[3].c[0] = buffer[12]; orientation[3].c[1] = buffer[13]; orientation[3].c[2] = buffer[14]; orientation[3].c[3] = buffer[15];
				newKeyFrame.rotation.set(orientation[1].flt, orientation[2].flt, orientation[3].flt, orientation[0].flt);
			}

			if (newTrack->channelMask & CHANNEL_SCALE) {
				file.read(buffer, 12);
				Utils::UFloat scale[3];
				scale[0].c[0] = buffer[0]; scale[0].c[1] = buffer[1]; scale[0].c[2] = buffer[2]; scale[0].c[3] = buffer[3];
				scale[1].c[0] = buffer[4]; scale[1].c[1] = buffer[5]; scale[1].c[2] = buffer[6]; scale[1].c[3] = buffer[7];
				scale[2].c[0] = buffer[8]; scale[2].c[1] = buffer[9]; scale[2].c[2] = buffer[10]; scale[2].c[3] = buffer[11];
				newKeyFrame.scale.set(scale[0].flt, scale[1].flt, scale[2].flt);
			}
			delete buffer;
		}
	}
}

void Animation::loadAni2(const std::string &filename, std::string sourceName, std::string destName) {
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

		animationName = destName;
		animationNameHash = animationName;
		length = aiAnimation->mDuration / 1000.0f;
		tracks.clear();

		for(unsigned int c = 0; c < aiAnimation->mNumChannels; c++) {

			AnimationTrack* newTrack = CreateTrack(aiAnimation->mChannels[c]->mNodeName.data);
			newTrack->channelMask = CHANNEL_POSITION  + CHANNEL_ROTATION + CHANNEL_SCALE;



			unsigned int numKeyFrames = aiAnimation->mChannels[c]->mNumPositionKeys;

			newTrack->keyFrames.resize(numKeyFrames);
			for (size_t j = 0; j < numKeyFrames; ++j) {
				AnimationKeyFrame& newKeyFrame = newTrack->keyFrames[j];
				newKeyFrame.time = aiAnimation->mChannels[c]->mPositionKeys[j].mTime / 1000.0f;

				//std::cout << "Num Key1: " << newKeyFrame.time << std::endl;

				newKeyFrame.position.set(aiAnimation->mChannels[c]->mPositionKeys[j].mValue.x, aiAnimation->mChannels[c]->mPositionKeys[j].mValue.y, aiAnimation->mChannels[c]->mPositionKeys[j].mValue.z);
				newKeyFrame.scale.set(aiAnimation->mChannels[c]->mScalingKeys[j].mValue.x, aiAnimation->mChannels[c]->mScalingKeys[j].mValue.y, aiAnimation->mChannels[c]->mScalingKeys[j].mValue.z);
				
				aiQuaternion quat = aiAnimation->mChannels[c]->mRotationKeys[j].mValue;
				newKeyFrame.rotation.set(quat.x, quat.y, quat.z, quat.w);
			}
			
		}
	}
}

void Animation::SetAnimationName(const std::string& name_){
	animationName = name_;
	animationNameHash = StringHash(name_);
}

void Animation::SetLength(float length_){
	length = std::max(length_, 0.0f);
}

AnimationTrack* Animation::CreateTrack(const std::string& name_){
	StringHash nameHash_(name_);
	AnimationTrack* oldTrack = FindTrack(nameHash_);
	if (oldTrack)
		return oldTrack;

	AnimationTrack& newTrack = tracks[nameHash_];
	newTrack.name = name_;
	newTrack.nameHash = nameHash_;
	return &newTrack;
}

void Animation::RemoveTrack(const std::string& name_){
	auto it = tracks.find(StringHash(name_));
	if (it != tracks.end())
		tracks.erase(it);
}

void Animation::RemoveAllTracks(){
	tracks.clear();
}

AnimationTrack* Animation::Track(size_t index) const{
	if (index >= tracks.size())
		return nullptr;

	size_t j = 0;
	for (auto it = tracks.begin(); it != tracks.end(); ++it){
		if (j == index)
			return const_cast<AnimationTrack*>(&(it->second));
		++j;
	}

	return nullptr;
}

AnimationTrack* Animation::FindTrack(const std::string& name_) const{
	auto it = tracks.find(StringHash(name_));
	return it != tracks.end() ? const_cast<AnimationTrack*>(&(it->second)) : nullptr;
}

AnimationTrack* Animation::FindTrack(StringHash nameHash_) const{
	auto it = tracks.find(nameHash_);
	return it != tracks.end() ? const_cast<AnimationTrack*>(&(it->second)) : nullptr;
}
