#pragma once

#include <algorithm>
#include "engine/Vector.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class NoiseGen {
public:

	NoiseGen();
	void getGloudShape(unsigned int& cloud_shape);
	void getGloudDetail(unsigned int& cloud_detail);

private:

	float getPerlinValue(Vector3f texPos, int freq);
	float getPerlinValue(glm::vec3 texPos, int freq);
	float getWorleyVaule(Vector3f texPos, int freq);
	void genWorleyGrid();
};

