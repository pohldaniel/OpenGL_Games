#pragma once

#include <map>
#include <string>
#include <vector>

#include <assimp/scene.h>

#include "engine/Vector.h"

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

	double time;
	std::map<std::string, AssimpBoneTransformData> pose;

	struct greater_than {
		bool operator()(const double b, const AssimpKeyFrameData &a) {
			return a.time > b;
		}
	};
};

class AssimpAnimation {

	friend class Animator;

public:
	AssimpAnimation() = default;
	virtual ~AssimpAnimation() {}

	inline std::string getName() { return m_name; }
	inline float getDuration() { return m_duration; }

//private:

	std::string m_name;
	std::vector<AssimpKeyFrameData> m_keyFrames;
	float m_duration;


};