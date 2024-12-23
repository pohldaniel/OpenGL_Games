#pragma once

#include <vector>
#include <map>
#include <fstream>
#include "../utils/StringHash.h"
#include "../Vector.h"

static const unsigned char CHANNEL_POSITION = 1;
static const unsigned char CHANNEL_ROTATION = 2;
static const unsigned char CHANNEL_SCALE = 4;

struct AnimationKeyFrame{

	AnimationKeyFrame() :time(0.0f), scale(Vector3f::ONE), position(Vector3f::ZERO), rotation(Quaternion::IDENTITY) {
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

	void loadAnimationAni(std::string path);
	void loadAnimationAssimp(const std::string &filename, std::string sourceName, std::string destName);

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

	void setPositionOfTrack(const std::string& name, const Vector3f& position);
	void setPositionOfTrack(const std::string& name, const float x, const float y, const float z);

	void translateTrack(const std::string& name, const Vector3f& trans);
	void translateTrack(const std::string& name, const float dx, const float dy, const float dz);

	void setScaleOfTrack(const std::string& name, const Vector3f& scale);
	void setScaleOfTrack(const std::string& name, const float sx, const float sy, const float sz);	
	void setScaleOfTrack(const std::string& name, const float s);

	void scaleTrack(const std::string& name, const Vector3f& scale);
	void scaleTrack(const std::string& name, const float sx, const float sy, const float sz);
	void scaleTrack(const std::string& name, const float s);

//private:

	std::string animationName;
	StringHash animationNameHash;
	float length;
	std::map<StringHash, AnimationTrack> tracks;
};
