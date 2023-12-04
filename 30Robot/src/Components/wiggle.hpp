#pragma once

#include "i-random.hpp"

namespace cmpt {
	struct Wiggle {
		Wiggle(float amplitude) : amplitude(amplitude) {
			IRandom& randomService = entt::ServiceLocator<IRandom>::ref();
			noiseOffset = randomService.random(500);
		}

		float amplitude;
		float noiseOffset;
	};
}
