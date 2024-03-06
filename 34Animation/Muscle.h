#ifndef _MUSCLE_HPP_
#define _MUSCLE_HPP_

#include "Joint.h"
#include <vector>
#include <engine/Vector.h>

enum muscle_type{
	boneconnect,
	constraint,
	muscle
};

class Muscle{
public:

	const float multiplier = 0.0f;
	const float freeze = 0.0f;

	std::vector<int> vertices;
	std::vector<int> verticeslow;
	std::vector<int> verticesclothes;
	float length;
	float targetlength;
	Joint* parent1;
	Joint* parent2;
	float maxlength;
	float minlength;
	muscle_type type;
	bool visible;
	float rotate1, rotate2, rotate3;
	float lastrotate1, lastrotate2, lastrotate3;
	float oldrotate1, oldrotate2, oldrotate3;
	float newrotate1, newrotate2, newrotate3;

	float strength;

	Muscle();
	void load(FILE* tfile, int vertexNum, std::vector<Joint>& joints);
	void loadVerticesLow(FILE* tfile, int vertexNum);
	void loadVerticesClothes(FILE* tfile, int vertexNum);
	void DoConstraint(bool spinny);

	Matrix4f m_worldTransform;
};

#endif
