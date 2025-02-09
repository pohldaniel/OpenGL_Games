#pragma once

#include <random>

namespace Utils {

	extern std::random_device RandomDevice;
	extern std::mt19937 MersenTwist;
	extern std::uniform_real_distribution<float> Dist;

	extern float random(float range);
}