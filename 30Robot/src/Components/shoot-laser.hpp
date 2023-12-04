#pragma once

#include "i-random.hpp"

namespace cmpt {
	struct ShootLaser {
		ShootLaser(bool isActiv=false) : isActiv(isActiv) {
			IRandom& random = entt::ServiceLocator<IRandom>::ref();
			col = glm::vec3(122 , 249 + random.random(-150, 0), 237);
		}
		bool isActiv;
		glm::vec3 col;
	};
}
