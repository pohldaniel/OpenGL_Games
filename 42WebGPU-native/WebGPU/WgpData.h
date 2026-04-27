#pragma once
#include <array>
#include <engine/Vector.h>

struct Uniforms {
	Matrix4f projection;
	Matrix4f view;
	Matrix4f env;
	Matrix4f model;
	Matrix4f normal;
	std::array<float, 4> color;
	Vector3f camPosition;
	float _pad0;
	Matrix4f lightVP;
	Matrix4f shadow;
	Vector3f lightPosition;
	float _pad1;
};

struct LightingUniforms {
	std::array<Vector4f, 2> directions;
	std::array<Vector4f, 2> colors;
	float hardness = 32.0f;
	float kd = 1.0f;
	float ks = 0.5f;
	float _pad[1];
};

struct NormalUniforms {
	std::array <float, 3> light_pos_vs;
	uint32_t mode;
	float light_intensity;
	float depth_scale;
	float depth_layers;
	float padding;
};

struct PBRLightingUniforms {
	std::array <float, 3> position;
	float padding1;
	std::array <float, 3> color;
	float padding2;
};

struct MaterialUniforms {
	std::array <float, 4> baseColorFactor;
	float alphaCutoff;
	float padding[3];
};