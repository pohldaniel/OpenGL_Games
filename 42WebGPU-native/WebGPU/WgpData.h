#pragma once
#include <array>
#include <engine/Vector.h>

struct Uniforms {
	Matrix4f projectionMatrix;
	Matrix4f viewMatrix;
	Matrix4f modelMatrix;
	std::array<float, 4> color;
};