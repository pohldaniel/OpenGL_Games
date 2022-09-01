#pragma once

#include <map>
#include <string>
#include <vector>
#include <unordered_map>

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Vector.h"

typedef std::pair<float, Vector3f> PositionKey;
typedef PositionKey ScaleKey;
typedef std::pair<float, Quaternion> RotationKey;

struct AssimpKeyFrameData {
	std::map<std::string, std::vector<PositionKey>> positions;
	std::map<std::string, std::vector<ScaleKey>> scales;
	std::map<std::string, std::vector<RotationKey>> rotations;
	
	unsigned int getPositionIndex(float animationTime, std::string boneName){		
		for (unsigned int index = positions[boneName].size() - 1; index >= 0; --index) {			
			if (animationTime >= positions[boneName][index].first) {				
				return index;
			}
		}		
	}

	unsigned int getScaleIndex(float animationTime, std::string boneName) {

		for (unsigned int index = scales[boneName].size() - 1; index >= 0; --index) {
			if (animationTime >= scales[boneName][index].first) {
				return index;
			}
		}		
	}

	unsigned int getRotationIndex(float animationTime, std::string boneName) {
		for (unsigned int index = rotations[boneName].size() - 1; index >= 0; --index) {
			if (animationTime >= rotations[boneName][index].first) {
				return index;
			}
		}		
	}
};
class AssimpAnimatedMesh;
class AssimpAnimation {

	friend class Animator;

public:
	void loadAnimationFbx(const std::string &filename, std::string sourceName, std::string destName, unsigned int animationOffset = 50, unsigned int animationCuttOff = 230, unsigned int timeShift = 0);
	void loadAnimationDae(const std::string &filename, std::string sourceName, std::string destName, bool looping = false, unsigned int animationOffset = 0, unsigned int animationCuttOff = 0, float timeScale = 1.0f);

	AssimpAnimation() = default;
	virtual ~AssimpAnimation() {}

	inline std::string getName() { return m_name; }
	inline float getDuration() { return m_duration; }

//private:

	std::string m_name;
	float m_duration = 0.0f;
	unsigned int m_animationTicks = 0;
	float m_ticksPerSecond = 0;
	bool m_looping = true;

	AssimpKeyFrameData m_keyFrames;
	std::vector<std::string> m_boneList;
	
	float m_startTime = 0.0f;
	float m_endTime = 0.0f;
	
};