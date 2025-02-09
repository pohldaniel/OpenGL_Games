#include "RandomNew.h"

std::random_device Utils::RandomDevice;
std::mt19937 Utils::MersenTwist(RandomDevice());
std::uniform_real_distribution<float> Utils::Dist(0.0f, 1.0f);

float Utils::random(float range) {
	return Dist(MersenTwist) * range;
}