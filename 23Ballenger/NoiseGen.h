#pragma once

#include <algorithm>
#include "engine/Vector.h"

class NoiseGen {
public:

	NoiseGen();
	void getGloudShape(unsigned int& cloud_shape);
	void getGloudDetail(unsigned int& cloud_detail);

private:

	float getPerlinValue(Vector3f texPos, int freq);
	float getWorleyVaule(Vector3f texPos, int freq);
	void genWorleyGrid();
};

