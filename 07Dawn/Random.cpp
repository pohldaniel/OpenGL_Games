#include <cstdlib>
#include <cmath>
#include <iostream>
#include "random.h"

namespace RNG {
	void initRNG(time_t seed) {
		srand(static_cast<unsigned int>(seed));
	}

	size_t randomSizeT(size_t min, size_t max) {
		return min + (static_cast<size_t>((max - min + 1) * static_cast<double>(rand()) / static_cast<double>(RAND_MAX + 1.0)));
	}

	int randomInt(int min, int max) {
		return min + (static_cast<int>((max - min + 1) * static_cast<double>(rand()) / static_cast<double>(RAND_MAX + 1.0)));
	}

	double randomDouble(double min, double max) {
		return min + ((max - min) * static_cast<double>(rand()) / static_cast<double>(RAND_MAX));
	}
}
