#pragma once

#include <vector>
#include <map>
#include <fstream>
#include <engine/Vector.h>
#include "StringHash.h"

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h>

static const unsigned char CHANNEL_POSITION = 1;
static const unsigned char CHANNEL_ROTATION = 2;
static const unsigned char CHANNEL_SCALE = 4;

struct AnimationKeyFrame{

	AnimationKeyFrame() :time(0.0f),scale(Vector3f::ONE) {
	}

	float time;
	Vector3f position;
	Quaternion rotation;
	Vector3f scale;
};

struct AnimationTrack{

	void FindKeyFrameIndex(float time, size_t& index) const;
	std::string name;
	StringHash nameHash;
	unsigned char channelMask;
	std::vector<AnimationKeyFrame> keyFrames;
};

class Animation {


public:

	Animation();
	~Animation();

	static void RegisterObject();

	void loadAni(std::string path);
	void loadAni2(const std::string &filename, std::string sourceName, std::string destName);

	/// Load animation from a stream. Return true on success.
	//bool BeginLoad(Stream& source) override;

	void SetAnimationName(const std::string& name);
	void SetLength(float length);
	AnimationTrack* CreateTrack(const std::string& name);
	void RemoveTrack(const std::string& name);
	void RemoveAllTracks();


	const std::string& AnimationName() const { return animationName; }
	StringHash AnimationNameHash() const { return animationNameHash; }
	float Length() const { return length; }
	const std::map<StringHash, AnimationTrack>& Tracks() const { return tracks; }
	size_t NumTracks() const { return tracks.size(); }
	AnimationTrack* Track(size_t index) const;
	AnimationTrack* FindTrack(const std::string& name) const;
	AnimationTrack* FindTrack(StringHash nameHash) const;

private:

	std::string animationName;
	StringHash animationNameHash;
	float length;
	std::map<StringHash, AnimationTrack> tracks;
};
