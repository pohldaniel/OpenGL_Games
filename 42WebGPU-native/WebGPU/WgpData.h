#pragma once
#include <array>
#include <engine/Vector.h>

struct Uniforms {
	Matrix4f projectionMatrix;
	Matrix4f viewMatrix;
	Matrix4f modelMatrix;
	std::array<float, 4> color;
};

struct UniformsCompute {
	Matrix4f kernel;
	float test = 0.5f;
	uint32_t filterType = 0;
	float _pad[2];
};