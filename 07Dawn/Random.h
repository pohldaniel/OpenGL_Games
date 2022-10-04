#pragma once
#include <cstring>

namespace RNG { // RandomNumberGenerator

	void initRNG(unsigned int seed);
	size_t randomSizeT(size_t min, size_t max);
	int randomInt(int min, int max);
	double randomDouble(double min, double max);
}