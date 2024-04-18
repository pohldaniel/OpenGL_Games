#pragma once
#include <engine/Shader.h>
#include <engine/Camera.h>

class Grasses {
	unsigned int grass_vao_ = 0;
	Shader* grass_shader;
	Shader* grass_compute_shader;
	unsigned int blades_count_ = 0;

public:
	// Wind parameters
	float wind_magnitude = 1.0;
	float wind_wave_length = 1.0;
	float wind_wave_period = 1.0;

	void init();
	void update(const Camera& camera, float dt);
	void render(const Camera& camera);
};