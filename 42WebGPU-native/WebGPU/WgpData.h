#pragma once
#include <array>
#include <engine/Vector.h>

struct Uniforms {
	Matrix4f projectionMatrix;
	Matrix4f viewMatrix;
	Matrix4f modelMatrix;
	std::array<float, 4> color;
	Vector3f camPosition;
	float _pad[1];
};

struct LightingUniforms {
	std::array<Vector4f, 2> directions;
	std::array<Vector4f, 2> colors;
	float hardness = 32.0f;
	float kd = 1.0f;
	float ks = 0.5f;
	float _pad[1];
};