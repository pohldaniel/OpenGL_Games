#pragma once

#include <algorithm>
#include "engine/Vector.h"

class NoiseGen {
public:

	NoiseGen();
	void getGloudShape(unsigned int& cloud_shape);
	void getGloudDetail(unsigned int& cloud_detail);

	bool LoadVolumeFromFile(const char* fileName, unsigned int& tex, int dataSizeX, int dataSizeY, int dataSizeZ);
	bool WriteVolumeToFile(const char* fileName, Vector4f *data, int dataSizeX, int dataSizeY, int dataSizeZ);

	float getPerlinValue(Vector3f texPos, int freq);
	float getWorleyVaule(Vector3f texPos, int freq);
	void genWorleyGrid();
};

