#pragma once

#include <algorithm>
#include "engine/Vector.h"

class NoiseGen {
public:
	NoiseGen();
	void GetGloudShape(unsigned int& cloud_shape);
	void GetGloudDetail(unsigned int& cloud_detail);
private:
	float GetPerlinValue(Vector3f texPos, int freq);
	float GetWorleyVaule(Vector3f texPos, int freq);
	void GenWorleyGrid();
};

