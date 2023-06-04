#ifndef SCENELEMENTS_H
#define SCENELEMENTS_H

#include <glm/glm.hpp>
#include <random>

#include "camera.h"

struct sceneElements {

	glm::vec3 lightPos, lightColor, lightDir, fogColor, seed;
	glm::mat4 projMatrix;
	Camera2 * cam;
	bool wireframe = false;
};

#endif