#pragma once
#include <vector>
#include "engine/Extension.h"
#include "engine/Vector.h"
#include "engine/Shader.h"

#include "Constants.h"
#include "Camera.h"

class Water {

public:

	Water(const float& dt, const float& fdt);
	virtual ~Water();

	void create(unsigned int resolution, unsigned int width);
	void createBuffer();
	void generateVertices(std::vector<float>& vertexBuffer);
	void generateIndices(std::vector<unsigned int>& indexBuffer);

	void setGridPosition(int x, int z);

	void render(const Camera& camera, unsigned int &reflectionTexture, unsigned int &refractionTexture, unsigned int &refractionDepthTexture);

	Shader* m_waterShader;
	unsigned int m_width;
	unsigned int m_resolution;
	unsigned int m_totalVertices;
	unsigned int m_totalIndices;
	float m_gridSpacing;
	const float WAVE_SPEED = 0.03f;
	float move = 0.0f;
	std::unordered_map<std::string, Texture*> m_textures;

	unsigned int m_vbo, m_ibo, m_vao;
	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;

	Matrix4f m_model;
	std::vector<Matrix4f> modelMTX;

	const float& m_fdt;
	const float& m_dt;

	Matrix4f m_biasMatrix = Matrix4f(0.5, 0.0, 0.0, 0.5,
		0.0, 0.5, 0.0, 0.5,
		0.0, 0.0, 0.5, 0.5,
		0.0, 0.0, 0.0, 1.0);
};