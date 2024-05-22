#pragma once

#include <vector>
#include <map>
#include <fstream>
#include <engine/utils/StringHash.h>
#include <engine/Vector.h>

static const unsigned char CHANNEL_POSITION = 1;
static const unsigned char CHANNEL_ROTATION = 2;
static const unsigned char CHANNEL_SCALE = 4;

struct AnimationKeyFrame{

	AnimationKeyFrame() : m_time(0.0f), m_scale(Vector3f::ONE), m_position(Vector3f::ZERO), m_rotation(Quaternion::IDENTITY) {
	}

	float m_time;
	Vector3f m_position;
	Quaternion m_rotation;
	Vector3f m_scale;
};

struct AnimationTrack{

	void findKeyFrameIndex(float time, size_t& index) const;
	std::string m_name;
	StringHash m_nameHash;
	unsigned char m_channelMask;
	std::vector<AnimationKeyFrame> m_keyFrames;
};

class Animation {
	friend class AnimationNode;
	friend class AnimatedModel;
	friend class AnimationController;

public:

	Animation();
	~Animation();

	void loadAnimationAni(std::string path);
	void loadAnimationAssimp(const std::string &filename, std::string sourceName, std::string destName);

	void setAnimationName(const std::string& name);
	void setLength(float length);
	AnimationTrack* createTrack(const std::string& name);
	void removeTrack(const std::string& name);
	void removeAllTracks();


	const std::string& getAnimationName() const;
	StringHash getAnimationNameHash() const;
	float getLength() const;
	const std::map<StringHash, AnimationTrack>& getTracks() const;
	size_t getNumTracks() const;
	AnimationTrack* getTrack(size_t index) const;
	AnimationTrack* findTrack(const std::string& name) const;
	AnimationTrack* findTrack(StringHash nameHash) const;

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

private:

	std::string m_animationName;
	StringHash m_animationNameHash;
	float m_length;
	std::map<StringHash, AnimationTrack> m_tracks;
};
