#pragma once
#include <vector>
#include "XYZ.h"
#include "Joint.h"
#include "Muscle.h"
#include "Model.h"

const int max_joints = 50;

class Skeleton{
public:

	const float multiplier = 0.0f;
	const float gravity = 0.0f;

	std::vector<Joint> joints;

	std::vector<Muscle> muscles;

	int selected;

	int forwardjoints[3];
	XYZ forward;

	int id;

	int lowforwardjoints[3];
	XYZ lowforward;

	XYZ specialforward[5];
	int jointlabels[max_joints];

	Model model[7];
	Model modellow;
	Model modelclothes;
	int num_models;

	Model drawmodel;
	Model drawmodellow;
	Model drawmodelclothes;

	bool clothes;
	bool spinny;

	GLubyte skinText[512 * 512 * 3];
	int skinsize;

	float checkdelay;

	float longdead;
	bool broken;

	int free;
	int oldfree;
	float freetime;
	bool freefall;

	void FindForwards();
	void DoGravity(float* scale);
	void FindRotationMuscle(int which, int animation);
	void Load(const std::string& fileName, const std::string& lowfileName, const std::string& clothesfileName, const std::string& modelfileName, const std::string& model2fileName, const std::string& model3fileName, const std::string& model4fileName, const std::string& model5fileNamee, const std::string& model6fileName, const std::string& model7fileName, const std::string& modellowfileName, const std::string& modelclothesfileName, bool aclothes);

	Skeleton();

private:
	// convenience functions
	// only for Skeleton.cpp
	inline Joint& joint(int bodypart) { return joints[jointlabels[bodypart]]; }
	inline XYZ& jointPos(int bodypart) { return joint(bodypart).position; }
	inline XYZ& jointVel(int bodypart) { return joint(bodypart).velocity; }

	inline Joint& joint2(int bodypart) {
		return joints[jointlabels[bodypart]];
	}
};