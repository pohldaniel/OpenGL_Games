#include "random.h"

#include <cstdlib>
#include <cmath>

namespace RNG
{
	void initRNG(unsigned int seed)
	{
		srand(seed);
	}

	size_t randomSizeT(size_t min, size_t max)
	{
		return min + (static_cast<size_t>((max - min + 1) * static_cast<double>(rand()) / static_cast<double>(RAND_MAX + 1.0)));
	}

	int randomInt(int min, int max)
	{
		return randomSizeT(0, max - min) + min;
	}

	double randomDouble(double min, double max)
	{
		return min + ((max - min) * static_cast<double>(rand()) / static_cast<double>(RAND_MAX));
	}

}
