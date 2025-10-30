#pragma once

#include <engine/Camera.h>

class Level {

public:

	Level(const Camera& camera);
	~Level();

private:

	const Camera& camera;
};