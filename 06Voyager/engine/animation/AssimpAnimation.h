#pragma once

#include <map>
#include <string>
#include <vector>

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Vector.h"

struct AssimpBoneTransformData {

	std::string jointNameId;
	Vector3f positonKeys;
	Quaternion rotationKeys;
	Vector3f scallingKeys;

	AssimpBoneTransformData(std::string _jointNameId, Vector3f _positonKeys, Quaternion _rotationKeys, Vector3f _scallingKeys) {
		jointNameId = _jointNameId;
		positonKeys = _positonKeys;
		rotationKeys = _rotationKeys;
		scallingKeys = _scallingKeys;
	}
};

struct AssimpKeyFrameData {

	float time;
	std::map<std::string, AssimpBoneTransformData> pose;

	struct greater_than {
		bool operator()(const float b, const AssimpKeyFrameData &a) {
			return a.time > b;
		}
	};
};

class AssimpAnimation {

	friend class Animator;

public:
	void loadAnimationFbx(const std::string &filename, std::string sourceName, std::string destName, unsigned int animationOffset = 50, unsigned int animationCuttOff = 230, unsigned int timeShift = 0);
	void loadAnimationDae(const std::string &filename, std::string sourceName, std::string destName, unsigned int animationOffset = 0, unsigned int animationCuttOff = 0);

	AssimpAnimation() = default;
	virtual ~AssimpAnimation() {}

	inline std::string getName() { return m_name; }
	inline float getDuration() { return m_duration; }

//private:

	std::string m_name;
	std::vector<AssimpKeyFrameData> m_keyFrames;
	float m_duration = 0.0f;
	unsigned int m_animationTicks = 0;
	unsigned int m_ticksPerSecond = 0;
};